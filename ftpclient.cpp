#include "ftpclient.h"
#include <QTcpSocket>
#include <QFile>
#include <QRegExp>

ftpClient::ftpClient() {

}

int ftpClient::FTPConnect(){

    QString serverIp = "127.0.0.1";
    int port = 21;
    QString username = "macbook";
    QString password = "11235";

    QTcpSocket controlSocket;
    controlSocket.connectToHost(serverIp, port);
    if (!controlSocket.waitForConnected()) {
        qWarning() << "Error: Unable to connect to server";
        return 1;
    }

    qDebug() << "Connected to server. Receiving welcome message...";
    qDebug() << receiveResponse(controlSocket);

    qDebug() << "Sending username...";
    sendCommand(controlSocket, "USER " + username + "\r\n");
    qDebug() << receiveResponse(controlSocket);

    qDebug() << "Sending password...";

    qDebug() << receiveResponse(controlSocket);
    sendCommand(controlSocket, "PASS " + password + "\r\n");
    qDebug() << receiveResponse(controlSocket);

    ListDir(controlSocket, "/Users/macbook/main/");
    // Upload a file
    uploadFile(controlSocket, "/Users/macbook/Desktop/3.txt", "/Users/macbook/main/1.txt");

    // Download a file
    downloadFile(controlSocket, "/Users/macbook/Desktop/4.txt", "/Users/macbook/main/1.txt");



    controlSocket.close();
}
void ftpClient::sendCommand(QTcpSocket &socket, const QString &command)
{
    socket.write(command.toUtf8());
    socket.flush();
}

QString ftpClient::receiveResponse(QTcpSocket &socket) {
    while (!socket.canReadLine()) {
        socket.waitForReadyRead();
    }
    return QString::fromUtf8(socket.readLine());
}

QStringList ftpClient::ListDir(QTcpSocket& controlSocket, QString tempDir)
{
    QTcpSocket * dataSocket = openDataConnection(controlSocket);
    if (!dataSocket) {
        return QStringList();
    }

    qDebug() << "Sending LIST command...";
    sendCommand(controlSocket, "LIST "+ tempDir + "\r\n");
    qDebug() << receiveResponse(controlSocket);

    QString dirListing;
    qDebug() << "Receiving directory listing...";
    while (dataSocket->waitForReadyRead()) {
        dirListing.append(dataSocket->readAll());
    }
    dataSocket->close();
    delete dataSocket;

    QRegExp regex("^(\\S+)\\s+(\\d+)\\s+(\\S+)\\s+(\\S+)\\s+(\\d+)\\s+(\\S+\\s+\\S+\\s+\\S+)\\s+(.+)$");
    regex.setPatternSyntax(QRegExp::RegExp);


    QList<QString> fileList;
    QStringList lines = dirListing.split("\n", QString::SkipEmptyParts);
    foreach (const QString &line, lines) {
        if (regex.indexIn(line) != -1) {
            // QString permissions = regex.cap(1);
            // QString fileSize = regex.cap(5);
            QString fileName = regex.cap(7);
            fileList.append(fileName);
        }
    }

    qDebug()<<fileList;
    return fileList;
}

QTcpSocket* ftpClient::openDataConnection(QTcpSocket &controlSocket)
{
    qDebug()<<"Entering PASV mode";
    sendCommand(controlSocket, "PASV\r\n");
    QString pasvResponse = receiveResponse(controlSocket);

    if(pasvResponse.startsWith("226")){
        pasvResponse = receiveResponse(controlSocket);
        qDebug() << pasvResponse;
    }else{
        qDebug() << pasvResponse;
    }
    int start = pasvResponse.indexOf('(');
    int end = pasvResponse.indexOf(')');
    if (start == -1 || end == -1 || start >= end) {
        qWarning() << "Error: Invalid PASV response format";
        return nullptr;
    }

    QStringList addressParts = pasvResponse.mid(start + 1, end - start - 1).split(',');
    if (addressParts.size() != 6) {
        qWarning() << "Error: Invalid PASV response format";
        return nullptr;
    }

    QString ip = QString("%1.%2.%3.%4").arg(addressParts[0]).arg(addressParts[1]).arg(addressParts[2]).arg(addressParts[3]);
    int port = addressParts[4].toInt() * 256 + addressParts[5].toInt();

    QTcpSocket *dataSocket = new QTcpSocket();
    qDebug() << "Connecting to data IP:" << ip << "Port:" << port;
    dataSocket->connectToHost(ip, port);
    if (!dataSocket->waitForConnected()) {
        qWarning() << "Error: Unable to connect to data server";
        delete dataSocket;
        return nullptr;
    }
    return dataSocket;
}

void ftpClient::downloadFile(QTcpSocket &controlSocket, const QString localFilePath, const QString remoteFilename) {
    QTcpSocket *dataSocket = openDataConnection(controlSocket);
    if (!dataSocket) {
        return;
    }

    qDebug() << "Sending RETR command for file" << remoteFilename << "to" << localFilePath;
    sendCommand(controlSocket, "RETR " + remoteFilename + "\r\n");
    qDebug() << receiveResponse(controlSocket);

    qDebug() << "Receiving file...";
    QFile outFile(localFilePath);
    outFile.open(QIODevice::WriteOnly);
    while (dataSocket->waitForReadyRead()) {
        outFile.write(dataSocket->readAll());
    }
    outFile.close();

    qDebug() << receiveResponse(controlSocket);

    dataSocket->close();
    delete dataSocket;
}

void ftpClient::uploadFile(QTcpSocket &controlSocket,  const QString localFilePath, const QString remoteFileName)
{
    QTcpSocket *dataSocket = openDataConnection(controlSocket);
    if (!dataSocket) {
        return ;
    }

    qDebug() << "Upload file start...";
    qDebug() << "Sending STOR command for file" << localFilePath  << "to" << remoteFileName;
    sendCommand(controlSocket, "STOR " + remoteFileName + "\r\n");
    QString response = receiveResponse(controlSocket);
    qDebug() << response;
    if (!response.startsWith("150")) {
        qDebug() << "Error: Unexpected response to STOR command";
        dataSocket->close();
        delete dataSocket;
        return;
    }
    if (dataSocket->state() != QAbstractSocket::ConnectedState) {
        qDebug() << "Error: Data socket not connected";
        dataSocket->close();
        delete dataSocket;
        return;
    }

    qDebug() << "Uploading file...";
    QFile inFile(localFilePath);
    if (!inFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Error: Unable to open file for reading";
        dataSocket->close();
        delete dataSocket;
        return;
    }
    while (!inFile.atEnd()) {
        QByteArray buffer = inFile.read(1024);
        qint64 bytesWritten = dataSocket->write(buffer);
        if (bytesWritten == -1) {
            qWarning() << "Error: Write to data socket failed";
            break;
        }

        if (!dataSocket->waitForBytesWritten(5000)) { // Added timeout
            qWarning() << "Error: Timeout while waiting for data to be written";
            break;
        }
    }
    dataSocket->waitForBytesWritten();
    inFile.close();

    qDebug() << receiveResponse(controlSocket);

    dataSocket->close();
    delete dataSocket;
}
