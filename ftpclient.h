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
    void sendCommand(QTcpSocket &socket, const QString &command);
    int FTPConnect();
    QString receiveResponse(QTcpSocket &socket);
    QTcpSocket* openDataConnection(QTcpSocket &socket);
    void downloadFile(QTcpSocket &controlSocket, const QString localFilePath, const QString remoteFileName);
    void uploadFile(QTcpSocket &controlSocket, const QString localFilePath, const QString remoteFileName);
    QStringList ListDir(QTcpSocket &controlSocket, QString tempDir);


private:
    QString server_addr;
    QString user, pass;
    int data_port = 0;
    QString data_ip;
    QString cur_status = QString();
    QString cur_mode = "IDLE";
    QTcpSocket* command_socket = new QTcpSocket();
    QTcpSocket* data_socket = new QTcpSocket();
};

#endif // FTPCLIENT_H
