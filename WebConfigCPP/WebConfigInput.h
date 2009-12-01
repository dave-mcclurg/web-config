#ifndef WEBCONFIGINPUT_H
#define WEBCONFIGINPUT_H

#include <string>

namespace WebConfig
{
    /// <summary>
    /// Settings for input form
    /// </summary>
    class FormSettings
    {
	public:
		std::string Name;
        bool AutoSubmit;   // submit onchange or onclick
        bool AutoSave;     // save inputs to disk

		FormSettings(std::string name)
        {
            Name = name;
			AutoSubmit = false;
			AutoSave = false;
        }
    };

    /// <summary>
    /// This class represents a text input for a form
    /// </summary>
    class InputBase
    {
	protected:
        /// <summary>
        /// Get extra input attributes
        /// </summary>
        /// <returns>html</returns>
		std::string GetExtraAttributes();

	public:
        /// <summary>form this input belongs to</summary>
        FormSettings* pForm;

        /// <summary>label for input</summary>
        std::string Label;

        /// <summary>Generated unique id</summary>
        std::string UniqueID;

        // extra attributes
        bool ReadOnly;   // client cannot change
        bool Disabled;
        std::string Title;    // similar to a tool tip

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="path">path is "form-name/label-text"</param>
        /// <param name="getValue">lambda expression to get value</param>
        /// <param name="setValue">lambda expression to set value</param>
        InputBase(std::string path);

        /// <summary>
        /// set the input value
        /// </summary>
		virtual void SetValue(std::string value) = 0;

        /// <summary>
        /// get the input value
        /// </summary>
		virtual std::string ToString() = 0;

        /// <summary>
        /// Get html representation of the input table row
        /// </summary>
		virtual std::string ToHtml() = 0;
    };
}

#endif // #ifndef WEBCONFIGINPUT_H
