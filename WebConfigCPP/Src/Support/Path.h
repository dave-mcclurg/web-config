#ifndef PATH_H
#define PATH_H

#include <string>
#include <vector>

class Path
{
public:

	/// platform-specific character used to separate directory levels in a path string
	static const char DirectorySeparatorChar;
	static const char AltDirectorySeparatorChar;

	/// platform-specific volume separator character
	static const char VolumeSeparatorChar;

	/// Changes the extension of a path string.
	static std::string ChangeExtension(const std::string& path, const std::string& extension);

	/// Returns the directory information for the specified path string.
	static std::string GetDirectoryName(const std::string& path);

	/// Returns the extension of the specified path string.
	static std::string GetExtension(const std::string& path);

	/// Returns the file name and extension of the specified path string.
	static std::string GetFileName(const std::string& path);

	/// Returns the file name of the specified path string without the extension.
	static std::string GetFileNameWithoutExtension(const std::string& path);

	/// Determines whether a path includes a file name extension.
	static bool HasExtension(const std::string& path);

	/// Determines whether the specified file exists.
	static bool FileExists(const std::string& path);

	/// Determines whether the specified directory exists.
	static bool DirectoryExists(const std::string& path);

	/// Gets the names of subdirectories in the specified directory.
	static int GetDirectories(const std::string& path, std::vector<std::string>& list);

	/// Returns the names of files in a specified directory.
	static int GetFiles(const std::string& path, std::vector<std::string>& list);
};


#endif // #ifndef PATH_H
