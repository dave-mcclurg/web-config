// WebConfig - Use a web browser to configure your application
// Copyright (c) 2009 David McClurg <dpm@efn.org>
// Under the MIT License, details: License.txt.

using System;
using System.Collections.Generic;
using System.Web;
using System.Text;
using System.Diagnostics;

namespace WebConfig
{
    /// <summary>
    /// Simple html sting builder
    /// </summary>
    public class HtmlBuilder
    {
        Stack<string> tagStack;
        StringBuilder result;

        /// <summary>
        /// Constructor
        /// </summary>
        public HtmlBuilder()
        {
            tagStack = new Stack<string>();
            result = new StringBuilder();
        }

        /// <summary>
        /// return string with formatted arguments
        /// </summary>
        public string fmt(string fmt, params string[] args)
        {
            return String.Format(fmt, args);
        }

        /// <summary>
        /// return html encoded text
        /// </summary>
        public string text(string s)
        {
            return HttpUtility.HtmlEncode(s);
        }

        /// <summary>
        /// return html encoded attributes
        /// </summary>
        public string attr(params string[] sa)
        {
            Debug.Assert((sa.Length % 2) == 0);

            string s = "";
            for (int i = 0; i < sa.Length; i += 2)
            {
                s += String.Format(" {0}=\"{1}\"",
                   HttpUtility.HtmlAttributeEncode(sa[i].ToString()),
                   HttpUtility.HtmlAttributeEncode(sa[i + 1].ToString()));
            }
            return s;
        }

        /// <summary>
        /// append string to result
        /// </summary>
        public void append(string s)
        {
            result.Append(s);
        }

        /// <summary>
        /// Converts result into a string
        /// </summary>
        public override string ToString()
        {
            close_all();
            return result.ToString();
        }

        /// <summary>
        /// open a tag with attributes
        /// </summary>
        /// <param name="tag">name of tag</param>
        /// <param name="at">attributes for tag</param>
        public void open(string tag, string at)
        {
            tagStack.Push(tag);
            append(fmt("<{0}{1}>\n", tag, at));
        }

        /// <summary>
        /// open a tag with no attributes
        /// </summary>
        /// <param name="tag">name of tag</param>
        public void open(string tag)
        {
            //hack to add doctype to beginning of html page
            if (tag.ToLower() == "html")
            {
                append("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">\n");
            }

            tagStack.Push(tag);
            append(fmt("<{0}>\n", tag));

            //hack to add meta info to head section
            if (tag.ToLower() == "head")
            {
                append("<META http-equiv=Content-Type content=\"text/html; charset=windows-1252\">\n");
            }
        }

        /// <summary>
        /// close a tag
        /// </summary>
        /// <param name="tag">name of tag</param>
        public void close(string tag)
        {
            Debug.Assert(tagStack.Count > 0);
            string top = tagStack.Pop();
            Debug.Assert(top == tag);
            append(fmt("</{0}>\n", tag));
        }

        /// <summary>
        /// close all tags
        /// </summary>
        public void close_all()
        {
            while (tagStack.Count > 0)
            {
                close(tagStack.Peek());
            }
        }

        /// <summary>
        /// include javascript file
        /// </summary>
        /// <param name="name">path of javascript file</param>
        public void include_js(string name)
        {
            append(fmt("<script{0}></script>",
                     attr("src", name,
                          "type", "text/javascript")));
        }

        /// <summary>
        /// include css file
        /// </summary>
        /// <param name="name">path of css file</param>
        public void include_css(string name)
        {
            append(fmt("<link{0}>",
                     attr("rel", "stylesheet",
                          "href", name,
                          "type", "text/css")));
        }

        /// <summary>
        /// append a link
        /// </summary>
        /// <param name="url">location</param>
        /// <param name="s">text</param>
        public void link(string url, string s)
        {
            open("a", attr("href", url));
            append(s);
            close("a");
        }

        /// <summary>
        /// append an image
        /// </summary>
        /// <param name="src">path of image</param>
        public void image(string src)
        {
            open("img", attr("src", src));
            close("img");   // proper close for XHTML
        }
        
        /// <summary>
        /// add a non-breaking space
        /// </summary>
        public void nbsp()
        {
            append("&nbsp;");
        }

        /// <summary>
        /// add a break
        /// </summary>
        public void br()
        {
            append("<br>");
        }

        /// <summary>
        /// add a horizontal rule
        /// </summary>
        public void hr()
        {
            append("<hr>");
        }
    }
}
