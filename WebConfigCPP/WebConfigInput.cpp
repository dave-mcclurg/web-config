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
	/// Get html representation
	/// </summary>
	/// <returns>html</returns>
	string InputText::ToHtml()
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

	/// <summary>
	/// Get html representation
	/// </summary>
	/// <returns>html</returns>
	string InputButton::ToHtml()
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

	/// <summary>
	/// Get html representation
	/// </summary>
	/// <returns>html</returns>
	string InputLink::ToHtml()
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

	/// <summary>
	/// Get html representation
	/// </summary>
	/// <returns>html</returns>
	string InputBool::ToHtml()
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

	/// <summary>
	/// Get html representation
	/// </summary>
	/// <returns>html</returns>
	string InputSelect::ToHtml()
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

	/// <summary>
	/// Get html representation
	/// </summary>
	/// <returns>html</returns>
	string InputSlider::ToHtml()
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
}
