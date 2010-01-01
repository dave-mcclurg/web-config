// WebConfig - Use a web browser to configure your application
// Copyright (c) 2009 David McClurg <dpm@efn.org>
// Under the MIT License, details: License.txt.

#include <stack>
#include <string>

namespace WebConfig
{
    /// <summary>
    /// Simple html sting builder
    /// </summary>
    class HtmlBuilder
    {
		std::stack<std::string> tagStack;
		std::string result;

	public:

        /// <summary>
        /// Constructor
        /// </summary>
        HtmlBuilder()
        {
            result = "";
        }

        /// <summary>
        /// return string with formatted arguments
        /// </summary>
        std::string fmt(const char* format, ...);

        /// <summary>
        /// return html encoded text
        /// </summary>
        std::string text(const std::string& str);

        /// <summary>
        /// return html encoded attributes
        /// </summary>
        std::string attr(const std::string& name, const std::string& value);

        /// <summary>
        /// append string to result
        /// </summary>
        void append(const std::string& s)
        {
            result += s;
        }

        /// <summary>
        /// Converts result into a string
        /// </summary>
        std::string ToString()
        {
            close_all();
            return result;
        }

        /// <summary>
        /// open a tag with attributes
        /// </summary>
        /// <param name="tag">name of tag</param>
        /// <param name="at">attributes for tag</param>
        void open(const std::string& tag, const std::string& at);
        void open(const std::string& tag);

        /// <summary>
        /// close a tag
        /// </summary>
        /// <param name="tag">name of tag</param>
        void close(const std::string& tag);
        void close_all();

        /// <summary>
        /// include file
        /// </summary>
        /// <param name="name">path of file</param>
        void include_js(const std::string& name);
        void include_css(const std::string& name);

        /// <summary>
        /// append a link
        /// </summary>
        /// <param name="url">location</param>
        /// <param name="s">text</param>
        void link(const std::string& url, const std::string& s);

        /// <summary>
        /// append an image
        /// </summary>
        /// <param name="src">path of image</param>
        void image(const std::string& src);
        
        /// <summary>
        /// add a non-breaking space
        /// </summary>
        void nbsp()
        {
            append("&nbsp;");
        }

        /// <summary>
        /// add a break
        /// </summary>
        void br()
        {
            append("<br>");
        }

        /// <summary>
        /// add a horizontal rule
        /// </summary>
        void hr()
        {
            append("<hr>");
        }
    };
}
