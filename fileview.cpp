#include "fileview.h"
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QNetworkReply>
#include <QtGui>
#include <QtNetwork/QNetworkAccessManager>
#include "ui_fileview.h"

FileView::FileView(QString ftpHost, QString ftpUser, QString ftpPass, int ftpport, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FileView)
{
    ftpAddress = ftpHost;
    ftpPort = ftpport;
    username = ftpUser;
    password = ftpPass;
    QIcon fileIcon_item(":/icons/file.png");
    QIcon dirIcon_tem(":/icons/folder.png");
    QColor customDirColor_item(100, 130, 173);
    fileIcon = fileIcon_item;
    customDirColor = customDirColor_item;
    dirIcon = dirIcon_tem;
    ui->setupUi(this);
    ui->listWidget->clear();

    qDebug() << "Connecting to" << ftpAddress << ftpPort << username << password;
    int response = FtpClient.FTPConnect(ftpAddress, ftpPort, username, password);
    if (response != -1) {
        getFileList();
    }
}

FileView::~FileView()
{
    delete ui;
}

void FileView::on_pushButton_2_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Upload file", NULL);
    ui->lineEdit->setText(filename);
    // Choost upload file path
    QFile *file = new QFile(ui->lineEdit->text());
    qDebug() << "Open file:" << ui->lineEdit->text();
    QFileInfo fileInfo(*file);
    QString uploadFileName = fileInfo.fileName();
    if (file->open(QIODevice::ReadOnly)) {
        FtpClient.uploadFile(fileInfo.filePath(), dirPath + "/" + uploadFileName);
        getFileList();
    }
}

void FileView::getFileList()
{
    // Update the file list window
    QPair<QStringList, QStringList> res = FtpClient.ListDir(dirPath);
    QStringList fileList=res.first;
    QStringList isDirList = res.second;
    ui->listWidget->clear();
    if (dirPath.split("/").size() > 1) {
        // Add the ... to return previous folder
        QListWidgetItem *item = new QListWidgetItem();
        item->setText("...");
        item->setSizeHint(QSize(ui->listWidget->width(), 30));
        item->setTextAlignment(Qt::AlignLeft);
        item->setTextAlignment(Qt::AlignVCenter);
        ui->listWidget->addItem(item);
    }

    if (fileList.size() > 0) {
        for (int i = 0; i < fileList.size(); i++)
        {
            if (fileList.at(i) != "")
            {
                // Add new item to list view
                QListWidgetItem *item = new QListWidgetItem();

                item->setForeground(Qt::white);
                item->setTextAlignment(Qt::AlignLeft);
                item->setData(Qt::UserRole, QVariant(isDirList.at(i)));
                item->setSizeHint(QSize(ui->listWidget->width(), 30));
                item->setText(fileList.at(i));
                if (isDirList.at(i) == "d") {
                    item->setIcon(dirIcon);
                    item->setBackground(customDirColor);
                } else {
                    item->setIcon(fileIcon);
                }
                item->setTextAlignment(Qt::AlignVCenter);
                ui->listWidget->addItem(item);
            }
        }
    }
}


void FileView::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    if (item->text() == "...") {
        // go to previous folder
        if (dirPath.split("/").size() > 1) {
            QStringList item = dirPath.split("/");
            item.removeLast();
            dirPath = item.join("/");
            getFileList();
        }
    } else {
        if(item->data(Qt::UserRole)=="d"){
            // go to the folder
            dirPath += "/"+item->text();
            getFileList();
        }else{
            // Download file
            QString downloadPath = ui->lineEdit_2->text();
            if (downloadPath.isEmpty()){
                QMessageBox::information(
                    nullptr,
                    "Download folder",
                    "Please choose download folder"
                    );
            }else{
                QString downloadfilePath = dirPath + "/" + item->text();
                FtpClient.downloadFile(downloadPath+ "/" + item->text(), downloadfilePath);
                QMessageBox::information(
                    nullptr,
                    "Success download",
                    "Success download file"
                    );
            }
        }
    }
}


void FileView::on_pushButton_3_clicked()
{
    QString directory = QFileDialog::getExistingDirectory(
        nullptr,
        "Choose download",
        "",
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
        );

    if (!directory.isEmpty()) {
        qDebug() << "Selected directory:" << directory;
    } else {
        qDebug() << "No directory selected";
    }
    ui->lineEdit_2->setText(directory);
}

void FileView::on_addDir_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this,
                                         tr("Type directory name"),
                                         tr("Diretory name:"),
                                         QLineEdit::Normal,
                                         "",
                                         &ok);
    if (ok && !text.isEmpty()) {
        int response = FtpClient.addDir(dirPath + "/" + text);
        if (response == -1) {
            QMessageBox::information(nullptr,
                                     "Add directory failed",
                                     "Permission Denied or Not valid name");
        }
    }
    getFileList();
}

void FileView::on_removeItem_clicked()
{
    QList<QListWidgetItem *> selectedItem = ui->listWidget->selectedItems();
    if (selectedItem.size() > 0) {
        for (int i = 0; i < selectedItem.size(); i++) {
            if (selectedItem.at(i)->data(Qt::UserRole) == "d") {
                int removeCode = FtpClient.removeDirRecursive(dirPath + "/"
                                                              + selectedItem.at(i)->text());
                if (removeCode == -1) {
                    QMessageBox::information(nullptr,
                                             "Remove folder failed",
                                             "Permission Denied or Directory not empty");
                }

            } else {
                int removeCode = FtpClient.removeFile(dirPath + "/" + selectedItem.at(i)->text());
                if (removeCode == -1) {
                    QMessageBox::information(nullptr, "Remove file failed", "Permission Denied");
                }
            }
        }
        getFileList();
    } else {
        QMessageBox::information(nullptr,
                                 "Remove file or folder",
                                 "Please choose remove file/folder");
    }
}

void FileView::on_disconnectButton_clicked()
{
    emit returnToMainWindow();
}
