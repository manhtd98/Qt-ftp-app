#include "ftpclient.h"
#include <QTcpSocket>
#include <QFile>
#include <QRegExp>

ftpClient::ftpClient() {
    
}
ftpClient::~ftpClient()
{
    controlSocket.close();
};

int ftpClient::FTPConnect(QString serverIp,  int port, QString username, QString password ){
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
    QString loginResponse = receiveResponse(controlSocket);
    qDebug()<< loginResponse;
    if(loginResponse.contains("230")){
        return 1;
    }
    controlSocket.close();
    return -1;
    // ListDir(controlSocket, "/Users/macbook/main/");
    // // Upload a file
    // uploadFile(controlSocket, "/Users/macbook/Desktop/3.txt", "/Users/macbook/main/5.txt");

    // // Download a file
    // downloadFile(controlSocket, "/Users/macbook/Desktop/4.txt", "/Users/macbook/main/5.txt");
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

QPair<QStringList, QStringList> ftpClient::ListDir(QString tempDir)
{
    QTcpSocket * dataSocket = openDataConnection();
    if (!dataSocket) {
        return qMakePair(QStringList(), QStringList());
    }

    qDebug() << "Sending LIST command..." << tempDir;
    sendCommand(controlSocket, "LIST "+ tempDir + "\r\n");
    qDebug() << receiveResponse(controlSocket);

    QString dirListing;
    qDebug() << "Receiving directory listing...";
    while (dataSocket->waitForReadyRead()) {
        dirListing.append(dataSocket->readAll());
    }
    dataSocket->close();
    delete dataSocket;

    QRegExp regex(
        "^(\\S+)\\s+(\\d+)\\s+(\\S+)\\s+(\\S+)\\s+(\\d+)\\s+(\\S+\\s+\\S+\\s+\\S+)\\s+(.+)$");
    regex.setPatternSyntax(QRegExp::RegExp);

    QList<QString> fileList;
    QList<QString> isDirList;
    QStringList lines = dirListing.split("\n", QString::SkipEmptyParts);
    foreach (const QString &line, lines) {
        if (regex.indexIn(line) != -1) {
            QString fileName = regex.cap(7);
            fileName.chop(1);
            fileList.append(fileName);
            isDirList.append(line[0]);
        }
    }

    qDebug() << fileList;
    return qMakePair(fileList, isDirList);
}

QTcpSocket* ftpClient::openDataConnection()
{
    qDebug()<<"Entering PASV mode";
    sendCommand(controlSocket, "PASV\r\n");
    QString pasvResponse = receiveResponse(controlSocket);

    if (pasvResponse.startsWith("226")) {
        pasvResponse = receiveResponse(controlSocket);
        qDebug() << pasvResponse;
    } else {
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

    QString ip = QString("%1.%2.%3.%4")
                     .arg(addressParts[0])
                     .arg(addressParts[1])
                     .arg(addressParts[2])
                     .arg(addressParts[3]);
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
void ftpClient::addDir(QString tempDir)
{
    qDebug() << "MKD" << tempDir;
    sendCommand(controlSocket, "MKD " + tempDir + "\r\n");
    QString reponse = receiveResponse(controlSocket);
    if (!reponse.startsWith("25")) {
        qDebug() << receiveResponse(controlSocket);
    }
}
void ftpClient::removeDir(QString dirPath)
{
    qDebug() << "RMD" << dirPath;
    sendCommand(controlSocket, "RMD " + dirPath + "\r\n");
    QString reponse = receiveResponse(controlSocket);
    if (!reponse.startsWith("25")) {
        qDebug() << receiveResponse(controlSocket);
    }
}
void ftpClient::removeFile(QString filePath)
{
    qDebug() << "DELE" << filePath;
    sendCommand(controlSocket, "DELE " + filePath + "\r\n");
    QString reponse = receiveResponse(controlSocket);
    if (!reponse.startsWith("25")) {
        qDebug() << receiveResponse(controlSocket);
    }
}

void ftpClient::downloadFile(const QString localFilePath, const QString remoteFilename) {
    QTcpSocket *dataSocket = openDataConnection();
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

void ftpClient::uploadFile(const QString localFilePath, const QString remoteFileName)
{
    QTcpSocket *dataSocket = openDataConnection();
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
        qDebug()<<bytesWritten;
        if (bytesWritten == -1) {
            qWarning() << "Error: Write to data socket failed";
            break;
        }
    }
    dataSocket->waitForBytesWritten();
    inFile.close();
    dataSocket->close();
    delete dataSocket;
}
