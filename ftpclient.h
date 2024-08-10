#ifndef FTPCLIENT_H
#define FTPCLIENT_H
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <QTcpSocket>


#define BUFFER_SIZE     8192


class ftpClient
{
public:
    ftpClient();
    ~ftpClient();
    void sendCommand(QTcpSocket &socket, const QString &command);
    int FTPConnect(QString serverIp,  int port, QString username, QString password );
    QString receiveResponse(QTcpSocket &socket);
    QTcpSocket* openDataConnection();
    int downloadFile(const QString localFilePath, const QString remoteFileName);
    int uploadFile(const QString localFilePath, const QString remoteFileName);
    QPair<QStringList, QStringList> ListDir(QString tempDir);
    int addDir(QString tempDir);
    int removeDir(QString dirPath);
    int removeFile(QString filePath);
    int removeDirRecursive(QString dirPath);

private:
    QString server_addr;
    QString user, pass;
    int server_port = 21;
    QTcpSocket controlSocket;
};

#endif // FTPCLIENT_H
