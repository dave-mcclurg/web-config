// WebConfig - Use a web browser to configure your application
// Copyright (c) 2009 David McClurg <dpm@efn.org>
// Under the MIT License, details: License.txt.

#include "HTTPServer.h"
#include "HTTPUtility.h"
#include "HTMLBuilder.h"
#include "WebConfigInput.h"
#include "WebConfigManager.h"
#include "Convert.h"
#include "StringHelper.h"
#include "Path.h"
#include "IniFile.h"

#include <map>
#include <vector>
#include <set>

#include <assert.h>
#include <stdio.h>

using namespace std;

namespace WebConfig
{
    /// <summary>
    /// This class is an http server and manages the list of inputs
    /// </summary>
    class ManagerImpl
    {
	public:

        /// <summary>
        /// Saved value for forms with AutoSave
        /// </summary>
        class SavedValue
        {
		public:
			string UniqueID;
            string Value;
            SavedValue(string id, string v) { UniqueID = id; Value = v; }
        };

        /// <summary>
        /// http server
        /// </summary>
        HTTPServer* theServer;

        /// <summary>
        /// Dictionary of inputs keyed by UniqueID
        /// </summary>
        map<string, InputBase*> inputs;

        /// <summary>
        /// Dictionary of forms keyed by Name
        /// </summary>
        map<string, FormSettings*> forms;

        /// <summary>
        /// inputs restored from disk
        /// </summary>
        vector<SavedValue> restoredInputs;

        /// <summary>
        /// root folder to serve content from
        /// </summary>
        string theFolder;

		// private constructor
        ManagerImpl()
        {
			theServer = NULL;
			theFolder = "c:\\www";
        }

        /// <summary>
        /// Get top page with frames
        /// </summary>
        /// <returns>html</returns>
        string GetTopPage()
        {
            HtmlBuilder b;
            b.open("html");
            b.open("head");
            b.open("title");
            b.append("Web Config");
            b.close("title");
            b.close("head");
            b.open("frameset", b.attr("cols", "200,*"));
            b.open("frame", b.attr("src", "menu.cgi") + b.attr("name", "menu")); b.close("frame");
            b.open("frame", b.attr("src", "contents.cgi") + b.attr("name", "contents")); b.close("frame");
            b.open("noframes");
            b.append("A browser which supports frame display is required for browsing this page.");

            return b.ToString();
        }

        /// <summary>
        /// Get menu with links to each form
        /// </summary>
        /// <returns>html</returns>
        string GetMenuPage()
        {
            HtmlBuilder b;
            b.open("html");
            b.open("head");
            b.open("script", b.attr("language", "javascript"));
            b.append("function frmUpdate(page){ parent.contents.location=page; }\n");
            b.close("script");
            b.close("head");
            b.open("body");
            b.open("fieldset");
            b.open("table");

            set<string> uniqueNames;
			for (map<string, InputBase*>::iterator i = inputs.begin(); i != inputs.end(); ++i)
            {
                string s = (*i).second->pForm->Name;
                if (uniqueNames.find(s) == uniqueNames.end())
                {
                    uniqueNames.insert(s);

                    b.open("tr");
                    b.open("td");
                    b.open("a", b.attr("href", "") +
						b.attr("onClick", b.fmt("frmUpdate('%s.cgi');", s.c_str())));
                    b.append(b.text(s));
                    b.close("a");
                    b.close("td");
                    b.close("tr");
                }
            }

            b.close_all();
            return b.ToString();
        }

        /// <summary>
        /// Get specified form
        /// </summary>
        /// <param name="formName">name of form</param>
        /// <returns>html</returns>
        string GetFormPage(string formName)
        {
            HtmlBuilder b;
            b.open("html");
            b.open("head");
            b.open("title");
            b.append(b.text(formName));
            b.close("title");
            b.open("style", b.attr("type", "text/css"));
            b.append("th    {text-align: right;}\n");
            b.append("td    {padding-left: 1em; text-align: left;}\n");
            b.close("style");
            b.include_js("scripts/slider.js");
            b.include_css("styles/slider.css");
            b.open("script", b.attr("type", "text/javascript"));
            b.append("function doclick(sel){\n");
            b.append("  sel.value=\"True\";\n");
            b.append("  sel.form.submit();\n");
            b.append("}\n");
            b.close("script");
            b.close("head");
            b.open("body");
            b.open("h2");
            b.append(b.text(formName));
            b.close("h2");
            b.hr();
            b.open("form", b.attr("name", formName) +
                           b.attr("action", b.fmt("%s.cgi", formName.c_str())) +
                           b.attr("method", "post"));
            b.open("table");
			for (map<string, InputBase*>::iterator i = inputs.begin(); i != inputs.end(); ++i)
			{
                InputBase* input = (*i).second;
                if (input->pForm->Name == formName)
                {
                    b.append(input->ToHtml());
                }
            }
            b.open("tr");
            b.open("th");
            b.close("th");
            b.open("td");

            FormSettings* form = GetFormSettings(formName);
            if (!form->AutoSubmit)
            {
                b.open("input", b.attr("type", "submit") + b.attr("value", "SUBMIT"));
                b.close("input");
                b.open("input", b.attr("type", "reset") + b.attr("value", "RESET"));
                b.close("input");
            }

            b.close("td");
            b.close("tr");
            b.close("table");
            b.close("form");

            // required "linkware" credits
            b.hr();
            b.link("http://carpe.ambiprospect.com/slider/", "sliders by CARPE Design");

            return b.ToString();
        }

        /// <summary>
        /// Update the inputs from the client
        /// </summary>
        /// <param name="cgivars">name-value pairs</param>
        void OnPost(map<string, string> cgivars)
        {
			for (map<string, string>::iterator i = cgivars.begin(); i != cgivars.end(); ++i)
            {
				map<string, InputBase*>::iterator j = inputs.find((*i).first);
                if (j != inputs.end())
                {
                    (*j).second->SetValue((*i).second);
                }
			}
		}

		/// <summary>
        /// Respond to HTTP requests
        /// </summary>
        /// <param name="rq">request parameters</param>
        /// <param name="rp">response parameters</param>
        void OnResponse(const HTTPRequestParams& rq, HTTPResponse& rp)
        {
            // Handle post
            if (rq.Method == "POST")
            {
				map<string, string> cgivars;

				string postStr = rq.BodyData; //Encoding.ASCII.GetString(rq.BodyData, 0, rq.BodySize);
				vector<string> settings;
				StringHelper::Split(postStr, "&;", settings);
				for (vector<string>::iterator i = settings.begin(); i != settings.end(); ++i)
                {
					const string& setting = (*i);
					vector<string> nameValue;
					StringHelper::Split(setting, "=", nameValue);
                    if (nameValue.size() == 2 && nameValue[1] != "")
                    {
						string name = HttpUtility::UrlDecode(nameValue[0]);
                        string value = HttpUtility::UrlDecode(nameValue[1]);
                        cgivars[name] = value;
                    }
                }

                OnPost(cgivars);
                // respond same as for GET
            }

            // handle top using frames
            if (rq.URL == "/")
            {
                string html = GetTopPage();
                rp.BodyData = html; //Encoding.ASCII.GetBytes(html);
                return;
            }

            // handle generated menu and forms
			if (Path::GetExtension(rq.URL) == ".cgi")
            {
                if (rq.URL == "/menu.cgi")
                {
                    string html = GetMenuPage();
                    rp.BodyData = html; //Encoding.ASCII.GetBytes(html);
                }
                else // contents
                {
					string formName = Path::GetFileNameWithoutExtension(rq.URL);
                    string html = GetFormPage(formName);
                    rp.BodyData = html; //Encoding.ASCII.GetBytes(html);
                }
                return;
            }

            string path = theFolder + rq.URL;
			bool valid = (path.find("..") == string::npos); // make it secure

			if (valid && Path::FileExists(path))
            {
				if (Path::FileExists(path + "index.htm"))
                    path += "\\index.htm";
                else
                {
					vector<string> dirs, files;
					Path::GetDirectories(path, dirs);
					Path::GetFiles(path, files);

                    HtmlBuilder b;
                    b.open("html");
                    b.open("head");
                    b.close("head");
                    b.open("body");
                    b.open("h2");
                    b.append(b.text("Folder listing for " + path.substr(theFolder.length() + 1)));
                    b.close("h2");
                    for (unsigned int i = 0; i < dirs.size(); i++)
                    {
						b.link(rq.URL + "/" + Path::GetFileName(dirs[i]),
							"[" + Path::GetFileName(dirs[i]) + "]");
                        b.br();
                    }
                    for (unsigned int i = 0; i < files.size(); i++)
                    {
						b.link(rq.URL + "/" + Path::GetFileName(files[i]),
							"[" + Path::GetFileName(files[i]) + "]");
                        b.br();
                    }
                    rp.BodyData = b.ToString(); //Encoding.ASCII.GetBytes(b.ToString());
                    return;
                }
            }

			if (valid && Path::FileExists(path))
            {
#if 0
                RegistryKey rk = Registry.ClassesRoot.OpenSubKey(Path.GetExtension(path), true);

                // Get the data from a specified item in the key.
                String s = (String)rk.GetValue("Content Type");
#endif

                string s = "???";	// FIXME

				rp.fs.open(path.c_str(), ios::in|ios::binary);
                if (s != "")
                    rp.Headers["Content-type"] = s;
            }
            else
            {
				rp.Status = (int)RESPONSE_NOT_FOUND;

                HtmlBuilder b;
                b.open("html");
                b.open("head");
                b.close("head");
                b.open("body");
                b.append(b.text("File not found!!"));

                rp.BodyData = b.ToString(); //Encoding.ASCII.GetBytes(b.ToString());
            }

        }

        /// <summary>
        /// Get the root folder
        /// </summary>
        string GetFolder()
        {
            return theFolder;
        }

        /// <summary>
        /// Startup the server
        /// </summary>
        void Startup(int thePort, string theFolder)
        {
            assert(NULL == theServer);

			static ManagerImpl* ProxyInstance = 0;
			struct Proxy
			{
				static void OnResponse(const HTTPRequestParams& rq, HTTPResponse& rp)
				{
					ProxyInstance->OnResponse(rq, rp);
				}
			};

			ProxyInstance = this;

            this->theFolder = theFolder;
			theServer = new HTTPServer(&Proxy::OnResponse);
            theServer->Start(thePort);

            LoadInputs();
        }

        /// <summary>
        /// Shutdown the server
        /// </summary>
        void Shutdown()
        {
            assert(NULL != theServer);

            theServer->Stop();
			delete theServer;
			theServer = NULL;

            SaveInputs();

            inputs.clear();
            forms.clear();
        }

        /// <summary>
        /// Load input values to restore
        /// </summary>
        void LoadInputs()
        {
            restoredInputs.clear();

			IniFile ini(theFolder + "/WebConfig.ini");
			vector<string> keys;
			if (ini.ReadSectionKeys("Inputs", keys) > 0)
			{
				for (vector<string>::iterator i = keys.begin(); i != keys.end(); ++i)
				{
					string key = (*i);
					string value = ini.ReadString("Inputs", key, "");
					restoredInputs.push_back(SavedValue(key, value));
				}
			}
        }

        /// <summary>
        /// Save input values for forms with AutoSave
        /// </summary>
        void SaveInputs()
        {
			IniFile ini(theFolder + "/WebConfig.ini");
			for (map<string,InputBase*>::iterator i = inputs.begin(); i != inputs.end(); ++i)
            {
				InputBase* input = (*i).second;
                if (input->pForm->AutoSave && !input->ToString().empty())
				{
					ini.WriteString("Inputs", (*i).first, input->ToString());
				}
            }
        }

        /// <summary>
        /// Update the manager; should be called periodically
        /// </summary>
        void Update()
        {
            theServer->Update();
        }

        /// <summary>
        /// Get form settings from name
        /// </summary>
        /// <returns></returns>
        FormSettings* GetFormSettings(string formName)
        {
			map<string, FormSettings*>::iterator i = forms.find(formName);
            if (i != forms.end())
            {
                return (*i).second;
            }
            else
            {
                FormSettings* form = new FormSettings(formName);
                forms[formName] = form;
                return form;
            }
        }

        /// <summary>
        /// Add an input to the dictionary
        /// </summary>
        /// <param name="input">form input</param>
        void AddInput(InputBase* input)
        {
			map<string, InputBase*>::iterator i = inputs.find(input->UniqueID);
            if (i == inputs.end())
            {
				for (vector<SavedValue>::iterator j = restoredInputs.begin(); j != restoredInputs.end(); ++j)
				{
					if ((*j).UniqueID == input->UniqueID)
					{
						// restore value
						input->SetValue((*j).Value);
						break;
					}
				}
                inputs[input->UniqueID] = input;
            }
        }

        /// <summary>
        /// Remove an input from the dictionary
        /// </summary>
        void RemoveInput(InputBase* input)
        {
			map<string, InputBase*>::iterator i = inputs.find(input->UniqueID);
            if (i != inputs.end())
            {
                inputs.erase(i);
            }
        }
    };

	/// <summary>
	/// Constructor
	/// </summary>
	Manager::Manager()
	{
		pImpl = new ManagerImpl;
	}

	/// <summary>
	/// Startup the server
	/// </summary>
	void Manager::Startup(int thePort, std::string theFolder)
	{
		pImpl->Startup(thePort, theFolder);
	}

	/// <summary>
	/// Shutdown the server
	/// </summary>
	void Manager::Shutdown()
	{
		pImpl->Shutdown();
	}

	/// <summary>
	/// Update the manager; should be called periodically
	/// </summary>
	void Manager::Update()
	{
		pImpl->Update();
	}

	/// <summary>
	/// Get form settings from name
	/// </summary>
	FormSettings* Manager::GetFormSettings(std::string formName)
	{
		return pImpl->GetFormSettings(formName);
	}

	/// <summary>
	/// Add an input to the dictionary
	/// </summary>
	void Manager::AddInput(InputBase* input)
	{
		pImpl->AddInput(input);
	}

	/// <summary>
	/// Remove an input from the dictionary
	/// </summary>
	void Manager::RemoveInput(InputBase* input)
	{
		pImpl->RemoveInput(input);
	}
}
