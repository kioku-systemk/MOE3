//
//  KsceneViewer.cpp
//  MOE3
//
//  Created by kioku on 2013/08/08.
//  Copyright (c) 2013 System K. All rights reserved.
//

#include "Core/MOE.h"
#include "Core/CoreWindow.h"
#include "Gfx/Graphics.h"
#include "EasyMIDIController/EasyMIDIController.h"

#include <stdio.h>
#include <stdlib.h>
#include "Core/Stream.h"
#include "SceneGraph/SceneGraph.h"
#include "SceneGraph/MrzLoader.h"
#include "SceneGraph/AnimLoader.h"
#include "Gfx/SceneGraphRender.h"

#include "Core/Math.h"

#include "Core/UI/SimpleGUI.h"
#include "SceneGraph/SceneGraph.h"

#include "Core/Time.h"
#include "Core/Thread.h"

#include "Demo/Demo.h"

namespace {
	std::string g_mrzfile;
    
    const s8* vname[] = {"ClearColor", "p1","p2","p3","p4"};
    const int paramnum = sizeof(vname)/sizeof(const s8*);
}

class MOEWindow : public CoreWindow
{
public:
	MOEWindow(int x, int y, int width, int height)
	: CoreWindow(x, y, width, height, "KScene3"
	){
        g = mnew MOE::Graphics();

        m_midi = mnew EasyMIDIController();
        m_gui  = mnew SimpleGUI::GUIManager();
        m_gui->Resize(width, height);
        skGUI::BaseWindow* win = m_gui->GetRoot();
        m_width  = width;
        m_height = height;
        
        // TEST
        const f32 col[] = {0.50,0.50,0.50,0.50};
        m_frame1 = mnew SimpleGUI::Frame(m_gui,0,0,width, 30, col);
        m_frame2 = mnew SimpleGUI::Frame(m_gui,0,30,100, height-30, col);
        win->AddChild(m_frame1);
        win->AddChild(m_frame2);

        SimpleGUI::Caption* cap = mnew SimpleGUI::Caption(m_gui, 10, 5, "KScene3", 16);
        m_frame1->AddChild(cap);

		m_timeslider = mnew SimpleGUI::Slider(m_gui, 200,10,width - 220,16);
		m_animcheck = mnew SimpleGUI::Check(m_gui,"Realtime",110,5);
        m_timeval = mnew SimpleGUI::Caption(m_gui, 200,8,"0.000", 16);
        m_timeslider->SetChangedFunc(changeTimerslider_, this);
        m_frame1->AddChild(m_timeval);
    	m_frame1->AddChild(m_timeslider);
		m_frame1->AddChild(m_animcheck);

        for (int i = 0; i < paramnum; ++i)
		{
			SimpleGUI::Caption* name = mnew SimpleGUI::Caption(m_gui, 10, 120 * i, vname[i], 16);
			m_frame2->AddChild(name);
            for (int j = 0; j < 4; ++j) {
                m_bar[i][j] = mnew SimpleGUI::Slider(m_gui, 10, 20 + 20 * j + 120 * i, 80, 16);
                SimpleGUI::Caption* barval = mnew SimpleGUI::Caption(m_gui, 12, 18 + 20* j + 120 * i, "0.000", 16);
                m_frame2->AddChild(barval);
                m_bar[i][j]->SetUserData(barval);
                m_bar[i][j]->SetChangedFunc(changebarParam_, m_bar[i][j]);
                m_frame2->AddChild(m_bar[i][j]);
            }
		}

		m_cameracheck = mnew SimpleGUI::Check(m_gui,"Camera View",5,620);
    	m_frame2->AddChild(m_cameracheck);

        m_openbtn = mnew SimpleGUI::Button(m_gui,"OpenModel",5,height - 100, 90, 16);
        m_openbtn->SetClickedFunc(MOEWindow::openBtnFunc, this);
    	m_frame2->AddChild(m_openbtn);
        m_reloadbtn = mnew SimpleGUI::Button(m_gui,"ReloadModel",5,height - 80, 90, 16);
        m_reloadbtn->SetClickedFunc(MOEWindow::reloadModelBtnFunc, this);
    	m_frame2->AddChild(m_reloadbtn);
        m_rebufbtn = mnew SimpleGUI::Button(m_gui,"ReloadShader",5,height - 60, 90, 16);
        m_rebufbtn->SetClickedFunc(MOEWindow::reloadShaderFunc, this);
    	m_frame2->AddChild(m_rebufbtn);

		m_cameranode = 0;
        m_root = 0;
		m_anim = 0;
        m_srender = new MOE::SceneGraphRender(g);	
		
		setupResourcePath();
		ReloadModels();
 
        m_rot = m_view = MOE::Math::Identity();
        m_zoom = 5.0f;
		m_trans = MOE::Math::vec3(0,0,0);
        
        Fit();

		mx = 0;
        my = 0;
        press = 0;
#if MOE_PLATFORM_WINDOWS
		m_inited = true;
#endif
        Draw();
	}
	~MOEWindow()
    {
    }
    
    static void changebarParam_(float val,void* thisptr){
        SimpleGUI::Caption* cap = static_cast<SimpleGUI::Caption*>(static_cast<SimpleGUI::Slider*>(thisptr)->GetUserData());
        char buf[64];
        sprintf(buf, "%.3f", val);
        cap->SetText(buf);
    }
    static void changeTimerslider_(float val,void* thisptr){
        static_cast<MOEWindow*>(thisptr)->changeTimerslider(val);
    }
    void changeTimerslider(float val){
        char buf[64];
        double animtime = 0.0;
        if (m_anim)
            animtime = m_anim->GetMaxAnimTime();
        sprintf(buf, "%.3lf", static_cast<double>(val) * animtime);
        m_timeval->SetText(buf);
        
    }
    
    
	void ReloadModels()
	{
        delete m_root;
        delete m_anim;
        
		printf("Load model:%s\n", g_mrzfile.c_str());
		MOE::Stream mst(g_mrzfile.c_str(), MOE::Stream::MODE_INPUT_BINARY_ONMEMORY);
        MOE::MrzLoader loader;
        MOE::SceneGraph::Node* node = loader.Load(&mst);
        m_root = node;
      
		m_cameranode = MOE::SceneGraph::FindNode(m_root, "Camera");

		std::string animfile;
		size_t p = g_mrzfile.rfind(".");
		if (p != std::string::npos)
		{
			animfile = g_mrzfile.substr(0,p);
			animfile += "_mrz.anim";
		}
		if (animfile != "") {
			MOE::Stream ast(animfile.c_str(), MOE::Stream::MODE_INPUT_BINARY_ONMEMORY);
			MOE::AnimLoader aloader;
			MOE::Animation* anim = aloader.Load(&ast);
			m_anim = anim;
			if (anim)
				printf("Loaded Animation.\n");
		}
	}
    void Fit()
    {
        MOE::Math::vec3 bmax, bmin;
		MOE::SceneGraph::GetBBox(m_root, bmax, bmin);
        m_trans = (bmax + bmin) * .5;
		m_zoom = MOE::Math::length(bmax - bmin);
    }
    
	class forceupdate{
	public:
		void operator()(MOE::SceneGraph::Geometry* geo)
		{
			geo->EnableNeedUpdate();
		}
	};
	void ReloadBuffers()
	{
		m_srender->Clear();
		forceupdate needupdatefunc;
		MOE::SceneGraph::VisitAllGeometry(m_root,needupdatefunc);
        Draw();
	}

	void setupResourcePath()
	{
#if MOE_PLATFORM_WINDOWS
		const s8* dirchar = "\\";
#else
		const s8* dirchar = "/";
#endif
		size_t np;
		if ((np = g_mrzfile.rfind(dirchar)) != std::string::npos)
		{
			const std::string dirpath = g_mrzfile.substr(0, np+1);
			m_srender->SetResourcePath(dirpath.c_str());
		}
		
	}

    void OpenModel()
    {
        const char* fn = FileOpenDialog("MRZ");
        if (fn)
        {
            g_mrzfile = std::string(fn);
			setupResourcePath();
            ReloadModels();
            Fit();
            m_srender->Clear();
        }
    }
    static void openBtnFunc(void* thisptr){ static_cast<MOEWindow*>(thisptr)->OpenModel(); }
    static void reloadModelBtnFunc(void* thisptr){ static_cast<MOEWindow*>(thisptr)->ReloadModels(); }
    static void reloadShaderFunc(void* thisptr){ static_cast<MOEWindow*>(thisptr)->ReloadBuffers(); }
    
    s32 mx;
    s32 my;
    s32 press;
    f32 m_zoom;
    MOE::Math::vec3 m_trans;
    MOE::Math::matrix m_view;
    MOE::Math::matrix m_rot;
	void MouseLeftDown(int x, int y)
    {
        mx = x;
        my = y;
        if (!m_gui->MouseDown(0, x,y))
			press |= 1;
        Draw();
    }
	void MouseLeftUp(int x, int y)
    {
        press = (press & (~1));
        m_gui->MouseUp(0, x,y);
        Draw();
    }
    void MouseRightDown(int x, int y)
    {
        mx = x;
        my = y;
        press |= 2;
        Draw();
    }
    void MouseRightUp(int x, int y)
    {
        press = (press & (~2));
        Draw();
    }
    void MouseMiddleDown(int x, int y)
    {
        mx = x;
        my = y;
        press |= 4;
        Draw();
    }
    void MouseMiddleUp(int x, int y)
    {
        press = (press & (~4));
        Draw();
    }

	void MouseMove(int x, int y)
    {
        s32 dx = x - mx;
        s32 dy = y - my;
        if ((press & 4) || ((press&1)&&(press&2))) {
            m_trans.x += -dx * 0.02;
            m_trans.y +=  dy * 0.02;
        }
        else if (press & 1) {
            m_rot = MOE::Math::RotationY(dx * 0.2) * MOE::Math::RotationX(-dy * 0.2) * m_rot;
            m_view = MOE::Math::RotationY(dx * 0.2) * MOE::Math::RotationX( dy * 0.2) * m_view;
        }
        else if (press & 2) {
            m_zoom += m_zoom * -(dx+dy)/200.0;
            if (m_zoom < 1.0)
                m_zoom = 1.0;
        }

        mx = x;
        my = y;
        m_gui->MouseMove(x, y);
        Draw();
    }
	void Wheel(float dx, float dy, float dz) {}
	void KeyDown(int key){
		if (key == 27)
			exit(0);
        m_gui->KeyDown(key);
	}
	void KeyUp(int key)
    {
		if (key == 'r' || key == 'R')
			ReloadModels();
        if (key == 's' || key == 'S')
			ReloadBuffers();
        if (key == 'o' || key == 'O')
            OpenModel();
        if (key == 'f' || key == 'F')
            Fit();
        
		m_gui->KeyUp(key);
        Draw();
    }
	
    void updateGUI()
    {

		const int maxmidinum = 8;
		static f32 bval[maxmidinum] = {};
		for (int midictrl = 0; midictrl < maxmidinum; ++midictrl)
		{
			const f32 cv = m_midi->GetControlParam(0);
			if (bval[midictrl] != cv){
				bval[midictrl] = cv;
				m_bar[midictrl / 4 + 1][midictrl % 4]->SetValue(cv);
			}
		}
    }
    
    void Draw()
    {
        updateGUI();
        const float mr = m_bar[0][0]->GetValue();
		const float mg = m_bar[0][1]->GetValue();
		const float mb = m_bar[0][2]->GetValue();
		const float ma = m_bar[0][3]->GetValue();

        g->Enable(VG_CULL_FACE);
        g->Enable(VG_DEPTH_TEST);
        g->ClearColor(mr,mg,mb,ma);
        g->Clear(VG_COLOR_BUFFER_BIT | VG_DEPTH_BUFFER_BIT);

		// Animation
		if (m_anim && m_anim->GetMaxAnimTime() != 0) {
			double maxanimtime = m_anim->GetMaxAnimTime();
			if (m_animcheck->GetState()) {
				f64 tm = fmod(MOE::GetTimeCount(),maxanimtime);
				m_timeslider->SetValue(tm/maxanimtime);
			}
            const f32 animtime = m_timeslider->GetValue()*maxanimtime;
			m_anim->Animate(m_root, animtime);
            
            m_srender->SetUniform("time", MOE::Math::vec4(animtime,animtime,animtime,animtime));
		} else {
            f32 tm = fmod(MOE::GetTimeCount(), 10.0);
            m_srender->SetUniform("time", MOE::Math::vec4(tm,tm,tm,tm));
        }
        
		// View
        using namespace MOE::SceneGraph;
        using namespace MOE::Math;
        f32 fov = 60.0f;
		matrix view = LookAt(vec3(m_trans.x,m_trans.y,m_zoom), vec3(m_trans.x,m_trans.y,0), vec3(0,1,0));
		view = view * m_view;
		if (m_cameracheck->GetState() && m_cameranode)
		{
			const matrix cammat = GetParentMatrix(m_cameranode);
			const vec3 cpos = (cammat * vec4(0,0,0,1)).xyz();
			const vec3 ctar = (cammat * vec4(0,0,-1,1)).xyz();
			const vec3 cup  = (cammat * vec4(0,1,0,0)).xyz();
			view = LookAt(cpos, ctar, cup);
            if (m_cameranode->GetType() == NODETYPE_TRANSFORM)
            {
                const Transform* tr = static_cast<const Transform*>(m_cameranode);
                s32 n = tr->GetChildCount();
                for (s32 i = 0; i < n; ++i){
                    const Node* nd = tr->GetChild(i);
                    if (nd->GetType() == NODETYPE_CAMERA){
                        const Camera* camnode = static_cast<const Camera*>(nd);
                        const f64 asp = static_cast<f64>(m_height) / static_cast<f64>(m_width);
                        fov = camnode->GetFov() * asp;
                        break;
                    }
                }
            }
		}
        matrix proj = PerspectiveFov(fov, m_width/static_cast<f32>(m_height), m_zoom*0.1, 10.0*m_zoom);

		m_srender->SetProjMatrix(proj);
		m_srender->SetViewMatrix(view);
        
        for (int i = 1; i < paramnum; ++i) { // skip clearcolor
            const vec4 vec4val(m_bar[i][0]->GetValue(),m_bar[i][1]->GetValue(),m_bar[i][2]->GetValue(),m_bar[i][3]->GetValue());
            m_srender->SetUniform(vname[i], vec4val);
        }
		// Update,Render
		m_srender->UpdateBuffers(m_root);
		m_srender->Draw(m_root);
		
        g->Disable(VG_DEPTH_TEST);
        g->Disable(VG_CULL_FACE);
        
        m_gui->Draw();
        
        SwapBuffer();
    }
     
	void Idle(void)
	{
        // call draw in Animation mode
        if (m_animcheck->GetState())
            Draw();
        else
            MOE::Sleep(1);
	}
	
	void Resize(int w, int h)
	{
        m_width = w;
        m_height = h;
        g->Viewport(0, 0, w, h);
        m_gui->Resize(w, h);
        m_frame1->SetSize(w, 30);
        m_frame2->SetSize(100, h-30);
		m_timeslider->SetSize(w-220,16);
        m_openbtn->SetPos(5, h - 100);
        m_reloadbtn->SetPos(5, h -  80);
        m_rebufbtn->SetPos(5, h -  60);
        Draw();
	}
    
private:
    MOE::Graphics* g;
    MOE::SceneGraphRender* m_srender;
    MOE::SceneGraph::Node* m_root;
    MOE::Animation* m_anim;
    
    s32 m_width;
    s32 m_height;

    EasyMIDIController* m_midi;
    SimpleGUI::GUIManager* m_gui;
    SimpleGUI::Frame* m_frame1, *m_frame2;
    SimpleGUI::Slider* m_bar[paramnum][4];
	SimpleGUI::Slider* m_timeslider;
    SimpleGUI::Caption* m_timeval;
	SimpleGUI::Check* m_animcheck;
	SimpleGUI::Check* m_cameracheck;
    SimpleGUI::Button* m_openbtn;
    SimpleGUI::Button* m_reloadbtn;
    SimpleGUI::Button* m_rebufbtn;
    
	const MOE::SceneGraph::Node* m_cameranode;
};

#if defined(WIN32) && defined(RELEASEOUT)
int WINAPI WinMain(
	HINSTANCE hInstance, 
	HINSTANCE hPrevInstance, 
	LPSTR lpCmdLine, 
	int nCmdShow
)
#else
int main(int argc, char *argv[])
#endif
{
	printf("KScene3 - System K(c)\n");
	if (argc >= 2)
		g_mrzfile = std::string(argv[1]);
    MOEWindow win(32, 32, 1024, 800);
    CoreWindow::MainLoop();
    return 0;
}
