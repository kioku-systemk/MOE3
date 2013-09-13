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

namespace {
	std::string g_mrzfile;
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
    	m_frame1->AddChild(m_timeslider);
		m_frame1->AddChild(m_animcheck);
        
		SimpleGUI::Caption* clcl = mnew SimpleGUI::Caption(m_gui, 10, 0, "ClearColor", 16);
        m_frame2->AddChild(clcl);

        m_bar[0] = mnew SimpleGUI::Slider(m_gui, 10,20,80,16);
        m_bar[1] = mnew SimpleGUI::Slider(m_gui, 10,40,80,16);
        m_bar[2] = mnew SimpleGUI::Slider(m_gui, 10,60,80,16);
		m_frame2->AddChild(m_bar[0]);
        m_frame2->AddChild(m_bar[1]);
        m_frame2->AddChild(m_bar[2]);

		m_cameracheck = mnew SimpleGUI::Check(m_gui,"Camera View",10,100);
    	m_frame2->AddChild(m_cameracheck);
		
		m_cameranode = 0;
        m_root = 0;
		m_anim = 0;
        m_srender = new MOE::SceneGraphRender(g);

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
			

		ReloadModels();
 
        m_rot = m_view = MOE::Math::Identity();
        m_zoom = 5.0f;
		m_trans = MOE::Math::vec3(0,0,0);
        MOE::Math::vec3 bmax, bmin;
		MOE::SceneGraph::GetBBox(m_root, bmax, bmin);
        m_trans = (bmax + bmin) * .5;
		m_zoom = MOE::Math::length(bmax - bmin);

		mx = 0;
        my = 0;
        press = 0;
		m_inited = true;
	}
	~MOEWindow()
    {
    }
	
	void ReloadModels()
	{
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
    }
	void MouseLeftUp(int x, int y)
    {
        press = (press & (~1));
        m_gui->MouseUp(0, x,y);
    }
    void MouseRightDown(int x, int y)
    {
        mx = x;
        my = y;
        press |= 2;
    }
    void MouseRightUp(int x, int y)
    {
        press = (press & (~2));
    }
    void MouseMiddleDown(int x, int y)
    {
        mx = x;
        my = y;
        press |= 4;
    }
    void MouseMiddleUp(int x, int y)
    {
        press = (press & (~4));
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
        m_gui->KeyUp(key);
    }
	
    void updateGUI()
    {
        static f32 m0 = m_midi->GetControlParam(0);
        if (m0 != m_midi->GetControlParam(0)){
            m0 = m_midi->GetControlParam(0);
            m_bar[0]->SetValue(m0);
        }
        static f32 m1 = m_midi->GetControlParam(1);
        if (m1 != m_midi->GetControlParam(1)){
            m1 = m_midi->GetControlParam(1);
            m_bar[1]->SetValue(m1);
        }
        static f32 m2 = m_midi->GetControlParam(2);
        if (m2 != m_midi->GetControlParam(2)){
            m2 = m_midi->GetControlParam(2);
            m_bar[2]->SetValue(m2);
        }
	}
    
    void Draw()
    {
        updateGUI();

        const float mr = m_bar[0]->GetValue();
        const float mg = m_bar[1]->GetValue();
        const float mb = m_bar[2]->GetValue();
        g->Enable(VG_DEPTH_TEST);
        g->ClearColor(mr,mg,mb,0);
        g->Clear(VG_COLOR_BUFFER_BIT | VG_DEPTH_BUFFER_BIT);

		// Animation
		if (m_anim) {
			double maxanimtime = m_anim->GetMaxAnimTime();
			if (m_animcheck->GetState()) {
				f64 tm = fmod(MOE::GetTimeCount(),maxanimtime);
				m_timeslider->SetValue(tm/maxanimtime);
			}
			m_anim->Animate(m_root, m_timeslider->GetValue()*maxanimtime);
		}

		// View
        using namespace MOE::Math;
        matrix proj = PerspectiveFov(60, m_width/static_cast<f32>(m_height), m_zoom*0.1, 10.0*m_zoom);
		matrix view = LookAt(vec3(m_trans.x,m_trans.y,m_zoom), vec3(m_trans.x,m_trans.y,0), vec3(0,1,0));
		view = view * m_view;
		if (m_cameracheck->GetState() && m_cameranode)
		{
			const matrix cammat = GetParentMatrix(m_cameranode);
			const vec3 cpos = (cammat * vec4(0,0,0,1)).xyz();
			const vec3 ctar = (cammat * vec4(0,0,-1,1)).xyz();
			const vec3 cup  = (cammat * vec4(0,1,0,0)).xyz();
			view = LookAt(cpos, ctar, cup);
		}
		m_srender->SetProjMatrix(proj);
		m_srender->SetViewMatrix(view);
        
		// Update,Render
		m_srender->UpdateBuffers(m_root);
		m_srender->Draw(m_root);
		
        g->Disable(VG_DEPTH_TEST);
        
        m_gui->Draw();
        // own graphics code.
        
        SwapBuffer();
    }
     
	void Idle(void)
	{
        Draw();
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
    SimpleGUI::Slider* m_bar[3];
	SimpleGUI::Slider* m_timeslider;
	SimpleGUI::Check* m_animcheck;
	SimpleGUI::Check* m_cameracheck;
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
	if (argc < 2)
	{
		printf("kscene3.exe [mrz file]\n\n");
		return 0;
	}
	g_mrzfile = std::string(argv[1]);
    MOEWindow win(32, 32, 1024, 800);
    CoreWindow::MainLoop();
    return 0;
}
