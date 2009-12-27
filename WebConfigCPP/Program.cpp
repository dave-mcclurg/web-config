// WebConfig - Use a web browser to configure your application
// Copyright (c) 2009 David McClurg <dpm@efn.org>
// Under the MIT License, details: License.txt.

#include "WebConfig/resource.h"

#if 0
using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Diagnostics;
using System.Web;
using System.Text;

namespace SampleApp
{
	static class Program
	{
		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main()
		{
			string path = Application.StartupPath;
			int index = path.IndexOf(@"\bin\");
				if (-1 != index)
				{
					// resource files are in project folder
					path = path.Substring(0, index);
				}
				WebConfig.Manager.Instance.Startup(8080, path);

				Application.EnableVisualStyles();
				Application.Run(new MainForm());
		}
	}
}
#endif

