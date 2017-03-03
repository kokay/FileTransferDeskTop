#include <sstream>
#include "FileInfo.h"

FileInfo::FileInfo(const std::string& name, const long long size)
	: name(name)
	, size(size)
	, soFar(0)
	, checked(true)
{}