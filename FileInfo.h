#ifndef FILEINFO_H_
#define FILEINFO_H_

#include <boost/filesystem.hpp>

class FileInfo {
public:
	FileInfo(const std::string& name, const long long size);
	const std::string& getName() const { return name;};
	long long getSize() const { return size; };
	long long getSoFar() const { return soFar; };
	void addSoFar(const long long readSize) { soFar += readSize; };
	bool isChecked() const { return checked; };

private:
	std::string name;

	long long size;
	long long soFar;
	bool checked;
};

#endif //FILEINFO_H_
