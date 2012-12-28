//
//  main.cpp
//  MOE3
//
//  Created by kioku on 2012/12/28.
//  Copyright (c) 2012 System K. All rights reserved.
//

#include "../Core/CoreWindow.h"

// GL Headers
#if defined(__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

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
        glClearColor(0.5,0.5,0.5,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // own graphics code.
        
        SwapBuffer();
    }
     
	void Idle(void)
	{
        Draw();
	}
	
	void Resize(int w, int h)
	{
        glViewport(0, 0, w, h);
        Draw();
	}
    
private:
    int m_mode;
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
