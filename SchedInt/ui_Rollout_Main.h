/********************************************************************************
** Form generated from reading UI file 'Rollout_Main.ui'
**
** Created: Sat Dec 25 21:03:11 2010
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ROLLOUT_MAIN_H
#define UI_ROLLOUT_MAIN_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_Rollout_Main
{
public:
    QPushButton *pushButton;

    void setupUi(QDialog *Rollout_Main)
    {
        if (Rollout_Main->objectName().isEmpty())
            Rollout_Main->setObjectName(QString::fromUtf8("Rollout_Main"));
        Rollout_Main->resize(400, 300);
        pushButton = new QPushButton(Rollout_Main);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(150, 40, 73, 23));

        retranslateUi(Rollout_Main);

        QMetaObject::connectSlotsByName(Rollout_Main);
    } // setupUi

    void retranslateUi(QDialog *Rollout_Main)
    {
        Rollout_Main->setWindowTitle(QApplication::translate("Rollout_Main", "Dialog", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("Rollout_Main", "PushButton", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Rollout_Main: public Ui_Rollout_Main {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ROLLOUT_MAIN_H
