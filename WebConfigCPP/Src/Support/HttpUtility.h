#ifndef HTTPUTILITY_H
#define HTTPUTILITY_H

#include <string>

class HttpUtility
{
public:
	static int SingleHexToDecimal(char c)
	{
		if (c >= '0' && c <= '9') return c-'0';
		if (c >= 'a' && c <= 'f') return c-'a'+10;
		if (c >= 'A' && c <= 'F') return c-'A'+10;
		return -1;
	}

	/// converts URL chars like %20 into printable characters
	static std::string UrlDecode(const std::string& str)
	{
		const char* s = str.c_str();
		std::string d = "";

		while(*s)
		{
			if (*s == '%') 
			{
				int digit1 = SingleHexToDecimal(s[1]);
				if (digit1 != -1) 
				{
					int digit2 = SingleHexToDecimal(s[2]);
					if (digit2 != -1) 
					{
						s += 3;
						d += (char)((digit1 << 4) | digit2);
						continue;
					}       
				}       
			}
			d += *s++;
		}
		return d;
	}

	/// converts printable characters into URL chars like %20
	static std::string UrlEncode(const std::string& url)
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

	static std::string HtmlDecode(const std::string& str)
	{
		std::string s = "";
		for (const char* p=str.c_str(); *p!='\0'; p++)
		{
			if (*p == '&') 
			{
				if (!strncmp(p, "&amp;", 5)) {  s += '&'; p += 4; }
				else if (!strncmp(p, "&apos;", 6)) { s += '\''; p += 5; }
				else if (!strncmp(p, "&gt;", 4)) { s += '>'; p += 3; }
				else if (!strncmp(p, "&lt;", 4)) { s += '<'; p += 3; }
				else if (!strncmp(p, "&quot;", 6)) { s += '"'; p += 5; }
				else { s += *p; }
			}
			else 
			{
				s += *p;
			}
		}
		return s;
	}

	static std::string HtmlEncode(const std::string& str)
	{
		std::string s = "";
		for (const char* p=str.c_str(); *p!='\0'; p++)
		{
			switch (*p)
			{
			case '&': s += "&amp;"; break;
			case '\'': s += "&apos;"; break;
			case '>': s += "&gt;"; break;
			case '<': s += "&lt;"; break;
			case '"': s += "&quot;"; break;
			default: s += *p; break;
			}
		}
		return s;
	}
};

#endif // #ifndef HTTPUTILITY_H
