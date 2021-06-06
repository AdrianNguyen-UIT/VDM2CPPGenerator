#pragma once

class FileDialogs
{
public:
	static std::optional<std::string> OpenFile(const char* filter = "All Files\0*.*\0\0");
	static std::optional<std::string> SaveFile(const char* filter = "All Files\0*.*\0\0");
};