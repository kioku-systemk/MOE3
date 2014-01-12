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

#ifndef _WIN32
#include <unistd.h>
#endif

namespace {
	std::string g_demoluafile;
    s32 ww = 256;
    s32 wh = 200;
}

class MOEWindow : public CoreWindow
{
public:
	MOEWindow(int x, int y, int width, int height)
	: CoreWindow(x, y, width, height, "MOE3 demoplayer"
	){
        g = mnew MOE::Graphics();

        m_gui  = mnew SimpleGUI::GUIManager();
        m_gui->Resize(width, height);
        skGUI::BaseWindow* win = m_gui->GetRoot();
        m_width  = width;
        m_height = height;
        
        m_guivisible = true;

        const f32 col[] = {0.50,0.50,0.50,0.50};
        m_frame1 = mnew SimpleGUI::Frame(m_gui,(width-ww)/2,(height-wh)/2,ww, wh, col);
        win->AddChild(m_frame1);

        SimpleGUI::Caption* cap = mnew SimpleGUI::Caption(m_gui, 10, 5, "MOE3 Demo player", 16);
        m_frame1->AddChild(cap);

		m_fullscreencheck = mnew SimpleGUI::Check(m_gui,"Fullscreen mode",5,30);
        m_fullscreencheck->SetState(true);
    	m_frame1->AddChild(m_fullscreencheck);
    
        m_windowcheck = mnew SimpleGUI::Check(m_gui,"Window mode",5,50);
        m_windowcheck->SetState(false);
    	m_frame1->AddChild(m_windowcheck);

        
        m_exitbtn = mnew SimpleGUI::Button(m_gui,"Exit",35,wh - 30, 90, 16);
        m_exitbtn->SetClickedFunc(MOEWindow::exitBtnFunc, this);
    	m_frame1->AddChild(m_exitbtn);
        m_demobtn = mnew SimpleGUI::Button(m_gui,"Demo!!",130,wh - 30, 90, 16);
        m_demobtn->SetClickedFunc(MOEWindow::demoBtnFunc, this);
    	m_frame1->AddChild(m_demobtn);
        
		mx = 0;
        my = 0;
        press = 0;
#if MOE_PLATFORM_WINDOWS
		m_inited = true;
#endif
        
        m_demo = 0;
        
        Draw();
	}
	~MOEWindow()
    {
    }
    
    void LoadDemo()
    {
        if (m_fullscreencheck->GetState())
            GoFullscreen(true);
    
        m_guivisible = false;
        Draw();
        
        std::string exepath = GetExePath();
        chdir((exepath + "/../").c_str());
        MOE::Stream::LoadKDB("demo.kdb");
        const s8* fn = "demo.lua";
        delete m_demo;
        g_demoluafile = std::string(fn);
        m_demo = new MOE::Demo(g);
        m_demo->Load(g_demoluafile.c_str());
        m_demo->Resize(m_width,m_height);
        
        m_demo->Cache();
        m_demo->Play();
    }
    void ExitDemo()
    {
        exit(0);
    }
    
    static void demoBtnFunc(void* thisptr){ static_cast<MOEWindow*>(thisptr)->LoadDemo(); }
    static void exitBtnFunc(void* thisptr){ static_cast<MOEWindow*>(thisptr)->ExitDemo(); }

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
        if (m_guivisible && !m_gui->MouseDown(0, x,y))
			press |= 1;
        Draw();
    }
	void MouseLeftUp(int x, int y)
    {
        press = (press & (~1));
        if (m_guivisible)
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
        static bool fsmode = false;
        if (key == 'f' || key == 'F'){
            fsmode = !fsmode;
            GoFullscreen(fsmode);
        }
		m_gui->KeyUp(key);
        Draw();
    }
	
    void Draw()
    {
        g->Enable(VG_CULL_FACE);
        g->Enable(VG_DEPTH_TEST);
        g->ClearColor(0,0,0,0);
        g->Clear(VG_COLOR_BUFFER_BIT | VG_DEPTH_BUFFER_BIT);

		// Animation
        f64 maxanimtime = 0;
		f64 tm = 0;
		if (m_demo) {
			maxanimtime = m_demo->GetDemoTime();
			tm = m_demo->GetTime();
		}
		const f32 animtime = tm;
        if (maxanimtime != 0.0 && maxanimtime <= tm)
            ExitDemo();
        
        if (m_demo)
        {
            // Update,Render
            m_demo->Update(animtime);
            m_demo->Render(animtime);
            
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
        Draw();
	}
	
	void Resize(int w, int h)
	{
        m_width = w;
        m_height = h;
        g->Viewport(0, 0, w, h);
        m_gui->Resize(w, h);
        
        if (m_demo)
            m_demo->Resize(w,h);
        Draw();
	}
    
private:
    MOE::Graphics* g;
    MOE::Demo* m_demo;
    
    s32 m_width;
    s32 m_height;

    SimpleGUI::GUIManager* m_gui;
    b8 m_guivisible;
    SimpleGUI::Frame* m_frame1;
	SimpleGUI::Check* m_fullscreencheck;
    SimpleGUI::Check* m_windowcheck;
    SimpleGUI::Button* m_demobtn;
    SimpleGUI::Button* m_exitbtn;
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
    MOEWindow win(32, 32, 1280,720);
    CoreWindow::MainLoop();
    return 0;
}
