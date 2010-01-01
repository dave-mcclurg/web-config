#ifndef STRINGHELPER_H
#define STRINGHELPER_H

#include <string>
#include <algorithm>

class StringHelper
{
public:

#define SPACES " \t\r\n"

	static std::string trim_right (const std::string & s, const std::string & t = SPACES)
	{ 
		std::string d (s); 
		std::string::size_type i (d.find_last_not_of (t));
		if (i == std::string::npos)
			return "";
		else
			return d.erase (d.find_last_not_of (t) + 1) ; 
	}

	static std::string trim_left (const std::string & s, const std::string & t = SPACES) 
	{ 
		std::string d (s); 
		return d.erase (0, s.find_first_not_of (t)) ; 
	}

	static std::string trim (const std::string & s, const std::string & t = SPACES)
	{ 
		std::string d (s); 
		return trim_left (trim_right (d, t), t) ; 
	}

	// returns a lower case version of the std::string 
	static std::string tolower (const std::string & s)
	{
		std::string d (s);

		transform (d.begin (), d.end (), d.begin (), (int(*)(int)) ::tolower);
		return d;
	}

	// returns an upper case version of the std::string 
	static std::string toupper (const std::string & s)
	{
		std::string d (s);

		transform (d.begin (), d.end (), d.begin (), (int(*)(int)) ::toupper);
		return d;
	}

	static int Split(const std::string& seq, const std::string& delims, std::vector<std::string>& list)
	{
		int pos = 0, len = seq.size();
		while (pos < len)
		{
			// remove any delimiters including optional (white)spaces
			while ( (delims.find(seq[pos]) != std::string::npos) && (pos < len) )
				++pos;
			// leave if @eos
			if (pos==len)
				break;

			// Save token data
			std::string str=""; // Init/clear the str token buffer
			while ( (delims.find(seq[pos]) == std::string::npos) && (pos < len) )
				str += seq[pos++];
			// put valid str buffer into the supplied list
			if ( ! str.empty() )
				list.push_back(str);
		}
		return list.size();
	}
};

#endif // #ifndef STRINGHELPER_H
