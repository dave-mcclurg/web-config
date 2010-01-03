// WebConfig - Use a web browser to configure your application
// Copyright (c) 2009 David McClurg <dpm@efn.org>
// Under the MIT License, details: License.txt.

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include "windows.h"
#include "winbgi2.h"

#include "Simulation.h"
#include "WebConfigManager.h"
#include "WebConfigInput.h"

#include "GuiConsole.h"
#include "CaptureScreen.h"

using namespace std;

#define ESC 0x1b

namespace SampleApp
{
	static void GetResourcePath(char* path)
	{
		GetModuleFileName(NULL, path, _MAX_PATH);
		int length = strlen(path);
		if (length > 0)
		{
			int i = length;
			while ((i > 0) && (path[i] != '\\'))
			{
				i--;
			}
			length = i;
			path[length] = 0;
		}
		if (length > 4 && stricmp(&path[length-4], "\\bin") == 0)
		{
			length -= 4;
			path[length] = 0;
		}
	}

	void CaptureScreen()
	{
		string path = WebConfig::Manager::Instance().GetFolder() + "\\Screen.BMP";
		CaptureScreen(WinBGI::gethandle(), path);
	}

	void Run()
	{
		char path[_MAX_PATH];
		GetResourcePath(path);

		WebConfig::Manager::Instance().Startup(8080, path);

		srand((unsigned int)time(NULL));  // Seed the random number generator

		// Initialize the graphics interface
		int GraphDriver=0,GraphMode=0;
		WinBGI::initgraph( &GraphDriver, &GraphMode, "WebConfig Sample Application", 640, 480 ); // Start Window
		WinBGI::setviewport(0,0,640,480,0);

		// make sleep() and timeGetTime() return accurate timings
		timeBeginPeriod(1);

		Simulation::Instance().Init();

		string name = "david";
		bool showStats = false;
		int choice = 2;
		int fun = 50;

		new WebConfig::InputText("debug/name", name);
		new WebConfig::InputBool("debug/show stats", showStats);
		WebConfig::InputSelect* chooser = new WebConfig::InputSelect("debug/choose", choice);
		chooser->AddOption("now");
		chooser->AddOption("later");
		chooser->AddOption("never");
		new WebConfig::InputSliderInt("debug/fun", fun);

		new WebConfig::InputButton("debug/Capture Screen", CaptureScreen);
		new WebConfig::InputLink("debug/View Screen", "Screen.BMP");

		//Main Loop
		while (!WinBGI::quitgraph())
		{
			long timeStart = timeGetTime();

			// Check to see if a key has been pressed
			WinBGI::handle_input();
			if (WinBGI::kbhit())
			{
				char KeyPressed = WinBGI::getch();
				if (KeyPressed == '\x1b') {
					WinBGI::closegraph();
					break;
				}
			}

			WebConfig::Manager::Instance().Update();
			Simulation::Instance().Update();

			WinBGI::clearviewport();

			Simulation::Instance().Draw();

#define FPS 60

			long timeEnd = timeGetTime();
			long deltaTime = timeEnd - timeStart;
			if (deltaTime < (1000/FPS))
			{
				// cap frame frate
				Sleep((1000/FPS) - deltaTime);
			}
			else	// too slow; but yield to OS
			{
				Sleep(1);
			}
		}

		timeEndPeriod(1);

		WebConfig::Manager::Instance().Shutdown();

	} //end of main()

}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		LPSTR lpCmdLine, int nCmdShow)
{
#ifdef _DEBUG
	RedirectIOToConsole();
#endif

	SampleApp::Run();
}
