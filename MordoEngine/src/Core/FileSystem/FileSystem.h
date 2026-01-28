#pragma once
#include <filesystem>
#include <iostream>
#include <string>

class FileSystem {
public:
	static std::string getPath(const std::string& relativePath) {
		return (std::filesystem::current_path() / relativePath ).string();
	}
};