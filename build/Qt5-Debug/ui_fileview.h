/********************************************************************************
** Form generated from reading UI file 'fileview.ui'
**
** Created by: Qt User Interface Compiler version 5.15.13
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FILEVIEW_H
#define UI_FILEVIEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FileView
{
public:
    QLabel *label_4;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QLabel *label_2;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *lineEdit;
    QPushButton *pushButton_2;
    QPushButton *pushButton;
    QLabel *label_3;
    QHBoxLayout *horizontalLayout;
    QLineEdit *lineEdit_2;
    QPushButton *pushButton_3;
    QLabel *label;
    QListWidget *listWidget;

    void setupUi(QWidget *FileView)
    {
        if (FileView->objectName().isEmpty())
            FileView->setObjectName(QString::fromUtf8("FileView"));
        FileView->resize(720, 560);
        label_4 = new QLabel(FileView);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(140, 180, 641, 21));
        verticalLayoutWidget = new QWidget(FileView);
        verticalLayoutWidget->setObjectName(QString::fromUtf8("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(10, 17, 691, 531));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        label_2 = new QLabel(verticalLayoutWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        verticalLayout->addWidget(label_2);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        lineEdit = new QLineEdit(verticalLayoutWidget);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

        horizontalLayout_2->addWidget(lineEdit);

        pushButton_2 = new QPushButton(verticalLayoutWidget);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        QFont font;
        font.setPointSize(12);
        pushButton_2->setFont(font);

        horizontalLayout_2->addWidget(pushButton_2);

        pushButton = new QPushButton(verticalLayoutWidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setFont(font);

        horizontalLayout_2->addWidget(pushButton);


        verticalLayout->addLayout(horizontalLayout_2);

        label_3 = new QLabel(verticalLayoutWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        verticalLayout->addWidget(label_3);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        lineEdit_2 = new QLineEdit(verticalLayoutWidget);
        lineEdit_2->setObjectName(QString::fromUtf8("lineEdit_2"));

        horizontalLayout->addWidget(lineEdit_2);

        pushButton_3 = new QPushButton(verticalLayoutWidget);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));

        horizontalLayout->addWidget(pushButton_3);


        verticalLayout->addLayout(horizontalLayout);

        label = new QLabel(verticalLayoutWidget);
        label->setObjectName(QString::fromUtf8("label"));
        QFont font1;
        font1.setPointSize(20);
        label->setFont(font1);

        verticalLayout->addWidget(label);

        listWidget = new QListWidget(verticalLayoutWidget);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));
        listWidget->setMinimumSize(QSize(15, 15));
        listWidget->setBaseSize(QSize(15, 15));
        listWidget->setLineWidth(3);
        listWidget->setIconSize(QSize(10, 10));

        verticalLayout->addWidget(listWidget);


        retranslateUi(FileView);

        QMetaObject::connectSlotsByName(FileView);
    } // setupUi

    void retranslateUi(QWidget *FileView)
    {
        FileView->setWindowTitle(QCoreApplication::translate("FileView", "Form", nullptr));
        label_4->setText(QString());
        label_2->setText(QCoreApplication::translate("FileView", "Upload file", nullptr));
        pushButton_2->setText(QCoreApplication::translate("FileView", "Choose file", nullptr));
        pushButton->setText(QCoreApplication::translate("FileView", "Upload", nullptr));
        label_3->setText(QCoreApplication::translate("FileView", "Download file", nullptr));
        pushButton_3->setText(QCoreApplication::translate("FileView", "Download folder", nullptr));
        label->setText(QCoreApplication::translate("FileView", "File Brower", nullptr));
    } // retranslateUi

};

namespace Ui {
    class FileView: public Ui_FileView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FILEVIEW_H
