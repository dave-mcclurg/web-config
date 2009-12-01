// WebConfig - Use a web browser to configure your application
// Copyright (c) 2009 David McClurg <dpm@efn.org>
// Under the MIT License, details: License.txt.

using System;
using System.Text;
using System.Net.Sockets;
using System.Net;
using System.Collections.Generic;
using System.Windows.Forms;
using System.IO;
using System.Diagnostics;
using System.Web;
using Microsoft.Win32;

namespace WebConfig
{
    /// <summary>
    /// This class is an http server and manages the list of inputs
    /// </summary>
    public sealed class Manager
    {
        #region singleton pattern
        // http://www.yoda.arachsys.com/csharp/singleton.html
        private static readonly Manager instance = new Manager();
        public static Manager Instance { get { return instance; } }

        // Explicit static constructor to tell C# compiler
        // not to mark type as beforefieldinit
        static Manager() { }

        private Manager()
        {
        }
        #endregion

        /// <summary>
        /// http server
        /// </summary>
        HTTPServer theServer = null;

        /// <summary>
        /// Dictionary of inputs keyed by UniqueID
        /// </summary>
        Dictionary<string, InputBase> inputs = new Dictionary<string, InputBase>();

        /// <summary>
        /// Dictionary of forms keyed by Name
        /// </summary>
        Dictionary<string, FormSettings> forms = new Dictionary<string, FormSettings>();

        /// <summary>
        /// Saved value for forms with AutoSave
        /// </summary>
        public struct SavedValue
        {
            public string UniqueID;
            public string Value;
            public SavedValue(string id, string v) { UniqueID = id; Value = v; }
        }

        /// <summary>
        /// inputs restored from disk
        /// </summary>
        List<SavedValue> restoredInputs = null;

        /// <summary>
        /// root folder to serve content from
        /// </summary>
        string theFolder = @"c:\www";

        /// <summary>
        /// Get the root folder
        /// </summary>
        public string GetFolder()
        {
            return theFolder;
        }

        /// <summary>
        /// Startup the server
        /// </summary>
        public void Startup(int thePort, string theFolder)
        {
            Debug.Assert(null == theServer);

            this.theFolder = theFolder;
            this.theServer = new HTTPServer(OnResponse);
            this.theServer.Start(thePort);

            LoadInputs();
        }

        /// <summary>
        /// Shutdown the server
        /// </summary>
        public void Shutdown()
        {
            Debug.Assert(null != theServer);

            theServer.Stop();

            SaveInputs();

            inputs.Clear();
            forms.Clear();
        }

        /// <summary>
        /// Load input values to restore
        /// </summary>
        public void LoadInputs()
        {
            restoredInputs =
                Settings.LoadFromXml<List<SavedValue>>(theFolder + @"\WebConfig.xml", null);
        }

        /// <summary>
        /// Save input values for forms with AutoSave
        /// </summary>
        public void SaveInputs()
        {
            // save input values
            List<SavedValue> saveInputs = new List<SavedValue>();
            foreach (var pair in inputs)
            {
                if (pair.Value.Form.AutoSave)
                {
                    if (pair.Value.GetType() != typeof(InputButton) &&
                        pair.Value.GetType() != typeof(InputLink))
                    {
                        saveInputs.Add(new SavedValue(pair.Key, pair.Value.GetValue()));
                    }
                }
            }
            Settings.SaveToXml<List<SavedValue>>(theFolder + @"\WebConfig.xml", saveInputs);
        }

        /// <summary>
        /// Update the manager; should be called periodically
        /// </summary>
        public void Update()
        {
            theServer.Update();
        }

        /// <summary>
        /// Get form settings from name
        /// </summary>
        /// <returns></returns>
        public FormSettings GetForm(string formName)
        {
            if (forms.ContainsKey(formName))
            {
                return forms[formName];
            }
            else
            {
                FormSettings form = new FormSettings(formName);
                forms.Add(formName, form);
                return form;
            }
        }

        /// <summary>
        /// Add an input to the dictionary
        /// </summary>
        /// <param name="input">form input</param>
        public void AddInput(InputBase input)
        {
            if (!inputs.ContainsKey(input.UniqueID))
            {
                if (null != restoredInputs)
                {
                    foreach (SavedValue i in restoredInputs)
                    {
                        if (i.UniqueID == input.UniqueID)
                        {
                            // restore value
                            input.SetValue(i.Value);
                            break;
                        }
                    }
                }
                inputs.Add(input.UniqueID, input);
            }
        }

        /// <summary>
        /// Remove an input from the dictionary
        /// </summary>
        public void RemoveInput(InputBase input)
        {
            if (inputs.ContainsKey(input.UniqueID))
            {
                inputs.Remove(input.UniqueID);
            }
        }

        /// <summary>
        /// Get top page with frames
        /// </summary>
        /// <returns>html</returns>
        private string GetTopPage()
        {
            HtmlBuilder b = new HtmlBuilder();
            b.open("html");
            b.open("head");
            b.open("title");
            b.append("Web Config");
            b.close("title");
            b.close("head");
            b.open("frameset", b.attr("cols", "200,*"));
            b.open("frame", b.attr("src", "menu.cgi", "name", "menu")); b.close("frame");
            b.open("frame", b.attr("src", "contents.cgi", "name", "contents")); b.close("frame");
            b.open("noframes");
            b.append("A browser which supports frame display is required for browsing this page.");

            return b.ToString();
        }

        /// <summary>
        /// Get menu with links to each form
        /// </summary>
        /// <returns>html</returns>
        private string GetMenuPage()
        {
            HtmlBuilder b = new HtmlBuilder();
            b.open("html");
            b.open("head");
            b.open("script", b.attr("language", "javascript"));
            b.append("function frmUpdate(page){ parent.contents.location=page; }\n");
            b.close("script");
            b.close("head");
            b.open("body");
            b.open("fieldset");
            b.open("table");

            List<string> uList = new List<string>();
            foreach (var pair in inputs)
            {
                string s = pair.Value.Form.Name;
                if (!uList.Contains(s))
                {
                    uList.Add(s);

                    b.open("tr");
                    b.open("td");
                    b.open("a", b.attr("href", "", "onClick", b.fmt("frmUpdate('{0}.cgi');", s)));
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
        private string GetFormPage(string formName)
        {
            HtmlBuilder b = new HtmlBuilder();
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
            b.open("form", b.attr("name", formName,
                                "action", b.fmt("{0}.cgi", formName),
                                "method", "post"));
            b.open("table");
            foreach (var pair in inputs)
            {
                InputBase input = pair.Value;
                if (input.Form.Name == formName)
                {
                    b.append(input.GetHtml());
                }
            }
            b.open("tr");
            b.open("th");
            b.close("th");
            b.open("td");

            FormSettings form = GetForm(formName);
            if (!form.AutoSubmit)
            {
                b.open("input", b.attr("type", "submit", "value", "SUBMIT"));
                b.close("input");
                b.open("input", b.attr("type", "reset", "value", "RESET"));
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
        private void OnPost(Dictionary<string, string> cgivars)
        {
            foreach (var pair in cgivars)
            {
                if (inputs.ContainsKey(pair.Key))
                {
                    // try to set the value of the input
                    try
                    {
                        InputBase input = inputs[pair.Key];
                        input.SetValue(pair.Value);
                    }
                    catch (Exception e)
                    {
                        Debug.WriteLine(e.ToString());
                    }
                }
            }
        }

        /// <summary>
        /// Respond to HTTP requests
        /// </summary>
        /// <param name="rq">request parameters</param>
        /// <param name="rp">response parameters</param>
        private void OnResponse(ref HTTPRequestParams rq, ref HTTPResponse rp)
        {
            // Handle post
            if (rq.Method == "POST")
            {
                string postStr = Encoding.ASCII.GetString(rq.BodyData, 0, rq.BodySize);
                Dictionary<string, string> cgivars = new Dictionary<string, string>();

                string[] settings = postStr.Split("&;".ToCharArray(), StringSplitOptions.RemoveEmptyEntries);
                foreach (string setting in settings)
                {
                    string[] nameValue = setting.Split("=".ToCharArray(), 2);
                    if (nameValue.Length == 2 && nameValue[1] != "")
                    {
                        string name = HttpUtility.UrlDecode(nameValue[0]);
                        string value = HttpUtility.UrlDecode(nameValue[1]);
                        cgivars.Add(name, value);
                    }
                }

                this.OnPost(cgivars);
                // respond same as for GET
            }

            // handle top using frames
            if (rq.URL == "/")
            {
                string html = this.GetTopPage();
                rp.BodyData = Encoding.ASCII.GetBytes(html);
                return;
            }

            // handle generated menu and forms
            if (rq.URL.EndsWith(".cgi"))
            {
                if (rq.URL == "/menu.cgi")
                {
                    string html = this.GetMenuPage();
                    rp.BodyData = Encoding.ASCII.GetBytes(html);
                }
                else // contents
                {
                    string formName = System.IO.Path.GetFileNameWithoutExtension(rq.URL);
                    string html = this.GetFormPage(formName);
                    rp.BodyData = Encoding.ASCII.GetBytes(html);
                }
                return;
            }

            string path = theFolder + "\\" + rq.URL.Replace("/", "\\");
            path = Path.GetFullPath(path);  // get absolute path
            bool valid = path.StartsWith(theFolder);   // make it secure

            if (valid && Directory.Exists(path))
            {
                if (File.Exists(path + "index.htm"))
                    path += "\\index.htm";
                else
                {
                    string[] dirs = Directory.GetDirectories(path);
                    string[] files = Directory.GetFiles(path);

                    HtmlBuilder b = new HtmlBuilder();
                    b.open("html");
                    b.open("head");
                    b.close("head");
                    b.open("body");
                    b.open("h2");
                    b.append(b.text("Folder listing for " + path.Substring(theFolder.Length + 1)));
                    b.close("h2");
                    for (int i = 0; i < dirs.Length; i++)
                    {
                        b.link(rq.URL + "/" + Path.GetFileName(dirs[i]),
                            "[" + Path.GetFileName(dirs[i]) + "]");
                        b.br();
                    }
                    for (int i = 0; i < files.Length; i++)
                    {
                        b.link(rq.URL + "/" + Path.GetFileName(files[i]),
                            "[" + Path.GetFileName(files[i]) + "]");
                        b.br();
                    }
                    rp.BodyData = Encoding.ASCII.GetBytes(b.ToString());
                    return;
                }
            }

            if (valid && File.Exists(path))
            {
                RegistryKey rk = Registry.ClassesRoot.OpenSubKey(Path.GetExtension(path), true);

                // Get the data from a specified item in the key.
                String s = (String)rk.GetValue("Content Type");

                // Open the stream and read it back.
                rp.fs = File.Open(path, FileMode.Open);
                if (s != "")
                    rp.Headers["Content-type"] = s;
            }
            else
            {
                rp.Status = (int)HTTPResponseStatus.NOT_FOUND;

                HtmlBuilder b = new HtmlBuilder();
                b.open("html");
                b.open("head");
                b.close("head");
                b.open("body");
                b.append(b.text("File not found!!"));

                rp.BodyData = Encoding.ASCII.GetBytes(b.ToString());
            }

        }
    }
}
