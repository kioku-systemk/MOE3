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
        
        m_demo = 0;
        // load lua
        if (g_demoluafile != "") {
            m_demo = new MOE::Demo(g);
            b8 r = m_demo->Load(g_demoluafile.c_str());
            if (!r)
                MOELogE("Load error: demo.lua");
        }
        
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

		m_cameracheck = mnew SimpleGUI::Check(m_gui,"Camera View",5,100);
    	m_frame2->AddChild(m_cameracheck);

        m_openbtn = mnew SimpleGUI::Button(m_gui,"OpenDemo",5,height - 100, 90, 16);
        m_openbtn->SetClickedFunc(MOEWindow::openBtnFunc, this);
    	m_frame2->AddChild(m_openbtn);
        m_reloadbtn = mnew SimpleGUI::Button(m_gui,"ReloadModel",5,height - 80, 90, 16);
        m_reloadbtn->SetClickedFunc(MOEWindow::reloadModelBtnFunc, this);
    	m_frame2->AddChild(m_reloadbtn);
        m_rebufbtn = mnew SimpleGUI::Button(m_gui,"ReloadShader",5,height - 60, 90, 16);
        m_rebufbtn->SetClickedFunc(MOEWindow::reloadShaderFunc, this);
    	m_frame2->AddChild(m_rebufbtn);

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
	}
	~MOEWindow()
    {
    }
	
	void ReloadModels()
	{
	}
    void ReloadBuffers()
	{
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
        }
    }
    static void openBtnFunc(void* thisptr){ static_cast<MOEWindow*>(thisptr)->OpenDemo(); }
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
        if (key == 'b' || key == 'B')
			ReloadBuffers();
        if (key == 'o' || key == 'O')
            OpenDemo();
        
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
        
        g->Enable(VG_CULL_FACE);
        g->Enable(VG_DEPTH_TEST);
        g->ClearColor(mr,mg,mb,0);
        g->Clear(VG_COLOR_BUFFER_BIT | VG_DEPTH_BUFFER_BIT);

		// Animation
        double maxanimtime = 10.0;
        if (m_animcheck->GetState()) {
            f64 tm = fmod(MOE::GetTimeCount(),maxanimtime);
            m_timeslider->SetValue(tm/maxanimtime);
        }
        const f32 animtime = m_timeslider->GetValue()*maxanimtime;
    
        if (m_demo)
        {
            //m_demo->SetMatrix("proj",proj);
            //m_demo->SetMatrix("view",view);
            // Update,Render
            m_demo->Update(animtime);
            m_demo->Render(animtime);
        }
        g->Disable(VG_DEPTH_TEST);
        g->Disable(VG_CULL_FACE);
        
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
        m_openbtn->SetPos(5, h - 100);
        m_reloadbtn->SetPos(5, h -  80);
        m_rebufbtn->SetPos(5, h -  60);
        Draw();
	}
    
private:
    MOE::Graphics* g;
    MOE::Demo* m_demo;
    
    s32 m_width;
    s32 m_height;

    EasyMIDIController* m_midi;
    SimpleGUI::GUIManager* m_gui;
    SimpleGUI::Frame* m_frame1, *m_frame2;
    SimpleGUI::Slider* m_bar[3];
	SimpleGUI::Slider* m_timeslider;
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
		g_demoluafile = std::string(argv[1]);
    MOEWindow win(32, 32, 1024, 800);
    CoreWindow::MainLoop();
    return 0;
}
