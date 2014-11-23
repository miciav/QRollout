#include <QtGui>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "rollout_details.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setCentralWidget(ui->mdiArea);

    createActions();
    trayIcon= new QSystemTrayIcon(QIcon(":/Resources/trash.svg"),this);
    createMenuTrayIcon();
    trayIcon->show();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionRollout_triggered()
{
    Rollout_Details *Rd = new Rollout_Details();
    ui->mdiArea->setOption(QMdiArea::DontMaximizeSubWindowOnActivation, false);
    ui->mdiArea->addSubWindow(Rd);
    Rd->showMaximized();
    ui->mdiArea->tileSubWindows();

}
void MainWindow::createActions()
{
    minimizeAction = new QAction(tr("Mi&nimize"), this);
    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

    maximizeAction = new QAction(tr("Ma&ximize"), this);
    connect(maximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));

    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void MainWindow::createMenuTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon->setContextMenu(trayIconMenu);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (trayIcon->isVisible()) {
        hide();
        event->ignore();
    }
}
