#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QSystemTrayIcon>
#include "qRolloutThread.h"


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();

    void createActions(); // crea le azioni relative al menu dell'icona

    void createMenuTrayIcon();

    void closeEvent(QCloseEvent *event); // evento che si lancia quando si chiude la finestra principale.

private:
    QRolloutThread * r;
    Ui::MainWindow *ui;
    QSystemTrayIcon *trayIcon;
    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;
    QMenu *trayIconMenu;


private slots:
    void on_actionRollout_triggered();
};

#endif // MAINWINDOW_H
