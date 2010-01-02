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

#include <string>

using namespace std;

#define ESC 0x1b

namespace SampleApp
{
	//void CaptureScreen()
	//{
	//    string path = WebConfig.Manager.Instance.GetFolder() + @"\Screen.JPG";
	//    WebConfig.ScreenCapture sc = new WebConfig.ScreenCapture();
	//    sc.CaptureWindowToFile(this.Handle, path, ImageFormat.Jpeg);
	//}

	//Simulation.Instance.Init();

	//string name = "david";
	//bool showStats = false;
	//int choice = 2;
	//int fun = 50;

	//new WebConfig.InputText("debug/name", () => name, (val) => name = val);
	//new WebConfig.InputBool("debug/show stats", () => showStats, (val) => showStats = val);
	//new WebConfig.InputSelect("debug/choose", () => choice, (val) => choice = val).SetOptions("now", "later", "never");
	//new WebConfig.InputSliderInt("debug/fun", () => fun, (val) => { fun = val; });

	//new WebConfig.InputButton("debug/Capture Screen", (val) => CaptureScreen());
	//new WebConfig.InputLink("debug/View Screen", "Screen.JPG");

	//WebConfig.Manager.Instance.Update();
	//           Simulation.Instance.Update(pictureBox1.Width, pictureBox1.Height);
	//           Simulation.Instance.Draw(e.Graphics);

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

	//void CaptureScreen()
	//{
	//	string path = WebConfig::Manager::Instance().GetFolder() + "\\Screen.JPG";
	//	CaptureWindowToFile(this.Handle, path, ImageFormat.Jpeg);
	//}

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

		Simulation::Instance().Init();

		string name = "david";
		bool showStats = false;
		int choice = 2;
		int fun = 50;

		vector<string> choiceList;
		choiceList.push_back("now");
		choiceList.push_back("later");
		choiceList.push_back("never");

		new WebConfig::InputText("debug/name", name);
		new WebConfig::InputBool("debug/show stats", showStats);
		(new WebConfig::InputSelect("debug/choose", choice))->SetOptions(&choiceList);
		new WebConfig::InputSliderInt("debug/fun", fun);

		//new WebConfig::InputButton("debug/Capture Screen", CaptureScreen);
		//new WebConfig::InputLink("debug/View Screen", "Screen.JPG");

		//Main Loop
		while (!WinBGI::shouldexit())
		{
			WebConfig::Manager::Instance().Update();
			WinBGI::delay( 1 );

			WinBGI::clearviewport();
			Simulation::Instance().Update();
			Simulation::Instance().Draw();

			// Check to see if a key has been pressed
			if (WinBGI::kbhit())
			{
				char KeyPressed = WinBGI::getch();
				if (KeyPressed == '\x1b') {
					WinBGI::closegraph();
				}
			}
		}

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
