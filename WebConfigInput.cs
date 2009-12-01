// WebConfig - Use a web browser to configure your application
// Copyright (c) 2009 David McClurg <dpm@efn.org>
// Under the MIT License, details: License.txt.

using System;
using System.Diagnostics;

namespace WebConfig
{
    /// <summary>
    /// Settings for input form
    /// </summary>
    public class FormSettings
    {
        public string Name;
        public bool AutoSubmit = false;   // submit onchange or onclick
        public bool AutoSave = false;     // save inputs to disk

        public FormSettings(string name)
        {
            this.Name = name;
        }
    }

    /// <summary>
    /// This class represents a text input for a form
    /// </summary>
    abstract public class InputBase
    {
        /// <summary>
        /// Delegate to get the input value
        /// </summary>
        public GetValueDelegate GetValue;
        public delegate string GetValueDelegate();

        /// <summary>
        /// Delegate to set the input value
        /// </summary>
        public SetValueDelegate SetValue;
        public delegate void SetValueDelegate(string value);

        /// <summary>form this input belongs to</summary>
        public FormSettings Form;

        /// <summary>label for input</summary>
        public string Label;

        /// <summary>Generated unique id name within form</summary>
        public string UniqueID;
        static int idCounter = 0;

        // extra attributes
        public bool ReadOnly = false;   // client cannot change
        public bool Disabled = false;
        public string Title = null;    // similar to a tool tip

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="path">path is "form-name/label-text"</param>
        /// <param name="getValue">lambda expression to get value</param>
        /// <param name="setValue">lambda expression to set value</param>
        public InputBase(string path, GetValueDelegate getValue, SetValueDelegate setValue)
        {
            ++idCounter;
            UniqueID = "input" + idCounter.ToString();

            // split path from "form-name/label-text"
            int index = path.LastIndexOf('/');
            Debug.Assert(-1 != index);

            Label = path.Substring(index + 1);
            string formName = path.Substring(0, index);

            GetValue = getValue;
            SetValue = setValue;

            // make sure the manager knows about this input
            Form = WebConfig.Manager.Instance.GetForm(formName);
            WebConfig.Manager.Instance.AddInput(this);
        }

        /// <summary>
        /// Get extra input attributes
        /// </summary>
        /// <returns>html</returns>
        protected string GetExtraAttributes()
        {
            HtmlBuilder b = new HtmlBuilder();
            if (ReadOnly)
                b.append(b.attr("ReadOnly", "True"));
            if (Disabled)
                b.append(b.attr("Disabled", "True"));
            if (null != Title)
                b.append(b.attr("Title", Title));
            return b.ToString();
        }

        /// <summary>
        /// Get html representation
        /// </summary>
        /// <returns>html</returns>
        abstract public string GetHtml();
    }

    /// <summary>
    /// This class represents a button input for a form
    /// </summary>
    public class InputText : InputBase
    {
        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="path">path is "form-name/label-text"</param>
        /// <param name="getValue">lambda expression to get value</param>
        /// <param name="setValue">lambda expression to set value</param>
        public InputText(string path, GetValueDelegate getValue, SetValueDelegate setValue)
            : base(path, getValue, setValue)
        {
        }

        /// <summary>
        /// Get html representation
        /// </summary>
        /// <returns>html</returns>
        override public string GetHtml()
        {
            HtmlBuilder b = new HtmlBuilder();
            b.open("tr");
            b.open("th"); b.append(b.text(Label + ":")); b.close("th");
            b.open("td");
            b.open("input", b.attr("type", "text",
                                    "name", UniqueID,
                                    "value", GetValue(),
                                    "style", "width: 300px") + GetExtraAttributes());
            return b.ToString();
        }
    }

    /// <summary>
    /// This class represents a button input for a form
    /// </summary>
    public class InputButton : InputBase
    {
        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="path">path is "form-name/label-text"</param>
        /// <param name="pressButton">lambda expression for button press</param>
        public InputButton(string path, SetValueDelegate pressButton)
            : base(path, null, pressButton)
        {
        }

        /// <summary>
        /// Get html representation
        /// </summary>
        /// <returns>html</returns>
        override public string GetHtml()
        {
            HtmlBuilder b = new HtmlBuilder();
            b.open("tr");
            b.open("th"); b.close("th");
            b.open("td");
            b.open("input", b.attr("type", "hidden",
                                    "name", UniqueID));

            string attr = b.attr("type", "button",
                                    "value", Label,
                                    "style", "width: 300px") + GetExtraAttributes();

            // buttons always auto-submit
            string script = b.fmt("doclick(this.form.{0});", UniqueID);
            attr += b.attr("onclick", script);

            b.open("input", attr);
            return b.ToString();
        }
    }

    /// <summary>
    /// This class represents a html link
    /// </summary>
    public class InputLink : InputBase
    {
        string url;

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="path">path is "form-name/label-text"</param>
        public InputLink(string path, string url)
            : base(path, null, null)
        {
            this.url = url;
        }

        /// <summary>
        /// Get html representation
        /// </summary>
        /// <returns>html</returns>
        override public string GetHtml()
        {
            HtmlBuilder b = new HtmlBuilder();
            b.open("tr");
            b.open("th"); b.close("th");
            b.open("td");

            string attr = b.attr("type", "button",
                                    "value", Label,
                                    "style", "width: 300px") + GetExtraAttributes();

            string script = b.fmt("document.location.href = \"{0}\";", url);
            attr += b.attr("onclick", script);

            b.open("input", attr);
            return b.ToString();
        }
    }

    /// <summary>
    /// This class represents a bool input for a form
    /// </summary>
    public class InputBool : InputBase
    {
        string[] options = { "No", "Yes" };

        public delegate bool GetBoolDelegate();
        public delegate void SetBoolDelegate(bool value);

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="path">path is "form-name/label-text"</param>
        /// <param name="getValue">lambda expression to get value</param>
        /// <param name="setValue">lambda expression to set value</param>
        public InputBool(string path, GetBoolDelegate getValue, SetBoolDelegate setValue)
            : base(path, () => getValue().ToString(), (val) => setValue(Convert.ToBoolean(val)))
        {
        }

        /// <summary>
        /// Set the options
        /// </summary>
        /// <param name="sa">array of strings</param>
        public void SetOptions(string yes, string no)
        {
            options[1] = yes;
            options[0] = no;
        }

        /// <summary>
        /// Get html representation
        /// </summary>
        /// <returns>html</returns>
        override public string GetHtml()
        {
            HtmlBuilder b = new HtmlBuilder();

            // Since the value of an unchecked checkbox is not sent via POST
            // we are going to use two radio buttons instead

            // build attributes
            string temp = GetExtraAttributes() + b.attr("type", "radio", "name", UniqueID);
            if (Form.AutoSubmit)
            {
                temp += b.attr("onclick", "this.form.submit();");
            }
            string[] at = new string[2];
            at[1] = temp + b.attr("value", "True");
            at[0] = temp + b.attr("value", "False");
            if (GetValue().ToLower() != "false")
                at[1] += " checked";
            else
                at[0] += " checked";

            b.open("tr");
            b.open("th"); b.append(b.text(Label + ":")); b.close("th");
            b.open("td");
            b.open("input", at[1]); b.close("input"); b.append(b.text(options[1]));
            b.open("input", at[0]); b.close("input"); b.append(b.text(options[0]));
            return b.ToString();
        }
    }

    /// <summary>
    /// This class represents a select input for a form
    /// </summary>
    public class InputSelect : InputBase
    {
        string[] options = { "Disabled", "Enabled" };

        public delegate int GetIntDelegate();
        public delegate void SetIntDelegate(int value);

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="path">path is "form-name/label-text"</param>
        /// <param name="getValue">lambda expression to get value</param>
        /// <param name="setValue">lambda expression to set value</param>
        public InputSelect(string path, GetIntDelegate getValue, SetIntDelegate setValue)
            : base(path, () => getValue().ToString(), (val) => setValue(Convert.ToInt32(val)))
        {
        }

        /// <summary>
        /// Set the options
        /// </summary>
        /// <param name="sa">array of strings</param>
        public void SetOptions(params string[] sa)
        {
            options = sa;
        }

        /// <summary>
        /// Get html representation
        /// </summary>
        /// <returns>html</returns>
        override public string GetHtml()
        {
            HtmlBuilder b = new HtmlBuilder();
            b.open("tr");
            b.open("th"); b.append(b.text(Label + ":")); b.close("th");
            b.open("td");

            string attr = b.attr("name", UniqueID, "style", "width: 300px");
            if (Form.AutoSubmit)
            {
                attr += b.attr("onchange", "this.form.submit();");
            }
            b.open("select", attr);

            for (int i = 0; i < options.Length; ++i)
            {
                attr = b.attr("value", i.ToString()) + GetExtraAttributes();
                if (GetValue().ToLower() == i.ToString())
                    attr += b.attr("selected", "true");
                b.open("option", attr);
                b.append(b.text(options[i]));
                b.close("option");
            }
            return b.ToString();
        }
    }

    /// <summary>
    /// This class represents a slider input for a form
    /// </summary>
    public class InputSlider : InputBase
    {
        /// <summary>
        /// minimum value for slider
        /// </summary>
        float minValue = 0;

        /// <summary>
        /// maximum value for slider
        /// </summary>
        float maxValue = 100;

        /// <summary>
        /// number of decimal places to display
        /// </summary>
        int decimals = 0;

        /// <summary>
        /// discrete values for the slider 0..100
        /// </summary>
        int valueCount = 101;

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="path">path is "form-name/label-text"</param>
        /// <param name="getValue">lambda expression to get value</param>
        /// <param name="setValue">lambda expression to set value</param>
        public InputSlider(string path, GetValueDelegate getValue, SetValueDelegate setValue)
            : base(path, getValue, setValue)
        {
        }

        /// <summary>
        /// Set slider range and display format
        /// </summary>
        /// <param name="minValue">minimum value for slider</param>
        /// <param name="maxValue">maximum value for slider</param>
        /// <param name="decimals">number of decimal places to display</param>
        public void SetRange(float minValue, float maxValue, int decimals)
        {
            this.minValue = minValue;
            this.maxValue = maxValue;
            this.decimals = decimals;
        }

        /// <summary>
        /// Get html representation
        /// </summary>
        /// <returns>html</returns>
        override public string GetHtml()
        {
            // validate value within range and compute percent
            float value = (float)Convert.ToDouble(GetValue());
            if (value < minValue)
                value = minValue;
            else if (value > maxValue)
                value = maxValue;
            float range = maxValue - minValue;
            int pctValue = (int)(value * 100 / range);

            string sliderID = UniqueID;
            string displayID = UniqueID + "_display";

            HtmlBuilder b = new HtmlBuilder();
            b.open("tr");
            b.open("th"); b.append(b.text(Label + ":")); b.close("th");
            b.open("td");

            b.open("div", b.attr("class", "carpe_horizontal_slider_display_combo"));
            b.open("div", b.attr("class", "carpe_horizontal_slider_track"));
            b.open("div", b.attr("class", "carpe_slider_slit"));
            b.nbsp();
            b.close("div");
            b.open("div", b.attr("class", "carpe_slider",
                                "id", sliderID,
                                "display", displayID,
                                "style", b.fmt("left: {0}px;", pctValue.ToString())));
            b.nbsp();
            b.close("div");
            b.close("div");
            b.open("div", b.attr("class", "carpe_slider_display_holder"));

            string attr = b.attr("class", "carpe_slider_display",
                "name", UniqueID,
                "id", displayID,
                "type", "text",
                "from", minValue.ToString(),
                "to", maxValue.ToString(),
                "value", value.ToString(),
                "valuecount", valueCount.ToString(),
                "decimals", decimals.ToString(),
                "typelock", "off") + GetExtraAttributes();
            if (Form.AutoSubmit)
            {
                attr += b.attr("onchange", "this.form.submit();");
            }
            b.open("input", attr);

            b.close("input");
            b.close("div");
            b.close("div");
            return b.ToString();
        }
    }

    /// <summary>
    /// Specialized input slider for ints
    /// </summary>
    public class InputSliderInt : InputSlider
    {
        public delegate int GetIntDelegate();
        public delegate void SetIntDelegate(int value);
        public InputSliderInt(string path, GetIntDelegate getValue, SetIntDelegate setValue)
            : base(path, () => getValue().ToString(), (val) => setValue(Convert.ToInt32(val)))
        {
        }
    }

    /// <summary>
    /// Specialized input slider for floats
    /// </summary>
    public class InputSliderFloat : InputSlider
    {
        public delegate float GetFloatDelegate();
        public delegate void SetFloatDelegate(float value);
        public InputSliderFloat(string path, GetFloatDelegate getValue, SetFloatDelegate setValue)
            : base(path, () => getValue().ToString(), (val) => setValue((float)Convert.ToDouble(val)))
        {
        }
    }
}
