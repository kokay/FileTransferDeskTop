#ifndef FILEINFO_H_
#define FILEINFO_H_

#include <boost/filesystem.hpp>

class FileInfo : public boost::filesystem::path {
public:
	static FileInfo parse(const std::string& line);
	FileInfo(const std::string& name, const long long size);
	const std::string& getName() const;
	const std::string& getPath() const;
	long long getSize() const;
	long long getSoFar() const;
	void addSoFar(const long long readSize);
	bool isChecked() const;
	std::string getInfo() const;

private:
	std::string name;
	std::string path;

	long long size;
	long long soFar;
	bool checked;
};

#endif // FILEINFO_H_
