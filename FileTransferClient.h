#ifndef FILETRANSFERCLIENT_H_
#define FILETRANSFERCLIENT_H_

#include <boost/asio.hpp>
#include <vector>
#include "FileInfo.h"

class FileTransferClient {
public:
    const std::string DEFAULT_REMOTE_IP_ADDRESS = "192.168.49.1";
    const int DEFAULT_PORT = 55555;
    const std::string DEFAULT_RECEIVING_DIR = "./ReceivedFiles/";

    FileTransferClient()
        : receivingDir(DEFAULT_RECEIVING_DIR)
		, socket(io_service)
	{}

    void connect(const std::string& remoteIpAddress, const int port);
	void close();
	void receiveFileList();
	void receiveFiles();
	void sendFileList();
	void sendFiles();
	void setReceivingDir(const std::string& path);
	std::vector<FileInfo> getFileList();

private:
	const std::string REQUEST = "Request File List";

	std::string receivingDir;
	boost::asio::io_service io_service;
	boost::asio::ip::tcp::socket socket;
	std::vector<FileInfo> fileList;

	void receiveFile(FileInfo& fileInfo);
	std::string readFileListString();
	void sendFile(FileInfo& fileInfo);
	std::string readLine();
};


#endif // FILETRANSFERCLIENT_H_
