//
//  main.cpp
//  MOE3
//
//  Created by kioku on 2012/12/28.
//  Copyright (c) 2012 System K. All rights reserved.
//

#include "Core/MOE.h"
#include "Core/CoreWindow.h"
#include "Gfx/Graphics.h"
#include "EasyMIDIController/EasyMIDIController.h"
#include "Utility/Parameters.h"
#include "Utility/Range.h"
#include "Utility/Parser.h"

#include <stdio.h>
#include <stdlib.h>

#include "Core/UI/SimpleGUI.h"

#include "SceneGraph/SceneGraph.h"

class MOEWindow : public CoreWindow
{
public:
	MOEWindow(int x, int y, int width, int height)
	: CoreWindow(x, y, width, height, "MOE3"
#ifdef RELEASEOUT
	,true // fullscreen
#endif
	){
        g = mnew MOE::Graphics();
#ifndef RELEASEOUT
        m_midi = mnew EasyMIDIController();
        m_gui  = mnew SimpleGUI::GUIManager();
        m_gui->Resize(width, height);
        skGUI::BaseWindow* win = m_gui->GetRoot();
        
        
        // TEST
        const f32 col[] = {0.50,0.50,0.50,0.50};
        m_frame1 = mnew SimpleGUI::Frame(m_gui,0,0,width, 30, col);
        m_frame2 = mnew SimpleGUI::Frame(m_gui,0,30,100, height-30, col);
        win->AddChild(m_frame1);
        win->AddChild(m_frame2);

        SimpleGUI::Caption* cap = mnew SimpleGUI::Caption(m_gui, 0, 0, "MOE3 Demo Engine", 16);
        m_frame1->AddChild(cap);
        
        m_bar[0] = mnew SimpleGUI::Slider(m_gui, 10,10,80,16);
        m_bar[1] = mnew SimpleGUI::Slider(m_gui, 10,30,80,16);
        m_bar[2] = mnew SimpleGUI::Slider(m_gui, 10,50,80,16);
        m_frame2->AddChild(m_bar[0]);
        m_frame2->AddChild(m_bar[1]);
        m_frame2->AddChild(m_bar[2]);
#endif
	}
	~MOEWindow()
    {
    }
	
	void MouseLeftDown(int x, int y)
    {
        m_gui->MouseDown(0, x,y);
    }
	void MouseLeftUp(int x, int y)
    {
        m_gui->MouseUp(0, x,y);
    }
	void MouseMove(int x, int y)
    {
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

#if 1
		// @test:スクリプトテスト
		MOE::CParameters mParams;
		MOE::CParser parser;
		parser.Parse("..\\data\\definition\\script.txt", mParams );		

		mParams.UseNamespace("test");
		s32 a = mParams.GetS32("a");
		s32 b = mParams.GetS32("b");
		f32 c = mParams.GetF32("c");
		string tex = mParams.GetString("tex");
		vec3 d = mParams.GetVector3("d");
		vec4 e = mParams.GetColor("e");
		MOE::CRange f = mParams.GetRange("f");
		
		MOELogI( "test:a(%d), b(%d), c(%f), d(%f,%f,%f), tex(%s) e(%f,%f,%f,%f) f(%f,%f)\n", 
			a, b, c, d.x, d.y, d.z, tex.c_str(), e.r, e.g, e.b, e.a, f.GetMinimum(), f.GetMaximum() );
#endif
	}
    
    void Draw()
    {
        updateGUI();
        
        const float mr = m_bar[0]->GetValue();
        const float mg = m_bar[1]->GetValue();
        const float mb = m_bar[2]->GetValue();
        g->ClearColor(mr,mg,mb,0);
        g->Clear(VG_COLOR_BUFFER_BIT | VG_DEPTH_BUFFER_BIT);
    
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
        g->Viewport(0, 0, w, h);
        m_gui->Resize(w, h);
        m_frame1->SetSize(w, 30);
        m_frame2->SetSize(100, h-30);
        Draw();
	}
    
private:
    MOE::Graphics* g;
#ifndef RELEASEOUT
    EasyMIDIController* m_midi;
    SimpleGUI::GUIManager* m_gui;
    SimpleGUI::Frame* m_frame1, *m_frame2;
    SimpleGUI::Slider* m_bar[3];
#endif
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
    MOEWindow win(32, 32, 720, 480);
    CoreWindow::MainLoop();
    return 0;
}
