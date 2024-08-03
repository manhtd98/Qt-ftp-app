#include "mainwindow.h"

#include <QApplication>

#include "ftpclient.h"

int main(int argc, char *argv[])
{
    // QApplication a(argc, argv);
    // MainWindow w;
    // w.setWindowTitle("FTP Client");
    // w.show();
    ftpClient ftp;
    ftp.FTPConnect();
    // return a.exec();
}
