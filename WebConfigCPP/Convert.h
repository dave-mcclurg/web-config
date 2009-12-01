#ifndef CONVERT_H
#define CONVERT_H

#include <sstream>
#include <stdexcept>

namespace WebConfig
{
	class Convert
	{
	public:

		class BadConversion : public std::runtime_error
		{
		public:
			BadConversion() : std::runtime_error("BadConversion") { }
		};

		template <class T> static std::string ToString(T x)
		{
			std::ostringstream o;
			if (!(o << x))
				throw BadConversion();
			return o.str();
		}

		static int ToInt(const std::string& s)
		{
			std::istringstream i(s);
			int x;
			if (!(i >> x))
				throw BadConversion();
			return x;
		}

		static float ToFloat(const std::string& s)
		{
			std::istringstream i(s);
			float x;
			if (!(i >> x))
				throw BadConversion();
			return x;
		}

		static bool ToBool(const std::string& s)
		{
			std::istringstream i(s);
			bool x;
			if (!(i >> x))
				throw BadConversion();
			return x;
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
}

#endif // #ifndef CONVERT_H
