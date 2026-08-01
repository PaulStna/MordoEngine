#pragma once
#include <filesystem>
#include <string>

/// Turns a path written relative to the project into one the OS will accept.
///
/// Resolution happens against the process working directory, not against the
/// executable, so the program must be launched with the project folder — the
/// one containing res/ — as its working directory or every asset load fails.
class FileSystem
{
public:
	/// @param relativePath Path as written in the source, relative to the
	///                     working directory, using forward slashes.
	static std::string getPath(const std::string& relativePath)
	{
		return (std::filesystem::current_path() / relativePath).string();
	}
};
