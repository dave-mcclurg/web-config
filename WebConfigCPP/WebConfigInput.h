#ifndef WEBCONFIGINPUT_H
#define WEBCONFIGINPUT_H

#include <string>
#include <vector>
#include "Convert.h"

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

	/// <summary>
	/// This class represents a button input for a form
	/// </summary>
	class InputText : public InputBase
	{
		std::string& Text;

	public:
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="path">path is "form-name/label-text"</param>
		InputText(std::string path, std::string& text) : InputBase(path), Text(text)
		{
		}

		/// <summary>
		/// set the input value
		/// </summary>
		virtual void SetValue(std::string value)
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
		virtual std::string ToHtml();
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
		InputButton(std::string path, void (*onPress)()) : InputBase(path)
		{
			OnPress = onPress;
		}

		/// <summary>
		/// set the input value
		/// </summary>
		virtual void SetValue(std::string value)
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
		virtual std::string ToHtml();
	};

	/// <summary>
	/// This class represents a html link
	/// </summary>
	class InputLink : public InputBase
	{
		std::string url;

	public:
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="path">path is "form-name/label-text"</param>
		InputLink(std::string path, std::string url) : InputBase(path)
		{
			this->url = url;
		}

		/// <summary>
		/// set the input value
		/// </summary>
		virtual void SetValue(std::string value)
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
		virtual std::string ToHtml();
	};

	/// <summary>
	/// This class represents a bool input for a form
	/// </summary>
	class InputBool : public InputBase
	{
		std::string options[2];
		bool& Flag;

	public:

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="path">path is "form-name/label-text"</param>
		/// <param name="getValue">lambda expression to get value</param>
		/// <param name="setValue">lambda expression to set value</param>
		InputBool(std::string path, bool& flag) : InputBase(path), Flag(flag)
		{
			options[0] = "No";
			options[1] = "Yes";
		}

		/// <summary>
		/// Set the options
		/// </summary>
		/// <param name="sa">array of strings</param>
		void SetOptions(std::string yes, std::string no)
		{
			options[1] = yes;
			options[0] = no;
		}

		/// <summary>
		/// set the input value
		/// </summary>
		virtual void SetValue(std::string value)
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
		virtual std::string ToHtml();
	};

	/// <summary>
	/// This class represents a select input for a form
	/// </summary>
	class InputSelect : public InputBase
	{
		std::vector<std::string> *options;
		int& Selected;

	public:

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="path">path is "form-name/label-text"</param>
		InputSelect(std::string path, int& selected) : InputBase(path), Selected(selected)
		{
			options = new std::vector<std::string>();
			options->push_back("Disabled");
			options->push_back("Enabled");
		}

		/// <summary>
		/// Set the options
		/// </summary>
		/// <param name="sa">array of strings</param>
		void SetOptions(std::vector<std::string> *sa)
		{
			options = sa;
		}

		/// <summary>
		/// set the input value
		/// </summary>
		virtual void SetValue(std::string value)
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
		virtual std::string ToHtml();
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
		InputSlider(std::string path, float& value) : InputBase(path), m_value(value)
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
		virtual void SetValue(std::string value)
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
		virtual std::string ToHtml();
	};

	/// <summary>
	/// Specialized input slider for ints
	/// </summary>
	class InputSliderInt : public InputSlider
	{
		int& m_iValue;
		float m_fValue;

	public:

		InputSliderInt(std::string path, int& value) : InputSlider(path, m_fValue), m_iValue(value)
		{
			m_fValue = (float)m_iValue;
		}

		/// <summary>
		/// set the input value
		/// </summary>
		virtual void SetValue(std::string value)
		{
			m_iValue = Convert::ToInt(value);
			InputSlider::SetValue(Convert::ToString(m_iValue));
		}
	};
}

#endif // #ifndef WEBCONFIGINPUT_H
