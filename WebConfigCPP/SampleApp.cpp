// WebConfig - Use a web browser to configure your application
// Copyright (c) 2009 David McClurg <dpm@efn.org>
// Under the MIT License, details: License.txt.

#if 0
using System;
using System.Text;
using System.Drawing;
using System.Drawing.Imaging;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace SampleApp
{
    public class MainForm : System.Windows.Forms.Form
    {
        private System.ComponentModel.IContainer components;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.PictureBox pictureBox1;

        void CaptureScreen()
        {
            string path = WebConfig.Manager.Instance.GetFolder() + @"\Screen.JPG";
            WebConfig.ScreenCapture sc = new WebConfig.ScreenCapture();
            sc.CaptureWindowToFile(this.Handle, path, ImageFormat.Jpeg);
        }

        public MainForm()
        {
            InitializeComponent();

            timer1.Interval = 1;
            timer1.Start();

            this.SetStyle(ControlStyles.OptimizedDoubleBuffer, true);
            this.SetStyle(ControlStyles.AllPaintingInWmPaint, true);
            this.SetStyle(ControlStyles.UserPaint, true);

            Simulation.Instance.Init();

            string name = "david";
            bool showStats = false;
            int choice = 2;
            int fun = 50;

            new WebConfig.InputText("debug/name", () => name, (val) => name = val);
            new WebConfig.InputBool("debug/show stats", () => showStats, (val) => showStats = val);
            new WebConfig.InputSelect("debug/choose", () => choice, (val) => choice = val).SetOptions("now", "later", "never");
            new WebConfig.InputSliderInt("debug/fun", () => fun, (val) => { fun = val; });

            new WebConfig.InputButton("debug/Capture Screen", (val) => CaptureScreen());
            new WebConfig.InputLink("debug/View Screen", "Screen.JPG");
        }

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        protected override void Dispose(bool disposing)
        {
            WebConfig.Manager.Instance.Shutdown();
            if (disposing)
            {
                if (components != null)
                {
                    components.Dispose();
                }
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code
        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.SuspendLayout();
            // 
            // timer1
            // 
            this.timer1.Interval = 500;
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // pictureBox1
            // 
            this.pictureBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.pictureBox1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pictureBox1.Location = new System.Drawing.Point(8, 40);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(378, 322);
            this.pictureBox1.TabIndex = 0;
            this.pictureBox1.TabStop = false;
            this.pictureBox1.Paint += new System.Windows.Forms.PaintEventHandler(this.pictureBox1_Paint);
            // 
            // MainForm
            // 
            this.ClientSize = new System.Drawing.Size(394, 368);
            this.Controls.Add(this.pictureBox1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Name = "MainForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "WebConfig Sample Application";
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.ResumeLayout(false);

        }
        #endregion

        private void timer1_Tick(object sender, System.EventArgs e)
        {
            WebConfig.Manager.Instance.Update();
            Simulation.Instance.Update(pictureBox1.Width, pictureBox1.Height);

            this.Refresh();
        }

        private void pictureBox1_Paint(object sender, System.Windows.Forms.PaintEventArgs e)
        {
            Simulation.Instance.Draw(e.Graphics);
        }
    }
}
#endif

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include "windows.h"
#include "winbgi2.h"

#include "WebConfigManager.h"
#include "WebConfigInput.h"

#include "GuiConsole.h"

#include <string>
using namespace std;

#define ESC 0x1b

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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine, int nCmdShow)
{
#ifdef _DEBUG
	RedirectIOToConsole();
#endif

    char path[_MAX_PATH];
	GetResourcePath(path);

	WebConfig::Manager::Instance().Startup(8080, path);

	//Simulation.Instance.Init();

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

	srand((unsigned int)time(NULL));  // Seed the random number generator
	int GraphDriver=0,GraphMode=0;
	initgraph( &GraphDriver, &GraphMode, "WebConfig Sample Application", 800, 600 ); // Start Window

	//Variable Declarations
	float ballX, ballY;
	float XVel, YVel;

	// Initial Position
	ballX = 100.0;
	ballY = 40.0;

	// Initial Velocity
	XVel = 4.0;
	YVel = 5.0;

	//Main Loop
	while (!shouldexit()) {

		WebConfig::Manager::Instance().Update();
		delay( 5 );

		// Remember previous position
		float prevX, prevY;
		prevX = ballX;
		prevY = ballY;

		// Move the ball (Add Velocity to Position)
		ballX += XVel;
		ballY += YVel;

		// Erase old Ball
		setcolor(BLACK);
        setfillstyle(SOLID_FILL, BLACK);
		fillellipse( (int)prevX, (int)prevY, 20, 20);

		// Draw New ball
		setcolor(RED);
        setfillstyle(SOLID_FILL, RED);
		fillellipse( (int)ballX, (int)ballY, 20, 20);

		// Bounch off left/right walls
		if ( ballX > 770  || ballX < 30) {
			XVel *= -1.0;
		}

		// Bounch off top/bottom walls
		if ( ballY > 570  || ballY < 30) {
			YVel *= -1.0;
		}

		// Check to see if a key has been pressed
		if (kbhit()) {
			char KeyPressed = getch();
			if (KeyPressed == '\x1b') {
				closegraph();
			}
		}
	}

	WebConfig::Manager::Instance().Shutdown();

} //end of main()

