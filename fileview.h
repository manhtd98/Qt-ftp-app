#ifndef FILEVIEW_H
#define FILEVIEW_H

#include <QWidget>
#include <QStringList>
#include <QtNetwork/QNetworkAccessManager>
#include <QListWidgetItem>

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
    void getFileList();
    void uploadFileListUpdate();
    void on_pushButton_3_clicked();
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::FileView *ui;
    QNetworkAccessManager* qmanager;

    QString ftpAddress;
    int ftpPort;
    QString username;
    QString password;

    QStringList fileList;
    QString uploadFileName;
    QString downloadFileName;

};

#endif // FILEVIEW_H
