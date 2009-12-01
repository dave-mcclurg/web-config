#ifndef INIFILE_H
#define INIFILE_H

#include <string>
#include <vector>

class IniFile
{
	std::string m_path;

public:

	IniFile(const std::string path) { m_path = path; }

	int ReadSectionKeys(const std::string& szSection, std::vector<std::string>& keyList);
	int ReadInteger(const std::string& szSection, const std::string& szKey, int iDefaultValue);
	float ReadFloat(const std::string& szSection, const std::string& szKey, float fltDefaultValue);
	bool ReadBoolean(const std::string& szSection, const std::string& szKey, bool bolDefaultValue);
	std::string ReadString(const std::string& szSection, const std::string& szKey, const std::string& szDefaultValue);
	void WriteInteger(const std::string& szSection, const std::string& szKey, int iValue);
	void WriteFloat(const std::string& szSection, const std::string& szKey, float fltValue);
	void WriteBoolean(const std::string& szSection, const std::string& szKey, bool bolValue);
	void WriteString(const std::string& szSection, const std::string& szKey, const std::string& szValue);
};

#endif // #ifndef CONVERT_H
