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
    qmanager = new QNetworkAccessManager(this);

    ftpAddress = ftpHost;
    ftpPort = ftpport;
    username = ftpUser;
    password = ftpPass;
    ui->setupUi(this);
    ui->progressBar->setEnabled(false);
    ui->progressBar->setValue(0);
    ui->listWidget->clear();
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
    QFile* file = new QFile(ui->lineEdit->text());
    qDebug()<< "Open file:" << ui->lineEdit->text();
    QFileInfo fileInfo(*file);
    uploadFileName = fileInfo.fileName();

    QUrl ftpPath;
    ftpPath.setUrl(ftpAddress + uploadFileName);
    ftpPath.setUserName(username);
    ftpPath.setPassword(password);
    ftpPath.setPort(ftpPort);

    if (file->open(QIODevice::ReadOnly))
    {
        ui->progressBar->setEnabled(true);
        ui->progressBar->setValue(0);

        QNetworkRequest request;
        request.setUrl(ftpPath);
        qDebug()<< "Upload file:" << ftpAddress + uploadFileName;

        QNetworkReply *uploadFileReply = qmanager->put(request, file);
        QObject::connect(uploadFileReply, &QNetworkReply::finished, [uploadFileReply, this]{
            if(uploadFileReply->error() != QNetworkReply::NoError)
            {
                QMessageBox::warning(this, "Failed", uploadFileReply->errorString());
                ui->label_4->setText("Upload file failed");
            }
            else
            {
                ui->label_4->setText("Upload file successfully");
                // Change the files.txt info
                uploadFileListUpdate();


                /// get new list file
                getFileList();
            }
        });
    }
    else
    {
        QMessageBox::warning(this, "Invalid File", "Failed to open file for upload.");
    }
}

void FileView::getFileList()
{
    QUrl ftpPath;
    ftpPath.setUrl(ftpAddress + "files.txt");
    ftpPath.setUserName(username);
    ftpPath.setPassword(password);
    ftpPath.setPort(ftpPort);

    QNetworkRequest request;
    request.setUrl(ftpPath);

    QNetworkReply *downloadFileReply = qmanager->get(request);
    connect(downloadFileReply, &QNetworkReply::finished, this, [downloadFileReply, this]{
        if(downloadFileReply->error() != QNetworkReply::NoError)
        {
            QMessageBox::warning(this, "Failed", "Failed to load file list: " + downloadFileReply->errorString());
        }
        else
        {
            QByteArray responseData;
            if (downloadFileReply->isReadable())
            {
                responseData = downloadFileReply->readAll();
            }

            // Display file list
            ui->listWidget->clear();
            fileList = QString(responseData).split(",");
            qDebug()<< fileList;

            if (fileList.size() > 0)
            {
                for (int i = 0; i < fileList.size(); i++)
                {
                    if (fileList.at(i) != "")
                    {
                        ui->listWidget->addItem(fileList.at(i));
                    }
                }
            }
        }
    });
};

void FileView::uploadFileListUpdate()
{
    QUrl ftpPath;
    ftpPath.setUrl(ftpAddress + "files.txt");
    ftpPath.setUserName(username);
    ftpPath.setPassword(password);
    ftpPath.setPort(ftpPort);


    QNetworkRequest request;
    request.setUrl(ftpPath);
    request.setRawHeader("Command", "LIST");

    QNetworkReply *downloadFileReply = qmanager->get(request);
    connect(downloadFileReply, &QNetworkReply::finished, this, [downloadFileReply, this]{
        if(downloadFileReply->error() != QNetworkReply::NoError)
        {
            QMessageBox::warning(this, "Failed", "Failed to load file list: " + downloadFileReply->errorString());
        }
        else
        {
            QByteArray responseData;
            if (downloadFileReply->isReadable())
            {
                responseData = downloadFileReply->readAll();
            }

            // Display file list
            ui->listWidget->clear();
            fileList = QString(responseData).split(",");
            qDebug()<< fileList;

            if (fileList.size() > 0)
            {
                for (int i = 0; i < fileList.size(); i++)
                {
                    if (fileList.at(i) != "")
                    {
                        ui->listWidget->addItem(fileList.at(i));
                    }
                }
            }
            fileList.append(uploadFileName);

            QString fileName = "files.txt";
            QFile* file = new QFile(qApp->applicationDirPath() + "/" + fileName);
            file->open(QIODevice::ReadWrite);
            if (fileList.size() > 0)
            {
                for (int j = 0; j < fileList.size(); j++)
                {
                    if (fileList.at(j) != "")
                    {
                        file->write(QString(fileList.at(j) + ",").toUtf8());
                    }
                }
            }
            file->close();
            QFile* newFile = new QFile(qApp->applicationDirPath() + "/" + fileName);
            if (newFile->open(QIODevice::ReadOnly))
            {
                // Update file list to server
                QUrl ftpPath;
                ftpPath.setUrl(ftpAddress + fileName);
                ftpPath.setUserName(username);
                ftpPath.setPassword(password);
                ftpPath.setPort(ftpPort);

                QNetworkRequest request;
                request.setUrl(ftpPath);
                QNetworkReply *reply = qmanager->put(request, newFile);
                connect(reply, &QNetworkReply::finished, [reply, this]{
                    if(reply->error() != QNetworkReply::NoError)
                    {
                        QMessageBox::warning(this, "Failed", "Failed to update file list: " + reply->errorString());
                    }else{
                        getFileList();
                    }

                });
                file->close();
            }




        }
    });
};

void FileView::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    downloadFileName = item->text();
    if(ui->lineEdit_2->text().size()==0){
        QMessageBox::warning(this, "Failed", "Please choose download folder");
    }else{
        QUrl ftpPath;
        ftpPath.setUrl(ftpAddress + downloadFileName);
        ftpPath.setUserName(username);
        ftpPath.setPassword(password);
        ftpPath.setPort(ftpPort);

        QNetworkRequest request;
        request.setUrl(ftpPath);

        QNetworkReply *reply = qmanager->get(request);
        connect(reply, &QNetworkReply::finished, [reply,this]{
            if(reply->error() != QNetworkReply::NoError)
            {
                QMessageBox::warning(this, "Failed", "Failed to load file list: " + reply->errorString());
            }
            else
            {
                QByteArray responseData;
                if (reply->isReadable())
                {
                    responseData = reply->readAll();
                }

                if (!responseData.isEmpty())
                {
                    // Download finished
                    QString folder = ui->lineEdit_2->text();
                    QFile file(folder + "/" + downloadFileName);
                    file.open(QIODevice::WriteOnly);
                    file.write((responseData));
                    file.close();

                    QMessageBox::information(this, "Success", "File successfully downloaded.");
                }
            }
        });
    }
}


void FileView::on_pushButton_3_clicked()
{
    QString folder = QFileDialog::getExistingDirectory(this, tr("Open Directory"), qApp->applicationDirPath(), QFileDialog::ShowDirsOnly);

    ui->lineEdit_2->setText(folder);
}

