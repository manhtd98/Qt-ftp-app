#include "ftpclient.h"
#include <QElapsedTimer>
#include <QFile>
#include <QRegExp>
#include <QTcpSocket>

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
        return -1;
    }

    qDebug() << "Connected to server. Receiving welcome message...";
    receiveResponse(controlSocket);

    qDebug() << "Sending username...";
    sendCommand(controlSocket, "USER " + username + "\r\n");
    QString response = receiveResponse(controlSocket);
    if (response.startsWith("220")) {
        qDebug() << receiveResponse(controlSocket);
    }
    qDebug() << "Sending password...";
    sendCommand(controlSocket, "PASS " + password + "\r\n");
    QString loginResponse = receiveResponse(controlSocket);
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

QString ftpClient::receiveResponse(QTcpSocket &socket)
{
    while (!socket.canReadLine()) {
        socket.waitForReadyRead();
    }
    QString response = QString::fromUtf8(socket.readLine());
    qDebug() << response;
    return response;
}

QPair<QStringList, QStringList> ftpClient::ListDir(QString tempDir)
{
    QElapsedTimer timer;
    timer.start();
    QTcpSocket * dataSocket = openDataConnection();
    if (!dataSocket) {
        return qMakePair(QStringList(), QStringList());
    }

    qDebug() << "Sending LIST command..." << tempDir;
    sendCommand(controlSocket, "LIST "+ tempDir + "\r\n");
    receiveResponse(controlSocket);

    QString dirListing;
    qDebug() << "Receiving directory listing...";
    while (dataSocket->waitForReadyRead()) {
        dirListing.append(dataSocket->readAll());
    }
    dataSocket->close();
    delete dataSocket;

    static QRegExp regex(
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
    qDebug() << "Function took" << timer.elapsed() << "ms";
    return qMakePair(fileList, isDirList);
}

QTcpSocket* ftpClient::openDataConnection()
{
    QElapsedTimer timer;
    timer.start();
    qDebug()<<"Entering PASV mode";
    sendCommand(controlSocket, "PASV\r\n");
    QString pasvResponse = receiveResponse(controlSocket);

    if (pasvResponse.startsWith("226")) {
        pasvResponse = receiveResponse(controlSocket);
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

    QString ip = QString("%1.%2.%3.%4").arg(addressParts[0], addressParts[1], addressParts[2], addressParts[3]);
    int port = addressParts[4].toInt() * 256 + addressParts[5].toInt();

    QTcpSocket *dataSocket = new QTcpSocket();
    qDebug() << "Connecting to data IP:" << ip << "Port:" << port;
    dataSocket->connectToHost(ip, port);
    if (!dataSocket->waitForConnected()) {
        qWarning() << "Error: Unable to connect to data server";
        delete dataSocket;
        return nullptr;
    }
    qDebug() << "Function took" << timer.elapsed() << "ms";
    return dataSocket;
}
int ftpClient::addDir(QString tempDir)
{
    QElapsedTimer timer;
    timer.start();
    qDebug() << "MKD" << tempDir;
    sendCommand(controlSocket, "MKD " + tempDir + "\r\n");
    QString response = receiveResponse(controlSocket);
    qDebug() << "Function took" << timer.elapsed() << "ms";
    if (response.startsWith("550")) {
        return -1;
    };
    if (!response.startsWith("25")) {
        response = receiveResponse(controlSocket);
        if (!response.startsWith("2")) {
            return -1;
        }
    }
    return 1;
}
int ftpClient::removeDir(QString dirPath)
{
    QElapsedTimer timer;
    timer.start();
    qDebug() << "RMD" << dirPath;
    sendCommand(controlSocket, "RMD " + dirPath + "\r\n");
    QString response = receiveResponse(controlSocket);
    qDebug() << "Function took" << timer.elapsed() << "ms";
    if (response.startsWith("550")) {
        return -1;
    };
    if (!response.startsWith("25")) {
        response = receiveResponse(controlSocket);
        qDebug() << response;
        if (!response.startsWith("2")) {
            return -1;
        }
    }
    return 1;
}
int ftpClient::removeDirRecursive(QString dirPath)
{
    QElapsedTimer timer;
    timer.start();
    qDebug() << "Removing directory recursively:" << dirPath;

    // List all files and directories inside the directory
    QPair<QStringList, QStringList> dirContents = ListDir(dirPath);
    QStringList fileList = dirContents.first;
    QStringList isDirList = dirContents.second;
    // Remove all files
    // for (int i = 0; i < fileList.size(); ++i) {
    //     QString item = fileList[i];
    //     if (isDirList[i] == 'd') {
    //         // It's a directory, call removeDirRecursive
    //         removeDirRecursive(dirPath + "/" + item);
    //     } else {
    //         // It's a file, delete it
    //         removeFile(dirPath + "/" + item);
    //     }
    // }
    qDebug() << "Function took" << timer.elapsed() << "ms";
    if (fileList.size() > 0) {
        return -1;
    }

    return removeDir(dirPath);
}

int ftpClient::removeFile(QString filePath)
{
    QElapsedTimer timer;
    timer.start();
    qDebug() << "DELE" << filePath;
    sendCommand(controlSocket, "DELE " + filePath + "\r\n");
    QString response = receiveResponse(controlSocket);
    qDebug() << "Function took" << timer.elapsed() << "ms";
    if (response.startsWith("550")) {
        return -1;
    };

    if (!response.startsWith("25")) {
        response = receiveResponse(controlSocket);
        if (!response.startsWith("25")) {
            return -1;
        }
    }
    return 1;
}

int ftpClient::downloadFile(const QString localFilePath, const QString remoteFilename)
{
    QElapsedTimer timer;
    timer.start();
    QTcpSocket *dataSocket = openDataConnection();
    if (!dataSocket) {
        return -1;
    }

    qDebug() << "Sending RETR command for file" << remoteFilename << "to" << localFilePath;
    sendCommand(controlSocket, "RETR " + remoteFilename + "\r\n");
    receiveResponse(controlSocket);

    qDebug() << "Receiving file...";
    QFile outFile(localFilePath);
    outFile.open(QIODevice::WriteOnly);
    while (dataSocket->waitForReadyRead()) {
        outFile.write(dataSocket->readAll());
    }
    outFile.close();

    receiveResponse(controlSocket);

    dataSocket->close();
    delete dataSocket;
    qDebug() << "Function took" << timer.elapsed() << "ms";
    return 1;
}

int ftpClient::uploadFile(const QString localFilePath, const QString remoteFileName)
{
    QElapsedTimer timer;
    timer.start();
    QTcpSocket *dataSocket = openDataConnection();
    if (!dataSocket) {
        return -1;
    }

    qDebug() << "Upload file start...";
    qDebug() << "Sending STOR command for file" << localFilePath  << "to" << remoteFileName;
    sendCommand(controlSocket, "STOR " + remoteFileName + "\r\n");
    QString response = receiveResponse(controlSocket);
    if (!response.startsWith("150")) {
        qDebug() << "Error: Unexpected response to STOR command";
        dataSocket->close();
        delete dataSocket;
        return -1;
    }
    if (dataSocket->state() != QAbstractSocket::ConnectedState) {
        qDebug() << "Error: Data socket not connected";
        dataSocket->close();
        delete dataSocket;
        return -1;
    }

    qDebug() << "Uploading file...";
    QFile inFile(localFilePath);
    if (!inFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Error: Unable to open file for reading";
        dataSocket->close();
        delete dataSocket;
        return -1;
    }
    while (!inFile.atEnd()) {
        QByteArray buffer = inFile.read(1024);
        qint64 bytesWritten = dataSocket->write(buffer);
        if (bytesWritten == -1) {
            qWarning() << "Error: Write to data socket failed";
            break;
        }
    }
    dataSocket->waitForBytesWritten();
    inFile.close();
    dataSocket->close();
    delete dataSocket;
    qDebug() << "Function took" << timer.elapsed() << "ms";
    return 1;
}
