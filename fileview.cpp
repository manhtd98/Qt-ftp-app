#include "fileview.h"
#include "ui_fileview.h"
#include <QtGui>
#include <QFileDialog>
#include <QtNetwork/QNetworkAccessManager>
#include <QNetworkReply>
#include <QMessageBox>

FileView::FileView(QString ftpHost, QString ftpUser, QString ftpPass, int ftpport, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FileView)
{
    ftpAddress = ftpHost;
    ftpPort = ftpport;
    username = ftpUser;
    password = ftpPass;
    ui->setupUi(this);
    ui->listWidget->clear();
    qDebug()<< ftpAddress<< ftpPort<< username<< password;
    FtpClient.FTPConnect(ftpAddress, ftpPort, username, password);

    getFileList();
}

FileView::~FileView()
{
    delete ui;
}

void FileView::on_pushButton_2_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Upload file", NULL);
    ui->lineEdit->setText(filename);
}

void FileView::on_pushButton_clicked()
{
    // Choost upload file path
    QFile* file = new QFile(ui->lineEdit->text());
    qDebug()<< "Open file:" << ui->lineEdit->text();
    QFileInfo fileInfo(*file);
    QString uploadFileName = fileInfo.fileName();
    if (file->open(QIODevice::ReadOnly))
    {
        FtpClient.uploadFile(fileInfo.filePath(), dirPath + "/" + uploadFileName);
        ui->label_4->setText("Upload file successfully");
        getFileList();
    }

}

void FileView::getFileList()
{
    QPair<QStringList, QStringList> res = FtpClient.ListDir(dirPath);
    QStringList fileList=res.first;
    QStringList isDirList = res.second;
    ui->listWidget->clear();
    // QIcon fileIcon(":/icons/file.png");
    QIcon dirIcon("://*folder*/.png");
    QListWidgetItem *item = new QListWidgetItem();
    item->setText("...");
    item->setTextAlignment(Qt::AlignLeft);
    item -> setBackground(Qt::green);
    ui->listWidget->addItem(item);

    if (fileList.size() > 0)
    {
        for (int i = 0; i < fileList.size(); i++)
        {
            if (fileList.at(i) != "")
            {
                QListWidgetItem *item = new QListWidgetItem(dirIcon, fileList.at(i));
                item->setForeground(Qt::white);
                item->setTextAlignment(Qt::AlignLeft);
                item->setData(Qt::UserRole, QVariant(isDirList.at(i)));
                ui->listWidget->addItem(item);
                if(isDirList.at(i)=="d"){
                    item -> setBackground(Qt::blue);
                }
            }
        }
    }
}




void FileView::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{

    if(item->text() == "..."){
        if(dirPath.split("/").size()>0){
            QStringList item = dirPath.split("/");
            item.removeLast();
            dirPath = item.join("/");
            getFileList();
        }
    }else{
        if(item->data(Qt::UserRole)=="d"){
            dirPath += "/"+item->text();
            getFileList();
        }else{
            // Download file here
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

