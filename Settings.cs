// WebConfig - Use a web browser to configure your application
// Copyright (c) 2009 David McClurg <dpm@efn.org>
// Under the MIT License, details: License.txt.

using System;
using System.IO;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Xml;
using System.Xml.Serialization;

namespace WebConfig
{
    public class Settings
    {
        /// <summary>
        /// Save object to Xml file
        /// </summary>
        static public bool SaveToXml<T>(string path, T obj)
        {
            bool result = true;

            try
            {
                StreamWriter sw = new StreamWriter(path);
                new XmlSerializer(obj.GetType()).Serialize(sw, obj);
                sw.Close();
            }
            catch (Exception ex)
            {
                result = false;
                MessageBox.Show(ex.ToString());
            }

            return result;
        }

        /// <summary>
        /// Load object from Xml file
        /// </summary>
        static public T LoadFromXml<T>(string path, T defaultObj)
        {
            T obj = defaultObj;

            if (File.Exists(path))
            {
                try
                {
                    StreamReader sr = new StreamReader(path);
                    obj = (T)new XmlSerializer(typeof(T)).Deserialize(sr);
                    sr.Close();
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.ToString());
                    obj = defaultObj;
                }
            }

            return obj;
        }
    }
}
