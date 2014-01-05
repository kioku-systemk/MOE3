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
#include "Core/Thread.h"

#include "Core/UI/SimpleGUI.h"
#include "SceneGraph/SceneGraph.h"

#include "Core/Time.h"

#include "Demo/Demo.h"

namespace {
	std::string g_demoluafile;
}

class MOEWindow : public CoreWindow
{
public:
	MOEWindow(int x, int y, int width, int height)
	: CoreWindow(x, y, width, height, "KScene3 Demoview"
	){
        g = mnew MOE::Graphics();

        m_midi = mnew EasyMIDIController();
        m_gui  = mnew SimpleGUI::GUIManager();
        m_gui->Resize(width, height);
        skGUI::BaseWindow* win = m_gui->GetRoot();
        m_width  = width;
        m_height = height;
        
        m_guivisible = true;
        
        m_demo = 0;
        
        // load lua
        if (g_demoluafile != "") {
            m_demo = new MOE::Demo(g);
            b8 r = m_demo->Load(g_demoluafile.c_str());
            if (!r)
                MOELogE("Load error: demo.lua");
            m_demo->Resize(width,height);
        }
        
        // TEST
        const f32 col[] = {0.50,0.50,0.50,0.50};
        m_frame1 = mnew SimpleGUI::Frame(m_gui,0,0,width, 30, col);
        m_frame2 = mnew SimpleGUI::Frame(m_gui,0,30,100, height-30, col);
        win->AddChild(m_frame1);
        win->AddChild(m_frame2);

        SimpleGUI::Caption* cap = mnew SimpleGUI::Caption(m_gui, 10, 5, "KScene3", 16);
        m_frame1->AddChild(cap);

        m_timelabel = mnew SimpleGUI::Caption(m_gui, 220,7, "0.000", 16);
		m_timeslider = mnew SimpleGUI::Slider(m_gui, 200,10,width - 220,16);
        m_timeslider->SetChangedFunc(changeTimeSliderFunc, this);
		m_animcheck = mnew SimpleGUI::Check(m_gui,"DEMO!!",110,5);
        m_animcheck->SetChangedFunc(changeAnimFunc,this);
        m_frame1->AddChild(m_timelabel);
    	m_frame1->AddChild(m_timeslider);
		m_frame1->AddChild(m_animcheck);
        /*
		SimpleGUI::Caption* clcl = mnew SimpleGUI::Caption(m_gui, 10, 0, "ClearColor", 16);
        m_frame2->AddChild(clcl);

        m_bar[0] = mnew SimpleGUI::Slider(m_gui, 10,20,80,16);
        m_bar[1] = mnew SimpleGUI::Slider(m_gui, 10,40,80,16);
        m_bar[2] = mnew SimpleGUI::Slider(m_gui, 10,60,80,16);
        SimpleGUI::Caption* label0 = mnew SimpleGUI::Caption(m_gui, 12,17, "0.000", 16);
        SimpleGUI::Caption* label1 = mnew SimpleGUI::Caption(m_gui, 12,37, "0.000", 16);
        SimpleGUI::Caption* label2 = mnew SimpleGUI::Caption(m_gui, 12,57, "0.000", 16);
        m_bar[0]->SetUserData(label0); m_bar[0]->SetChangedFunc(changeSliderFunc, m_bar[0]);
        m_bar[1]->SetUserData(label1); m_bar[1]->SetChangedFunc(changeSliderFunc, m_bar[1]);
        m_bar[2]->SetUserData(label2); m_bar[2]->SetChangedFunc(changeSliderFunc, m_bar[2]);
        m_frame2->AddChild(label0);
        m_frame2->AddChild(label1);
        m_frame2->AddChild(label2);
		m_frame2->AddChild(m_bar[0]);
        m_frame2->AddChild(m_bar[1]);
        m_frame2->AddChild(m_bar[2]);
         */

		m_idlecheck = mnew SimpleGUI::Check(m_gui,"Idle Draw",5,0);
        //m_idlecheck->SetState(true);
    	m_frame2->AddChild(m_idlecheck);
    
        m_overridecheck = mnew SimpleGUI::Check(m_gui,"Override",5,20);
        m_overridecheck->SetState(false);
    	m_frame2->AddChild(m_overridecheck);

        for (int i = 0; i < 8; ++i) {
            m_pbar[i] = mnew SimpleGUI::Slider(m_gui, 10, 40 + 20*i,80,16);
        
            SimpleGUI::Caption* label = mnew SimpleGUI::Caption(m_gui, 12, 40+20*i-3, "0.000", 16);
            m_pbar[i]->SetUserData(label);
            m_pbar[i]->SetChangedFunc(changeSliderFunc, m_pbar[i]);
            m_frame2->AddChild(label);
            m_frame2->AddChild(m_pbar[i]);
        }
        
        m_openbtn = mnew SimpleGUI::Button(m_gui,"OpenDemo",5,height - 140, 90, 16);
        m_openbtn->SetClickedFunc(MOEWindow::openBtnFunc, this);
    	m_frame2->AddChild(m_openbtn);
        m_reloadbtn = mnew SimpleGUI::Button(m_gui,"ReloadDemo",5,height - 120, 90, 16);
        m_reloadbtn->SetClickedFunc(MOEWindow::reloadDemoBtnFunc, this);
    	m_frame2->AddChild(m_reloadbtn);
        m_rebufbtn = mnew SimpleGUI::Button(m_gui,"ReloadShader",5,height - 100, 90, 16);
        m_rebufbtn->SetClickedFunc(MOEWindow::reloadShaderFunc, this);
    	m_frame2->AddChild(m_rebufbtn);

        m_exportbtn = mnew SimpleGUI::Button(m_gui,"ExportDemo",5,height - 60, 90, 16);
        m_exportbtn->SetClickedFunc(MOEWindow::exportBtnFunc, this);
    	m_frame2->AddChild(m_exportbtn);
        
		m_cameranode = 0;
 
        m_rot = m_view = MOE::Math::Identity();
        m_zoom = 5.0f;
		m_trans = MOE::Math::vec3(0,0,0);
        
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
	
	void ReloadDemo()
	{
        if (g_demoluafile != "")
        {
            //delete m_demo;
            //m_demo = new MOE::Demo(g);
            //m_demo->Resize(m_width,m_height);
            float demorate = m_timeslider->GetValue();
            b8 r = m_demo->Load(g_demoluafile.c_str());
            if (!r)
                MOELogE("Faild demo load.");
            m_demo->Resize(m_width,m_height);
            m_demo->SetTime(m_demo->GetDemoTime()*demorate);
        }
	}
    void ReloadBuffers()
	{
        if (m_demo)
            m_demo->ReloadEffectShaders();
	}
    
    void OpenDemo()
    {
        const char* fn = FileOpenDialog("lua");
        if (fn)
        {
            delete m_demo;
            g_demoluafile = std::string(fn);
            m_demo = new MOE::Demo(g);
            m_demo->Load(g_demoluafile.c_str());
            m_demo->Resize(m_width,m_height);
        }
    }
    
    void ExportDemo()
    {
        const char* fn = FileSaveDialog("dpk");
        if (m_demo)
            m_demo->Export(fn);
    }

    static void exportBtnFunc(void* thisptr){ static_cast<MOEWindow*>(thisptr)->ExportDemo(); }
    static void openBtnFunc(void* thisptr){ static_cast<MOEWindow*>(thisptr)->OpenDemo(); }
    static void reloadDemoBtnFunc(void* thisptr){ static_cast<MOEWindow*>(thisptr)->ReloadDemo(); }
    static void reloadShaderFunc(void* thisptr){ static_cast<MOEWindow*>(thisptr)->ReloadBuffers(); }
    static void changeSliderFunc(float v, void* thisptr) {
        SimpleGUI::Caption* cap = static_cast<SimpleGUI::Caption*>(static_cast<SimpleGUI::Slider*>(thisptr)->GetUserData());
        char buf[64];
        sprintf(buf, "%4.3lf", static_cast<double>(v));
        cap->SetText(buf);
    }
    static void changeTimeSliderFunc(float v, void* thisptr) {
		if (thisptr)
			static_cast<MOEWindow*>(thisptr)->changeTimeSlider(v);
    }
    void changeTimeSlider(float v)
    {
        if (m_demo && !m_demo->IsPlaying()){
            f64 tm = v * m_demo->GetDemoTime();
            m_demo->SetTime(tm);
            char buf[64];
            sprintf(buf, "%4.3lf", tm);
            m_timelabel->SetText(buf);
        }
    }
    static void changeAnimFunc(bool r, void* thisptr) { static_cast<MOEWindow*>(thisptr)->ChangeAnimMode(r); }
    void ChangeAnimMode(bool r)
    {
        if (r)
            m_demo->Play();
        else
            m_demo->Stop();
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
        if (key == ' ')
            m_animcheck->SetState(!m_animcheck->GetState());
        
        m_gui->KeyDown(key);
	}
	void KeyUp(int key)
    {
		if (key == 'r' || key == 'R')
			ReloadDemo();
        if (key == 's' || key == 'S')
			ReloadBuffers();
        if (key == 'o' || key == 'O')
            OpenDemo();
        if (key == 'v' || key == 'V')
            VisibleUI();
        
		m_gui->KeyUp(key);
        Draw();
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
        
        static float m4 = m_midi->GetControlParam(4);
        if (m4 != m_midi->GetControlParam(4)){
            m4 = m_midi->GetControlParam(4);
            m_pbar[0]->SetValue(m4);
        }
        static float m5 = m_midi->GetControlParam(5);
        if (m5 != m_midi->GetControlParam(5)){
            m5 = m_midi->GetControlParam(5);
            m_pbar[1]->SetValue(m5);
        }
        static float m6 = m_midi->GetControlParam(6);
        if (m6 != m_midi->GetControlParam(6)){
            m6 = m_midi->GetControlParam(6);
            m_pbar[2]->SetValue(m6);
        }
        static float m7 = m_midi->GetControlParam(7);
        if (m7 != m_midi->GetControlParam(7)){
            m7 = m_midi->GetControlParam(7);
            m_pbar[3]->SetValue(m7);
        }
	}
    
    void Draw()
    {
        updateGUI();
        const float mr = 0;//m_bar[0]->GetValue();
        const float mg = 0;//m_bar[1]->GetValue();
        const float mb = 0;//m_bar[2]->GetValue();
 
        g->Enable(VG_CULL_FACE);
        g->Enable(VG_DEPTH_TEST);
        g->ClearColor(mr,mg,mb,0);
        g->Clear(VG_COLOR_BUFFER_BIT | VG_DEPTH_BUFFER_BIT);

		// Animation
        f64 maxanimtime = 0;
		f64 tm = 0;
		if (m_demo) {
			maxanimtime = m_demo->GetDemoTime();
			tm = m_demo->GetTime();

			if (m_timeslider)
				m_timeslider->SetValue(tm / maxanimtime);
		}
		const f32 animtime = tm;

        if (m_demo)
        {
            // Update,Render
            m_demo->Update(animtime);
            m_demo->Render(animtime);
            
            if (m_overridecheck->GetState()) {
                m_demo->SetVec4("ep1",MOE::Math::vec4(m_pbar[0]->GetValue(),
                                                      m_pbar[1]->GetValue(),
                                                      m_pbar[2]->GetValue(),
                                                      m_pbar[3]->GetValue()));
                m_demo->SetVec4("ep2",MOE::Math::vec4(m_pbar[4]->GetValue(),
                                                      m_pbar[5]->GetValue(),
                                                      m_pbar[6]->GetValue(),
                                                      m_pbar[7]->GetValue()));
            } else {
                m_demo->ClearUniforms();
            }
        }
        g->Disable(VG_DEPTH_TEST);
        g->Disable(VG_CULL_FACE);
        
        if (m_guivisible)
            m_gui->Draw();
        // own graphics code.
        
        SwapBuffer();
    }
    
    void VisibleUI()
    {
        m_guivisible = !m_guivisible;
    }
	void Idle(void)
	{
        // call draw in Animation mode
        if (m_animcheck->GetState() || m_idlecheck->GetState())
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
        m_openbtn->SetPos(5, h - 140);
        m_reloadbtn->SetPos(5, h - 120);
        m_rebufbtn->SetPos(5, h -  100);
        
        m_exportbtn->SetPos(5, h - 60);
        if (m_demo)
            m_demo->Resize(w,h);
        Draw();
	}
    
private:
    MOE::Graphics* g;
    MOE::Demo* m_demo;
    
    s32 m_width;
    s32 m_height;

    EasyMIDIController* m_midi;
    SimpleGUI::GUIManager* m_gui;
    b8 m_guivisible;
    SimpleGUI::Frame* m_frame1, *m_frame2;
    SimpleGUI::Slider* m_bar[3];
    SimpleGUI::Slider* m_pbar[8];
	SimpleGUI::Slider* m_timeslider;
    SimpleGUI::Caption* m_timelabel;
	SimpleGUI::Check* m_animcheck;
	SimpleGUI::Check* m_idlecheck;
    SimpleGUI::Check* m_overridecheck;
    SimpleGUI::Button* m_exportbtn;
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
		g_demoluafile = std::string(argv[1]);
    MOEWindow win(32, 32, 1024, 800);
    CoreWindow::MainLoop();
    return 0;
}
