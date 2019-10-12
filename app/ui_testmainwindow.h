/********************************************************************************
** Form generated from reading UI file 'testmainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TESTMAINWINDOW_H
#define UI_TESTMAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TestMainWindow
{
public:
    QMenuBar *menubar;
    QWidget *centralwidget;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *TestMainWindow)
    {
        if (TestMainWindow->objectName().isEmpty())
            TestMainWindow->setObjectName(QStringLiteral("TestMainWindow"));
        TestMainWindow->resize(800, 600);
        menubar = new QMenuBar(TestMainWindow);
        menubar->setObjectName(QStringLiteral("menubar"));
        TestMainWindow->setMenuBar(menubar);
        centralwidget = new QWidget(TestMainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        TestMainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(TestMainWindow);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        TestMainWindow->setStatusBar(statusbar);

        retranslateUi(TestMainWindow);

        QMetaObject::connectSlotsByName(TestMainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *TestMainWindow)
    {
        TestMainWindow->setWindowTitle(QApplication::translate("TestMainWindow", "MainWindow", 0));
    } // retranslateUi

};

namespace Ui {
    class TestMainWindow: public Ui_TestMainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TESTMAINWINDOW_H
