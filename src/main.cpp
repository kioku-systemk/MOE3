//
//  main.cpp
//  MOE3
//
//  Created by kioku on 2012/12/28.
//  Copyright (c) 2012 System K. All rights reserved.
//

#include "../Core/MOE.h"
#include "../Core/CoreWindow.h"
#include "../Core/Graphics.h"
#include "../EasyMIDIController/EasyMIDIController.h"

#include <stdio.h>
#include <stdlib.h>

class MOEWindow : public CoreWindow
{
public:
	MOEWindow(int x, int y, int width, int height)
	: CoreWindow(x, y, width, height, "MOE3"
#if defined(RELEASEOUT)
	,true // fullscreen
#endif
	){
        g = mnew MOE::Graphics();
#if !defined(RELEASEOUT)
        midi = mnew EasyMIDIController();
#endif
	}
	~MOEWindow()
    {
    }
	
	void MouseLeftDown(int x, int y) {}
	void MouseLeftUp(int x, int y)   {}
	void MouseMove(int x, int y)     {}
	void Wheel(float dx, float dy, float dz) {}
	void KeyDown(int key){
		if (key == 27)
			exit(0);
	}
	void KeyUp(int key) {}
	
    void Draw()
    {
        const float mr = midi->GetControlParam(0);
        const float mg = midi->GetControlParam(1);
        const float mb = midi->GetControlParam(2);
        g->ClearColor(mr,mg,mb,0);
        g->Clear(VG_COLOR_BUFFER_BIT | VG_DEPTH_BUFFER_BIT);
        
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
        Draw();
	}
    
private:
    MOE::Graphics* g;
#ifndef RELEASEOUT
    EasyMIDIController* midi;
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
