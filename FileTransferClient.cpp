#include <iostream>
#include <fstream>
#include "FileTransferClient.h"
#include "dialog.h"

using namespace std;
using namespace boost;

const string FileTransferClient::DEFAULT_REMOTE_IP_ADDRESS = "192.168.49.1";
const string FileTransferClient::DEFAULT_PORT = "55555";
const string FileTransferClient::DEFAULT_RECEIVE_DIR = "./files";

const string FileTransferClient::NO_CONNECTION = "No connection";
const string FileTransferClient::CONNECTING = "Connecting..";
const string FileTransferClient::SENDING_FILE_LIST_REQUEST = "Sending the file list request...";
const string FileTransferClient::WAITING_FOR_FILE_LIST = "Waiting for the file list...";
const string FileTransferClient::RECEIVED_FILE_LIST = "Received the file list";
const string FileTransferClient::RECEIVING_FILES = "Receiving the file(s)...";
const string FileTransferClient::DONE = "Done";
const string FileTransferClient::CANCELED = "Canceled";
const string FileTransferClient::CONNECTION_FAILED = "Connection failed";

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
            emit changeStatus(CONNECTION_FAILED);
        }
    }));

    emit changeStatus(CONNECTING);
    asio::ip::tcp::endpoint ep(asio::ip::address::from_string(remoteIpAddress), port);
    socket.async_connect(ep, [this](const system::error_code& error) {
        if (error) {
            emit changeStatus(CONNECTION_FAILED);
            close();
            return;
		}
        sendFileListRequest();
	});
}

void FileTransferClient::sendFileListRequest() {
    emit changeStatus(SENDING_FILE_LIST_REQUEST);
    asio::async_write(socket, asio::buffer(REQUEST),
        [this](const system::error_code& error, const size_t&) {
		if (error) {
            emit changeStatus(CONNECTION_FAILED);
            close();
            return;
		}
        receiveFileList();
	});
}

void FileTransferClient::receiveFileList() {
    emit changeStatus(WAITING_FOR_FILE_LIST);
    asio::async_read_until(socket, responseBuf, "\r\n\r\n",
        [this](const system::error_code& error, const size_t&) {
		if (error) {
            emit changeStatus(CONNECTION_FAILED);
            close();
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
                emit changeStatus(CONNECTION_FAILED);
                cout << "Error: Could not parse file size.";
                close();
                return;
			}
			
			fileList.push_back({ fileName, fileSize });
		}
        dialog->setFileList(fileList);
        emit changeStatus(RECEIVED_FILE_LIST);
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
    emit changeStatus(RECEIVING_FILES);
    for (unsigned int i = 0; i < fileList.size(); ++i) {
		if (!fileList[i].isChecked()) continue;

        asio::write(socket, asio::buffer(std::to_string(i) + "\r\n"));
        receiveFile(&fileList[i]);
	}
    emit changeStatus(DONE);
}

void FileTransferClient::receiveFile(FileInfo* fileInfo) {
    ofstream ofs(receiveDir + "/" + fileInfo->getName(), ofstream::binary);
	if (!ofs.is_open()) {
		cout << "Error: Could not open " + receiveDir + fileInfo->getName() << endl;
        return;
	}

    dialog->setReceiveFileName(fileInfo->getName());
    while (fileInfo->getSoFar() < fileInfo->getSize()) {
        long long readSize = socket.read_some(asio::buffer(buf), error);
		if (error) {
            emit changeStatus(CONNECTION_FAILED);
            close();
            return;
		}

		ofs.write(buf, readSize);
		fileInfo->addSoFar(readSize);
        emit changeProgress(fileInfo->getSoFar(), fileInfo->getSize());
	}
	ofs.close();
}
