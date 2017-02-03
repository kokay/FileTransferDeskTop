#include <sstream>
#include <string>
#include "FileInfo.h"

using namespace std;

FileInfo::FileInfo(const std::string& _name, const long long _size)
	: name(_name)
	, size(_size)
	, soFar(0)
	, checked(true)
{}

FileInfo FileInfo::parse(const std::string& line) {
	std::istringstream iss(line);
	std::string name;
	long long size;
	iss >> name >> size;
	return FileInfo(name, size);
}

bool FileInfo::isChecked() const {
	return checked;
}

const std::string& FileInfo::getName() const {
	return name;
}

const std::string& FileInfo::getPath() const {
	return path;
}

long long FileInfo::getSize() const {
	return size;
}

long long FileInfo::getSoFar() const {
	return soFar;
}

void FileInfo::addSoFar(const long long readSize) {
	soFar += readSize;
}

std::string FileInfo::getInfo() const {
    return name + " " + to_string(boost::filesystem::file_size(*this));
}


