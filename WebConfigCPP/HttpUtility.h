#ifndef HTTPUTILITY_H
#define HTTPUTILITY_H

#include <string>

namespace WebConfig
{
	class HttpUtility
	{
	public:
		/// converts printable characters into URL chars like %20
		static std::string UrlDecode(std::string url)
		{
			std::string result;
			for (const char* s = url.c_str(); *s != 0; ++s)
			{
				if ((*s <= '/' && *s != '.' && *s != '-') ||
					(*s >= ':' && *s <= '?') ||
					(*s >= '[' && *s <= '`' && *s != '_'))
				{
					char temp[3] = "00";
					_itoa_s(*s, temp, sizeof(temp), 16);

					result += '%';
					result += temp;
				}
				else
				{
					result += *s;
				}
			}
			return result;
		}
	};
}

#endif // #ifndef HTTPUTILITY_H
