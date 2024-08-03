#ifndef FILEVIEW_H
#define FILEVIEW_H

#include <QWidget>
#include <QStringList>
#include <QtNetwork/QNetworkAccessManager>
#include <QListWidgetItem>
#include "ftpclient.h"

namespace Ui {
class FileView;
}

class FileView : public QWidget
{
    Q_OBJECT

public:
    explicit FileView(QString ftpHost=QString(""), QString ftpUser=QString(""), QString ftpPass=QString(""), int ftpport=21, QWidget *parent = nullptr);
    ~FileView();

private slots:
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
    void on_pushButton_3_clicked();
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);
    void getFileList();

private:
    Ui::FileView *ui;
    QString ftpAddress;
    int ftpPort;
    QString username;
    QString password;
    ftpClient FtpClient;
    QStringList fileList;
    QString dirPath = QString();


};

#endif // FILEVIEW_H
