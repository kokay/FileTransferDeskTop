#ifndef FILETRANSFERCLIENT_H_
#define FILETRANSFERCLIENT_H_

#include <QObject>
#include <vector>
#include <thread>
#include <boost/asio.hpp>
#include "FileInfo.h"

using namespace std;
using namespace boost;

class Dialog;
class FileTransferClient : public QObject {

    Q_OBJECT

public:
    static const string DEFAULT_REMOTE_IP_ADDRESS;
    static const string DEFAULT_PORT;
    static const string DEFAULT_RECEIVE_DIR;

    static const string NO_CONNECTION;
    static const string CONNECTING;
    static const string SENDING_FILE_LIST_REQUEST;
    static const string WAITING_FOR_FILE_LIST;
    static const string RECEIVED_FILE_LIST;
    static const string RECEIVING_FILES;
    static const string DONE;
    static const string CANCELED;
    static const string CONNECTION_FAILED;

    FileTransferClient(Dialog* dialog);
	const vector<FileInfo>& getFileList() const { return fileList; };

	void connect(const std::string& remoteIpAddress, const int port);
	void close();
    void receiveFiles(const string& receiveDir);
	void receiveFileList();

signals:
    void changeProgress(const long long, const long long);
    void changeStatus(const string&);

private:
	void sendFileListRequest();
	void receiveFilesHelper();
	void receiveFile(FileInfo* fileInfo);
	long long parseLongLong(const string& stringNum);

	asio::io_service ioService;
	asio::ip::tcp::socket socket;
    unique_ptr<asio::io_service::work> work;
    unique_ptr<std::thread> workThread;
    unique_ptr<std::thread> receiveFileThread;

	asio::streambuf responseBuf;
	vector<FileInfo> fileList;

	char buf[1024 * 3];
	system::error_code error;
	
    string receiveDir;
    Dialog* dialog;

    bool canceled = false;

    const string REQUEST = "Request File List\r\n";
};


#endif //FILETRANSFERCLIENT_H_
