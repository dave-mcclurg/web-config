// WebConfig - Use a web browser to configure your application
// Copyright (c) 2009 David McClurg <dpm@efn.org>
// Under the MIT License, details: License.txt.

#include "WebConfigInput.h"
#include "WebConfigManager.h"
#include "HTMLBuilder.h"
#include "HTTPServer.h"
#include "Convert.h"

#include <vector>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <assert.h>

using namespace std;

namespace WebConfig
{
	/// <summary>
	/// Get extra input attributes
	/// </summary>
	/// <returns>html</returns>
	string InputBase::GetExtraAttributes()
	{
		HtmlBuilder b;
		if (ReadOnly)
			b.append(b.attr("ReadOnly", "True"));
		if (Disabled)
			b.append(b.attr("Disabled", "True"));
		if (!Title.empty())
			b.append(b.attr("Title", Title));
		return b.ToString();
	}

	/// <summary>
	/// Constructor
	/// </summary>
	/// <param name="path">path is "form-name/label-text"</param>
	/// <param name="getValue">lambda expression to get value</param>
	/// <param name="setValue">lambda expression to set value</param>
	InputBase::InputBase(string path)
	{
		ReadOnly = false;
		Disabled = false;
		Title = "";

		static int idCounter = 0;
		++idCounter;
		UniqueID = "input" + Convert::ToString(idCounter);

		// split path from "form-name/label-text"
		int index = path.find_last_of('/');
		assert(-1 != index);

		Label = path.substr(index + 1);
		string formName = path.substr(0, index);

		// make sure the manager knows about this input
		pForm = WebConfig::Manager::Instance().GetFormSettings(formName);
		WebConfig::Manager::Instance().AddInput(this);
	}

	/// <summary>
	/// This class represents a button input for a form
	/// </summary>
	class InputText : public InputBase
	{
		string& Text;

	public:
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="path">path is "form-name/label-text"</param>
		InputText(string path, string& text) : InputBase(path), Text(text)
		{
		}

		/// <summary>
		/// set the input value
		/// </summary>
		virtual void SetValue(string value)
		{
			Text = value;
		}

		/// <summary>
		/// get the input value
		/// </summary>
		virtual std::string ToString()
		{
			return Text;
		}

		/// <summary>
		/// Get html representation
		/// </summary>
		/// <returns>html</returns>
		virtual string ToHtml()
		{
			HtmlBuilder b;
			b.open("tr");
			b.open("th"); b.append(b.text(Label + ":")); b.close("th");
			b.open("td");
			b.open("input", b.attr("type", "text") +
				b.attr("name", UniqueID) +
				b.attr("value", Text) +
				b.attr("style", "width: 300px") + GetExtraAttributes());
			return b.ToString();
		}
	};

	/// <summary>
	/// This class represents a button input for a form
	/// </summary>
	class InputButton : public InputBase
	{
		typedef void (*Callback)();
		Callback OnPress;

	public:
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="path">path is "form-name/label-text"</param>
		/// <param name="pressButton">lambda expression for button press</param>
		InputButton(string path, void (*onPress)()) : InputBase(path)
		{
			OnPress = onPress;
		}

		/// <summary>
		/// set the input value
		/// </summary>
		virtual void SetValue(string value)
		{
			OnPress();
		}

		/// <summary>
		/// get the input value
		/// </summary>
		virtual std::string ToString()
		{
			return "";
		}

		/// <summary>
		/// Get html representation
		/// </summary>
		/// <returns>html</returns>
		virtual string ToHtml()
		{
			HtmlBuilder b;
			b.open("tr");
			b.open("th"); b.close("th");
			b.open("td");
			b.open("input", b.attr("type", "hidden") +
				b.attr("name", UniqueID));

			string attr = b.attr("type", "button") +
				b.attr("value", Label) +
				b.attr("style", "width: 300px") + GetExtraAttributes();

			// buttons always auto-submit
			string script = b.fmt("doclick(this.form.%s);", UniqueID.c_str());
			attr += b.attr("onclick", script);

			b.open("input", attr);
			return b.ToString();
		}
	};

	/// <summary>
	/// This class represents a html link
	/// </summary>
	class InputLink : public InputBase
	{
		string url;

	public:
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="path">path is "form-name/label-text"</param>
		InputLink(string path, string url) : InputBase(path)
		{
			this->url = url;
		}

		/// <summary>
		/// set the input value
		/// </summary>
		virtual void SetValue(string value)
		{
		}

		/// <summary>
		/// get the input value
		/// </summary>
		virtual std::string ToString()
		{
			return "";
		}

		/// <summary>
		/// Get html representation
		/// </summary>
		/// <returns>html</returns>
		virtual string ToHtml()
		{
			HtmlBuilder b;
			b.open("tr");
			b.open("th"); b.close("th");
			b.open("td");

			string attr = b.attr("type", "button") +
				b.attr("value", Label) +
				b.attr("style", "width: 300px") + GetExtraAttributes();

			string script = b.fmt("document.location.href = \"%s\";", url.c_str());
			attr += b.attr("onclick", script);

			b.open("input", attr);
			return b.ToString();
		}
	};

	/// <summary>
	/// This class represents a bool input for a form
	/// </summary>
	class InputBool : public InputBase
	{
		string options[2];
		bool& Flag;

	public:

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="path">path is "form-name/label-text"</param>
		/// <param name="getValue">lambda expression to get value</param>
		/// <param name="setValue">lambda expression to set value</param>
		InputBool(string path, bool& flag) : InputBase(path), Flag(flag)
		{
			options[0] = "No";
			options[1] = "Yes";
		}

		/// <summary>
		/// Set the options
		/// </summary>
		/// <param name="sa">array of strings</param>
		void SetOptions(string yes, string no)
		{
			options[1] = yes;
			options[0] = no;
		}

		/// <summary>
		/// set the input value
		/// </summary>
		virtual void SetValue(string value)
		{
			Flag = Convert::ToBool(value);
		}

		/// <summary>
		/// get the input value
		/// </summary>
		virtual std::string ToString()
		{
			return Convert::ToString(Flag);
		}

		/// <summary>
		/// Get html representation
		/// </summary>
		/// <returns>html</returns>
		virtual string GetHtml()
		{
			HtmlBuilder b;

			// Since the value of an unchecked checkbox is not sent via POST
			// we are going to use two radio buttons instead

			// build attributes
			string temp = GetExtraAttributes() + b.attr("type", "radio") + b.attr("name", UniqueID);
			if (pForm->AutoSubmit)
			{
				temp += b.attr("onclick", "this.form.submit();");
			}
			string at[2];
			at[1] = temp + b.attr("value", "True");
			at[0] = temp + b.attr("value", "False");
			at[Flag? 1: 0] += " checked";

			b.open("tr");
			b.open("th"); b.append(b.text(Label + ":")); b.close("th");
			b.open("td");
			b.open("input", at[1]); b.close("input"); b.append(b.text(options[1]));
			b.open("input", at[0]); b.close("input"); b.append(b.text(options[0]));
			return b.ToString();
		}
	};

	/// <summary>
	/// This class represents a select input for a form
	/// </summary>
	class InputSelect : public InputBase
	{
		vector<string> *options;
		int& Selected;

	public:

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="path">path is "form-name/label-text"</param>
		InputSelect(string path, int& selected) : InputBase(path), Selected(selected)
		{
			options = new vector<string>();
			options->push_back("Disabled");
			options->push_back("Enabled");
		}

		/// <summary>
		/// Set the options
		/// </summary>
		/// <param name="sa">array of strings</param>
		void SetOptions(vector<string> *sa)
		{
			options = sa;
		}

		/// <summary>
		/// set the input value
		/// </summary>
		virtual void SetValue(string value)
		{
			Selected = Convert::ToInt(value);
		}

		/// <summary>
		/// get the input value
		/// </summary>
		virtual std::string ToString()
		{
			return Convert::ToString(Selected);
		}

		/// <summary>
		/// Get html representation
		/// </summary>
		/// <returns>html</returns>
		virtual string ToHtml()
		{
			HtmlBuilder b;
			b.open("tr");
			b.open("th"); b.append(b.text(Label + ":")); b.close("th");
			b.open("td");

			string attr = b.attr("name", UniqueID) + b.attr("style", "width: 300px");
			if (pForm->AutoSubmit)
			{
				attr += b.attr("onchange", "this.form.submit();");
			}
			b.open("select", attr);

			for (unsigned int i = 0; i < options->size(); ++i)
			{
				attr = b.attr("value", Convert::ToString(i)) + GetExtraAttributes();
				if (i == Selected)
					attr += b.attr("selected", "true");
				b.open("option", attr);
				b.append(b.text((*options)[i]));
				b.close("option");
			}
			return b.ToString();
		}
	};

	/// <summary>
	/// This class represents a slider input for a form
	/// </summary>
	class InputSlider : public InputBase
	{
		float& m_value;

		float minValue;		// minimum value for slider
		float maxValue;		// maximum value for slider
		int decimals;		// number of decimal places to display
		int valueCount;		// discrete values for the slider 0..100

	public:

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="path">path is "form-name/label-text"</param>
		/// <param name="getValue">lambda expression to get value</param>
		/// <param name="setValue">lambda expression to set value</param>
		InputSlider(string path, float& value) : InputBase(path), m_value(value)
		{
			minValue = 0;
			maxValue = 100;
			decimals = 0;
			valueCount = 101;
		}

		/// <summary>
		/// Set slider range and display format
		/// </summary>
		/// <param name="minValue">minimum value for slider</param>
		/// <param name="maxValue">maximum value for slider</param>
		/// <param name="decimals">number of decimal places to display</param>
		void SetRange(float minValue, float maxValue, int decimals)
		{
			this->minValue = minValue;
			this->maxValue = maxValue;
			this->decimals = decimals;
		}

		/// <summary>
		/// set the input value
		/// </summary>
		virtual void SetValue(string value)
		{
			m_value = Convert::ToFloat(value);
		}

		/// <summary>
		/// get the input value
		/// </summary>
		virtual std::string ToString()
		{
			return Convert::ToString(m_value);
		}

		/// <summary>
		/// Get html representation
		/// </summary>
		/// <returns>html</returns>
		virtual string ToHtml()
		{
			// validate value within range and compute percent
			float value = m_value;
			if (value < minValue)
				value = minValue;
			else if (value > maxValue)
				value = maxValue;
			float range = maxValue - minValue;
			int pctValue = (int)(value * 100 / range);

			string sliderID = UniqueID;
			string displayID = UniqueID + "_display";

			HtmlBuilder b;
			b.open("tr");
			b.open("th"); b.append(b.text(Label + ":")); b.close("th");
			b.open("td");

			b.open("div", b.attr("class", "carpe_horizontal_slider_display_combo"));
			b.open("div", b.attr("class", "carpe_horizontal_slider_track"));
			b.open("div", b.attr("class", "carpe_slider_slit"));
			b.nbsp();
			b.close("div");
			b.open("div", b.attr("class", "carpe_slider") +
				b.attr("id", sliderID) +
				b.attr("display", displayID) +
				b.attr("style", b.fmt("left: %dpx;", pctValue)));
			b.nbsp();
			b.close("div");
			b.close("div");
			b.open("div", b.attr("class", "carpe_slider_display_holder"));

			string attr = b.attr("class", "carpe_slider_display") +
				b.attr("name", UniqueID) +
				b.attr("id", displayID) +
				b.attr("type", "text") +
				b.attr("from", Convert::ToString(minValue)) +
				b.attr("to", Convert::ToString(maxValue)) +
				b.attr("value", Convert::ToString(value)) +
				b.attr("valuecount", Convert::ToString(valueCount)) +
				b.attr("decimals", Convert::ToString(decimals)) +
				b.attr("typelock", "off") + GetExtraAttributes();

			if (pForm->AutoSubmit)
			{
				attr += b.attr("onchange", "this.form.submit();");
			}
			b.open("input", attr);

			b.close("input");
			b.close("div");
			b.close("div");
			return b.ToString();
		}
	};

	/// <summary>
	/// Specialized input slider for ints
	/// </summary>
	class InputSliderInt : public InputSlider
	{
		int& m_iValue;
		float m_fValue;

	public:

		InputSliderInt(string path, int& value) : InputSlider(path, m_fValue), m_iValue(value)
		{
			m_fValue = (float)m_iValue;
		}

		/// <summary>
		/// set the input value
		/// </summary>
		virtual void SetValue(string value)
		{
			m_iValue = Convert::ToInt(value);
			InputSlider::SetValue(Convert::ToString(m_iValue));
		}
	};
}
