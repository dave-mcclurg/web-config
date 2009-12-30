// WebConfig - Use a web browser to configure your application
// Copyright (c) 2009 David McClurg <dpm@efn.org>
// Under the MIT License, details: License.txt.

#include "HTMLBuilder.h"
#include "HttpUtility.h"
#include <cassert>
#include <stdarg.h>

using namespace std;

namespace WebConfig
{
	/// <summary>
	/// return string with formatted arguments
	/// </summary>
	string HtmlBuilder::fmt(const char* format, ...)
	{
		char temp[1024];
		va_list args;
		va_start (args, format);
		vsprintf_s (temp, sizeof(temp), format, args);
		va_end (args);
		return string(temp);
	}

	/// <summary>
	/// return html encoded text
	/// </summary>
	string HtmlBuilder::text(const std::string& str)
	{
		return HttpUtility::HtmlEncode(str);
	}

	/// <summary>
	/// return html encoded attributes
	/// </summary>
	string HtmlBuilder::attr(const std::string& name, const std::string& value)
	{
		return fmt(" %s=\"%s\"", text(name).c_str(), text(value).c_str());
	}

	/// <summary>
	/// open a tag with attributes
	/// </summary>
	/// <param name="tag">name of tag</param>
	/// <param name="at">attributes for tag</param>
	void HtmlBuilder::open(const std::string& tag, const std::string& at)
	{
		tagStack.push(tag);
		append(fmt("<%s%s>\n", tag.c_str(), at.c_str()));
	}

	/// <summary>
	/// open a tag with no attributes
	/// </summary>
	/// <param name="tag">name of tag</param>
	void HtmlBuilder::open(const std::string& tag)
	{
		//hack to add doctype to beginning of html page
		if (_stricmp(tag.c_str(), "html") == 0)
		{
			append("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">\n");
		}

		tagStack.push(tag);
		append(fmt("<%s>\n", tag.c_str()));

		//hack to add meta info to head section
		if (_stricmp(tag.c_str(), "head") == 0)
		{
			append("<META http-equiv=Content-Type content=\"text/html; charset=windows-1252\">\n");
		}
	}

	/// <summary>
	/// close a tag
	/// </summary>
	/// <param name="tag">name of tag</param>
	void HtmlBuilder::close(const std::string& tag)
	{
		assert(!tagStack.empty());
		string top = tagStack.top();
		assert(top == tag);
		append(fmt("</%s>\n", top.c_str()));
		tagStack.pop();
	}

	/// <summary>
	/// close all tags
	/// </summary>
	void HtmlBuilder::close_all()
	{
		while (!tagStack.empty())
		{
			close(tagStack.top());
		}
	}

	/// <summary>
	/// include javascript file
	/// </summary>
	/// <param name="name">path of javascript file</param>
	void HtmlBuilder::include_js(const std::string& name)
	{
		append(fmt("<script%s></script>",
			(attr("src", name) + attr("type", "text/javascript")).c_str()));
	}

	/// <summary>
	/// include css file
	/// </summary>
	/// <param name="name">path of css file</param>
	void HtmlBuilder::include_css(const std::string& name)
	{
		append(fmt("<link%s>",
			(attr("rel", "stylesheet") +
			attr("href", name) +
			attr("type", "text/css")).c_str()));
	}

	/// <summary>
	/// append a link
	/// </summary>
	/// <param name="url">location</param>
	/// <param name="s">text</param>
	void HtmlBuilder::link(const std::string& url, const std::string& s)
	{
		open("a", attr("href", url.c_str()));
		append(s);
		close("a");
	}

	/// <summary>
	/// append an image
	/// </summary>
	/// <param name="src">path of image</param>
	void HtmlBuilder::image(const std::string& src)
	{
		open("img", attr("src", src.c_str()));
		close("img");   // proper close for XHTML
	}
}
