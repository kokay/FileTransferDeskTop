#include <fstream>
#include "FileTransferClient.h"

void FileTransferClient::connect(const std::string& remoteIpAddress, const int port) {
    boost::asio::ip::tcp::endpoint
            endpoint(boost::asio::ip::address::from_string(remoteIpAddress), port);
    socket.connect(endpoint);
}

void FileTransferClient::close() {
    socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
	socket.close();
}

void FileTransferClient::receiveFileList() {
	boost::asio::write(socket, boost::asio::buffer(REQUEST + "\r\n"));

	std::istringstream fileListString(readFileListString());
	std::string line;
	while (std::getline(fileListString, line)) {
		if (line.back() == '\r') line.pop_back();
		if (line.empty()) break;
		fileList.push_back(FileInfo::parse(line));
	}
}


std::vector<FileInfo> FileTransferClient::getFileList() {
	return fileList;
}

void FileTransferClient::receiveFiles() {
	boost::filesystem::path dir(receivingDir);
	if (!boost::filesystem::exists(dir))
		boost::filesystem::create_directories(dir);

	for (int i = 0; i < fileList.size(); ++i) {
		if (!fileList[i].isChecked()) continue;

		boost::asio::write(socket, boost::asio::buffer(std::to_string(i) + "\r\n"));
		receiveFile(fileList[i]);
	}
}

void FileTransferClient::receiveFile(FileInfo& fileInfo) {
	char buf[1024 * 3];
	std::ofstream ofs(receivingDir + fileInfo.getName(), std::ofstream::binary);
	while (fileInfo.getSoFar() < fileInfo.getSize()) {
		long long readSize = socket.read_some(boost::asio::buffer(buf));
		ofs.write(buf, readSize);
		fileInfo.addSoFar(readSize);
	}
	ofs.close();
}

void FileTransferClient::sendFileList() {
	for (const FileInfo& fileInfo : fileList) {
		boost::asio::write(socket, boost::asio::buffer(fileInfo.getInfo() + "\r\n"));
	}
}


std::string FileTransferClient::readFileListString() {
	boost::asio::streambuf buf;
	std::istream is(&buf);
	boost::asio::read_until(socket, buf, "\r\n\r\n");
	return std::string((std::istreambuf_iterator<char>(&buf)), 
		std::istreambuf_iterator<char>());
}

void FileTransferClient::sendFiles() {
	std::string line;
	while ((line = readLine()) != "") {
		int index = stoi(line);
		sendFile(fileList[index]);
	}
}

void FileTransferClient::sendFile(FileInfo& fileInfo) {
	std::ifstream ifs(fileInfo.getPath(), std::ofstream::binary);
	boost::asio::streambuf buf;
	std::ostream os(&buf);
	os << ifs.rdbuf();
	boost::asio::write(socket, buf);
}

std::string FileTransferClient::readLine() {
	boost::asio::streambuf buf;
	std::istream is(&buf);
	boost::asio::read_until(socket, buf, "\r\n");

	std::string line;
	std::getline(is, line);
	line.pop_back(); // last charactor should be \r
	return line;
}

void FileTransferClient::setReceivingDir(const std::string& path) {
	receivingDir = path + "/";
}
