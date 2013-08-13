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
#if 0	// for script
#include "Utility/ScriptManager.h"
#endif
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
        m_width  = width;
        m_height = height;
        
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
        
        m_root = 0;
#if 0
        m_srender = mnew MOE::SceneGraphRender(g);

        MOE::Stream mst("/Users/kioku/Desktop/scatx3.MRZ", MOE::Stream::MODE_INPUT_BINARY_ONMEMORY);
        //MOE::Stream mst("scatx3.MRZ", MOE::Stream::MODE_INPUT_BINARY_ONMEMORY);
        MOE::MrzLoader loader;
        MOE::SceneGraph::Node* node = loader.Load(&mst);
        if (node) {
            m_root = node;
        }
        
        MOE::Stream ast("/Users/kioku/Desktop/scatx3_mrz.anim", MOE::Stream::MODE_INPUT_BINARY_ONMEMORY);
        //MOE::Stream ast("scatx3_mrz.anim", MOE::Stream::MODE_INPUT_BINARY_ONMEMORY);
        MOE::AnimLoader aloader;
        MOE::Animation* anim = aloader.Load(&ast);
        m_anim = anim;
  
        //MOE::Stream mst("/Users/kioku/Desktop/git/MOE3/src/boxtest.MRZ", MOE::Stream::MODE_INPUT_BINARY_ONMEMORY);
/*        MOE::Stream mst("/Users/kioku/Desktop/scatb.MRZ", MOE::Stream::MODE_INPUT_BINARY_ONMEMORY);
        MOE::MrzLoader loader;
        MOE::SceneGraph::Node* node = loader.Load(&mst);
        MOE::Stream ast("/Users/kioku/Desktop/scatb_a.anim", MOE::Stream::MODE_INPUT_BINARY_ONMEMORY);
        MOE::AnimLoader aloader;
        MOE::Animation* anim = aloader.Load(&ast);
        anim = anim;
        anim->Animate(node,1);*/
#endif
#endif
#if 0	// for script
	// インスタンス取得
	MOE::CScriptManager *pScr = MOE::CScriptManager::GetInstance();

	// lua 読込
	if( pScr->Read( "../data/definition/test.lua", "testeffect" ) ){
		// lua スタック拡張( test として 100 くらい増やした)
		pScr->ExtraStack( "testeffect", 100 );

		// test 関数実行 : f32 & s32 型
		pScr->ExecFunc( "testeffect", "test" );
		pScr->DebugStackPrint( "testeffect" );
		f32 valF32 = pScr->GetExecFuncResultF32( "testeffect" );
		s32 valS32 = pScr->GetExecFuncResultS32( "testeffect" );
		pScr->ClearStack( "testeffect" );

		// test2 関数実行 : string型
		pScr->ExecFunc( "testeffect", "test2" );
		pScr->DebugStackPrint( "testeffect" );
		string valStr = pScr->GetExecFuncResultString( "testeffect" );
		pScr->ClearStack( "testeffect" );

		// test3 関数実行 : string型
		pScr->ExecFunc( "testeffect", "test3" );
		pScr->DebugStackPrint( "testeffect" );
		b8 valBool = pScr->GetExecFuncResultBool( "testeffect" );
		pScr->ClearStack( "testeffect" );

		// test4 関数実行 : c言語側システムコール呼出
		pScr->ExecFunc( "testeffect", "test4" );
		pScr->DebugStackPrint( "testeffect" );

		// test5 関数実行 : 引数を c言語側から渡す場合の呼出
		pScr->ExecFunc( "testeffect", "test5", 1, "%s%f%d%c", "test", 9.5f, 5, 'a' );
		pScr->DebugStackPrint( "testeffect" );

		// globalvalue
		valF32 = pScr->GetGlobalValueF32( "testeffect", "gFloat" );
		valS32 = pScr->GetGlobalValueS32( "testeffect", "gInt" );
		valBool = pScr->GetGlobalValueBool( "testeffect", "gBool" );
		valStr = pScr->GetGlobalValueString( "testeffect", "gString" );

		pScr->DebugStackPrint( "testeffect" );

		// table
		valF32 = pScr->GetTableValueF32( "testeffect", "gTable", "t2" );
		valS32 = pScr->GetTableValueS32( "testeffect", "gTable", "t1" );
		valStr = pScr->GetTableValueString( "testeffect", "gTable", "t0" );
		valBool = pScr->GetTableValueBool( "testeffect", "gTable", "t3" );

		pScr->DebugStackPrint( "testeffect" );

		// 後始末
		pScr->Release();
	}
#endif
        rot = MOE::Math::Identity();
        mx = 0;
        my = 0;
        press = false;
	}
	~MOEWindow()
    {
    }
	
    s32 mx;
    s32 my;
    b8  press;
    MOE::Math::matrix rot;
	void MouseLeftDown(int x, int y)
    {
        mx = x;
        my = y;
        press = true;
        m_gui->MouseDown(0, x,y);
    }
	void MouseLeftUp(int x, int y)
    {
        press = false;
        m_gui->MouseUp(0, x,y);
    }
	void MouseMove(int x, int y)
    {
        s32 dx = x - mx;
        s32 dy = y - my;
        if (press) {
            rot = MOE::Math::RotationY(dx * 0.2) * rot;
            rot = MOE::Math::RotationX(dy * 0.2) * rot;
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
#if 0
        using namespace MOE::Math;
        matrix proj = PerspectiveFov(45, m_width/static_cast<f32>(m_height), 0.1, 100.0);
        matrix view = LookAt(vec3(0,0,50), vec3(0,0,0), vec3(0,1,0));
        view = view * rot;
        m_srender->SetProjMatrix(proj);
        m_srender->SetViewMatrix(view);
        
        f64 tm = fmod(MOE::GetTimeCount(),5.0);
        if (m_anim)
            m_anim->Animate(m_root, tm);
        m_srender->UpdateBuffers(m_root);
        m_srender->Draw(m_root);
#endif
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
        Draw();
	}
    
private:
    MOE::Graphics* g;
    MOE::SceneGraphRender* m_srender;
    MOE::SceneGraph::Node* m_root;
    MOE::Animation* m_anim;
    
    s32 m_width;
    s32 m_height;
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
