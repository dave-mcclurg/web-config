#ifndef STRINGHELPER_H
#define STRINGHELPER_H

#include <string>

namespace WebConfig
{
	class StringHelper
	{
	public:

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
}

#endif // #ifndef STRINGHELPER_H
