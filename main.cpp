#include "mainwindow.h"

#include <QApplication>

#include "ftpclient.h"

void setupMessagePattern()
{
    qSetMessagePattern("[%{time hh:mm:ss.zzz}] [%{type}] [%{file}:%{line}] %{message}");
}

int main(int argc, char *argv[])
{
    setupMessagePattern();
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("FTP Client");
    w.show();
    return a.exec();
    // ftpClient ftp;
    // ftp.FTPConnect("127.0.0.1", 21, "macbook","11235");
}
