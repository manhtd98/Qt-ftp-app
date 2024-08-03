#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtNetwork/QNetworkAccessManager>
#include <QNetworkReply>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    line1=ui->lineEdit->text();
    line2=ui->lineEdit_2->text();
    line3=ui->lineEdit_3->text();
    line4=ui->lineEdit_4->text().toInt();
    bool valid=true;
    if(line1.length()==0){
        valid=false;
        ui->lineEdit->setPlaceholderText("Vui lòng nhập URL Server");
    }else if (!line1.startsWith("ftp:/")){
        valid=false;
        ui->lineEdit->clear();
        ui->lineEdit->setPlaceholderText("Địa chỉ Protocol không chính xác");
    }
    if (!line1.endsWith("/")){
        line1 += "/";
    }

    if(line2.length()==0){
        valid=false;
        ui->lineEdit_2->setPlaceholderText("Vui lòng nhập username");
    }
    if(line3.length()==0){
        valid=false;
        ui->lineEdit_3->setPlaceholderText("Vui lòng nhập mật khẩu");
    }
    if(line4==0){
        valid=false;
        ui->lineEdit_4->setPlaceholderText("Vui lòng nhập port");
    }
    if(valid){
        QDataStream;


        ui->pushButton->setText("Connecting");
        QUrl FTPClient(line1);
        FTPClient.setUserName(line2);
        FTPClient.setPassword(line3);
        FTPClient.setPort(line4);
        QNetworkAccessManager *qnetwork = new QNetworkAccessManager();
        qreply = qnetwork->get(QNetworkRequest(FTPClient));
        QObject::connect(qreply, &QNetworkReply::finished, this, &MainWindow::connectFTP);
    }
}
void MainWindow::connectFTP(){
    if((qreply->error()==QNetworkReply::ContentOperationNotPermittedError) or (qreply->error()==QNetworkReply::NoError)){
        ui->pushButton->setText("Connected");
        FileView *fileView = new FileView(line1, line2, line3, line4);
        fileView->show();
        this->hide();
    }else{
        ui->pushButton->setText("Disconnected");
        qDebug()<< "Error:" << qreply->errorString();
    }
}

