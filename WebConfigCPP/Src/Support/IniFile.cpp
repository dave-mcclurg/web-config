#include "IniFile.h"

#include <windows.h>
#include <stdio.h>
#include <string.h>

using namespace std;

int IniFile::ReadSectionKeys(const string& szSection, vector<string>& keyList)
{
    DWORD buffSize = 1024;  // start at this size
    char* pBuffer = NULL;
    DWORD size = 0;
    
    while(1)
    {
        // try to read all the key names into one buffer
        pBuffer = new char[buffSize];
        size = GetPrivateProfileString(szSection.c_str(),
                                    NULL,  // all keys in section
                                    "Error",
                                    pBuffer, // returned string
                                    buffSize,   // length
                                    m_path.c_str());

        if(strcmp(pBuffer,"Error") == 0)
        {
            delete [] pBuffer;
            return 0;
        }
        // if the buffer wasn't big enough,
        // double its size and try again
        if(size >= buffSize-2)
        {
            delete [] pBuffer;
            buffSize = buffSize * 2;
            continue;
        }
        else
        {
            // the buffer was big enough, so move on
            break;
        }
    }

    // now we have all the key names, extract them one by one
    char* movingPtr = pBuffer;
    while(1)
    {
        // end of list is marked by double NULL
        if(*movingPtr == 0)
        {
            break;
        }
        string newStr = movingPtr; // this will stop at next null

        if(newStr.empty())
        {
            continue;
        }
        keyList.push_back(newStr);

        // move past the string and its terminator to next string
        movingPtr += newStr.length()+1;
    }

    delete [] pBuffer;

    return keyList.size();
}
int IniFile::ReadInteger(const string& szSection, const string& szKey, int iDefaultValue)
{
	int iResult = GetPrivateProfileInt(szSection.c_str(), szKey.c_str(), iDefaultValue, m_path.c_str()); 
	return iResult;
}
float IniFile::ReadFloat(const string& szSection, const string& szKey, float fltDefaultValue)
{
	char szResult[256];
	char szDefault[256];
	float fltResult;
	sprintf_s(szDefault, sizeof(szDefault), "%f", fltDefaultValue);
	GetPrivateProfileString(szSection.c_str(), szKey.c_str(), szDefault, szResult, sizeof(szResult), m_path.c_str()); 
	fltResult = (float)atof(szResult);
	return fltResult;
}
bool IniFile::ReadBoolean(const string& szSection, const string& szKey, bool bolDefaultValue)
{
	char szResult[256];
	char szDefault[256];
	bool bolResult;
	sprintf_s(szDefault, sizeof(szDefault), "%s", bolDefaultValue? "True" : "False");
	GetPrivateProfileString(szSection.c_str(), szKey.c_str(), szDefault, szResult, sizeof(szResult), m_path.c_str()); 
	bolResult =  (strcmp(szResult, "True") == 0 || 
		strcmp(szResult, "true") == 0) ? true : false;
	return bolResult;
}
string IniFile::ReadString(const string& szSection, const string& szKey, const string& szDefaultValue)
{
	char szResult[256] = "";
	GetPrivateProfileString(szSection.c_str(),  szKey.c_str(), 
		szDefaultValue.c_str(), szResult, sizeof(szResult), m_path.c_str()); 
	return string(szResult);
}
void IniFile::WriteInteger(const string& szSection, const string& szKey, int iValue)
{
	char szValue[256];
	sprintf_s(szValue, sizeof(szValue), "%d", iValue);
	WritePrivateProfileString(szSection.c_str(),  szKey.c_str(), szValue, m_path.c_str()); 
}
void IniFile::WriteFloat(const string& szSection, const string& szKey, float fltValue)
{
	char szValue[256];
	sprintf_s(szValue, sizeof(szValue), "%f", fltValue);
	WritePrivateProfileString(szSection.c_str(),  szKey.c_str(), szValue, m_path.c_str()); 
}
void IniFile::WriteBoolean(const string& szSection, const string& szKey, bool bolValue)
{
	char szValue[256];
	sprintf_s(szValue, sizeof(szValue), "%s", bolValue ? "True" : "False");
	WritePrivateProfileString(szSection.c_str(),  szKey.c_str(), szValue, m_path.c_str()); 
}
void IniFile::WriteString(const string& szSection, const string& szKey, const string& szValue)
{
	WritePrivateProfileString(szSection.c_str(),  szKey.c_str(), szValue.c_str(), m_path.c_str());
}