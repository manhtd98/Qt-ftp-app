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
    explicit FileView(QString ftpHost = QString(),
                      QString ftpUser = QString(),
                      QString ftpPass = QString(),
                      int ftpport = 21,
                      QWidget *parent = nullptr);
    ~FileView();
signals:
    void returnToMainWindow();
private slots:
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);
    void getFileList();
    void on_addDir_clicked();
    void on_removeItem_clicked();
    void on_disconnectButton_clicked();

private:
    Ui::FileView *ui;
    QString ftpAddress;
    int ftpPort;
    QString username;
    QString password;
    ftpClient FtpClient;
    QStringList fileList;
    QString dirPath = QString();
    QIcon fileIcon;
    QIcon dirIcon;
    QColor customDirColor;
};

#endif // FILEVIEW_H
