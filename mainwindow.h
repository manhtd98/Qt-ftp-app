#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "fileview.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void connectFTP();


private:
    Ui::MainWindow *ui;
    QString line1;
    QString line2;
    QString line3;
    int line4;
    QNetworkReply *qreply;
};
#endif // MAINWINDOW_H
