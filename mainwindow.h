#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "UnResponseListener.h"
#include <QMdiArea>
#include <QStackedWidget>

#include <QUdpSocket>

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

    void readPendingDatagrams();

private:
    Ui::MainWindow *ui;

    UnResponseListener listener;
    //QQueue udpQueue;
    //QMutex udpQueueMutex;

    UdpListeningReceiveSocket *inSocket;
    UdpTransmitSocket outSocket;

    QMdiArea *mdiArea;
    QWidget *shredTree;

    //a sequence number for the OSC requests
    int seqRequest;
    int nBuffers;

    QUdpSocket *udpSocket;
};

#endif // MAINWINDOW_H
