#include <iostream>
#include <fstream>
#include "FileTransferClient.h"
#include "dialog.h"

using namespace std;
using namespace boost;

const string FileTransferClient::DEFAULT_REMOTE_IP_ADDRESS = "192.168.49.1";
const string FileTransferClient::DEFAULT_PORT = "55555";
const string FileTransferClient::DEFAULT_RECEIVE_DIR = "./files";

FileTransferClient::FileTransferClient(Dialog* dialog)
    : socket(ioService)
    , dialog(dialog)
{
    work.reset(new asio::io_service::work(ioService));
}

void FileTransferClient::close() {
    socket.shutdown(asio::ip::tcp::socket::shutdown_both);
	socket.close();
}

void FileTransferClient::connect(const string& remoteIpAddress, const int port) {
    workThread.reset(new std::thread([this] {
        try {
            ioService.run();
        } catch(const std::exception&) {
            dialog->setStatus("ERROR - Connection failed.");
        }

    }));

    dialog->setStatus("Connecting...");

    asio::ip::tcp::endpoint ep(asio::ip::address::from_string(remoteIpAddress), port);
    socket.open(ep.protocol());
	socket.async_connect(ep, [this](const system::error_code& error) {
		if (error) {
            dialog->setStatus("ERROR - Connection failed.");
			cout << "Error" << endl;
			return;
		}

        dialog->setStatus("Connected");
        sendFileListRequest();
	});
}

void FileTransferClient::sendFileListRequest() {
    dialog->setStatus("Sending file list request..");
    asio::async_write(socket, asio::buffer(REQUEST),
        [this](const system::error_code& error, const size_t&) {
		if (error) {
            dialog->setStatus("ERROR - Connection failed.");
            cout << "Error: Could not send File Request." << endl;
			return;
		}
		
        dialog->setStatus("Sent the file list request");
        receiveFileList();
	});
}

void FileTransferClient::receiveFileList() {
    dialog->setStatus("Receiving the file list...");
    asio::async_read_until(socket, responseBuf, "\r\n\r\n",
        [this](const system::error_code& error, const size_t&) {
		if (error) {
            dialog->setStatus("ERROR - Connection failed.");
            cout << "Error: Could not receive File List." << endl;
			return;
		}

		istream is(&responseBuf);
        string line, fileName, fileSizeString, fileType;
		while (getline(is, line, '\r')) {
			is.get();
            if (line.empty()) break;

            istringstream iss(line);
            iss >> fileName >> fileSizeString >> fileType;

            long long fileSize = parseLongLong(fileSizeString);
			if (fileSize == -1) {
				cout << "Error: Could not parse file size.";
				return;
			}
			
			fileList.push_back({ fileName, fileSize });
		}
        dialog->setStatus("Received the file list");
        dialog->setFileList(fileList);
    });
}

long long FileTransferClient::parseLongLong(const string& stringNum) {
	try {
		return stoll(stringNum);
	} catch (const invalid_argument&) {
		return -1;
	} catch(const out_of_range&) {
		return -1;
	}
}

void FileTransferClient::receiveFiles(const string& receiveDir) {
    this->receiveDir = receiveDir;
    filesystem::path dir(receiveDir);
    if (!filesystem::exists(dir))
        filesystem::create_directories(dir);

    receiveFileThread.reset(new std::thread(&FileTransferClient::receiveFilesHelper, this));
}

void FileTransferClient::receiveFilesHelper() {

    for (unsigned int i = 0; i < fileList.size(); ++i) {
		if (!fileList[i].isChecked()) continue;

        asio::write(socket, asio::buffer(std::to_string(i) + "\r\n"));
        receiveFile(&fileList[i]);
	}
    dialog->setStatus("Done");
}

void FileTransferClient::receiveFile(FileInfo* fileInfo) {
    ofstream ofs(receiveDir + "/" + fileInfo->getName(), ofstream::binary);
	if (!ofs.is_open()) {
		cout << "Error: Could not open " + receiveDir + fileInfo->getName() << endl;
	}


    dialog->setStatus("Receiving " + fileInfo->getName() + "...");
    while (fileInfo->getSoFar() < fileInfo->getSize()) {
		long long readSize = socket.read_some(asio::buffer(buf), error);
		if (error) {
			cout << "Error: Failed to read " + fileInfo->getName() << endl;
			return;
		}

		ofs.write(buf, readSize);
		fileInfo->addSoFar(readSize);
        changeProgress(fileInfo->getSoFar(), fileInfo->getSize());
	}
	ofs.close();
}
