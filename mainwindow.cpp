#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtNetwork/QNetworkAccessManager>
#include <QNetworkReply>
#include <QRegExp>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->lineEdit_3->setEchoMode(QLineEdit::Password);
}

MainWindow::~MainWindow()
{
    delete ui;
}


QString extractHostFromFtpUrl(const QString &url) {
    static QRegularExpression regex(R"(^ftp:\/\/([^\/:]+))"); // Regex to capture the host part
    QRegularExpressionMatch match = regex.match(url);
    if (match.hasMatch()) {
        return match.captured(1); // Return the captured host
    }
    return QString(); // Return empty if no match
}
QString extractIpAddress(const QString &input) {
    static QRegularExpression regex(R"((\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}))");
    QRegularExpressionMatch match = regex.match(input);
    if (match.hasMatch()) {
        return match.captured(1); // Return the captured IP address
    }
    return QString(); // Return empty if no match
}

void MainWindow::on_pushButton_clicked()
{
    line1=ui->lineEdit->text();
    line2=ui->lineEdit_2->text();
    line3=ui->lineEdit_3->text();
    line4=ui->lineEdit_4->text().toInt();
    qDebug() << line4;
    bool valid=true;

    if(line1.length()==0){
        valid=false;
        ui->lineEdit->setPlaceholderText("Please type URL Server");
    }else{
        QString extractFTPURL = extractHostFromFtpUrl(line1);
        QString extractIpURL = extractIpAddress(line1);
        if(!(extractFTPURL.size() or extractIpURL.size())){
            valid=false;
            ui->lineEdit->clear();
            ui->lineEdit->setPlaceholderText("FTP URL not valid, please check it");
        }else{
            if(extractFTPURL.size())line1 = extractFTPURL;
            else line1 = extractIpURL;
        }
    }
    if (line1.endsWith("/")){
        line1.chop(1);
    }

    if(line2.length()==0){
        valid=false;
        ui->lineEdit_2->setPlaceholderText("Please type valid username");
    }
    if(line3.length()==0){
        valid=false;
        ui->lineEdit_3->setPlaceholderText("Please type valid password");
    }
    if(line4==0){
        valid=false;
        ui->lineEdit_4->clear();
        ui->lineEdit_4->setPlaceholderText("Please type valid port");
    }
    if(valid){
        ui->pushButton->setText("Connecting");
        ftpClient ftp;
        int login_Response = ftp.FTPConnect(line1, line4, line2, line3);
        if(login_Response!=-1){
            ui->pushButton->setText("Connected");
            fileView = new FileView(line1, line2, line3, line4);
            connect(fileView, &FileView::returnToMainWindow, this, [this]() {
                fileView->hide();
                delete fileView;
                this->show();
            });
            fileView->show();
            this->hide();

        }else{
            ui->pushButton->setText("Failed");
        }
    }
}

