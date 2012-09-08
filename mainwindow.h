#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "UnResponseListener.h"
#include <QMdiArea>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_actionOpen_triggered();

    void on_actionAdd_Shred_triggered();

private:
    Ui::MainWindow *ui;

    UnResponseListener listener;
    UdpListeningReceiveSocket *inSocket;
    UdpTransmitSocket outSocket;

    QMdiArea *mdiArea;
};

#endif // MAINWINDOW_H