#ifndef CONVERT_H
#define CONVERT_H

#include <sstream>
#include <stdexcept>

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
};

#endif // #ifndef CONVERT_H
