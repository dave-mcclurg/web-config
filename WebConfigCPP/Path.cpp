#include "Path.h"

#include <iostream>
#include <fstream>
#include <windows.h>

using namespace std;

const char Path::DirectorySeparatorChar('\\');
const char Path::AltDirectorySeparatorChar('/');
const char Path::VolumeSeparatorChar(':');

// ChangeExtension - Changes the extension of a path string.
string Path::ChangeExtension(const string& path, const string& extension)
{
	return GetDirectoryName(path) + GetFileName(path) + extension;
}

/// Returns the directory information for the specified path string.
string Path::GetDirectoryName(const string& path)
{
	// last separator
	int sep = path.rfind(DirectorySeparatorChar);

	int asep = path.rfind(AltDirectorySeparatorChar);
	if (sep == string::npos || (asep != string::npos && asep < sep))
	{
		sep = asep;
	}

	if (sep == string::npos)
	{
		return "";
	}

	return path.substr(0, (sep + 1));
}

/// Returns the extension of the specified path string.
string Path::GetExtension(const string& path)
{
	// get name
	string name = GetFileName(path);

	// last dot
	int dot = name.rfind(".");
	if (dot == string::npos)
		return "";

	// get extension
	return name.substr(dot);
}

/// Returns the file name and extension of the specified path string.
string Path::GetFileName(const string& path)
{
	// number of characters in path
	int psize = path.size(); 

	// number of characters in dir
	int dsize = GetDirectoryName(path).length();

	if (dsize < psize)
	{
		return path.substr(dsize, (psize - dsize));
	}

	// there is no name
	return "";
}

/// Returns the file name of the specified path string without the extension.
string Path::GetFileNameWithoutExtension(const string& path)
{
	// get name
	string name = GetFileName(path);

	// last dot
	int dot = name.rfind(".");

	// no dot
	if (dot == string::npos)
	{
		return name;
	}

	// dot is anywhere else
	return name.substr(0, dot);
}

/// Determines whether a path includes a file name extension.
bool Path::HasExtension(const string& path)
{
	// get name
	string name = GetFileName(path);

	// last dot
	int dot = name.rfind(".");
	return (dot != string::npos);
}

/// Determines whether the specified file exists.
bool Path::FileExists(const string& path)
{
	bool bExists = false;
	fstream fin;
	fin.open(path.c_str(),ios::in);
	bExists = fin.is_open();
	fin.close();
	return bExists;
}

/// Gets the names of subdirectories in the specified directory.
int Path::GetDirectories(const string& path, vector<string>& list)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	hFind = FindFirstFile(path.c_str(), &FindFileData);
	if (hFind != INVALID_HANDLE_VALUE) 
	{
		do {
			if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
			{
				list.push_back(path + FindFileData.cFileName);
			}
		} while(FindNextFile(hFind, &FindFileData));
		FindClose(hFind);
	}

	return list.size();
}

/// Returns the names of files in a specified directory.
int Path::GetFiles(const string& path, vector<string>& list)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	hFind = FindFirstFile(path.c_str(), &FindFileData);
	if (hFind != INVALID_HANDLE_VALUE) 
	{
		do {
			if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			{
				list.push_back(path + FindFileData.cFileName);
			}
		} while(FindNextFile(hFind, &FindFileData));
		FindClose(hFind);
	}

	return list.size();
}
