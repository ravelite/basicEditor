#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "UnResponseListener.h"
#include <QMdiArea>
#include <QStackedWidget>

#include <QUdpSocket>
#include <QMap>

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
    void killShred();
    void on_actionSave_triggered();
    void onTextChanged();

private:
    Ui::MainWindow *ui;

    UnResponseListener listener;

    UdpListeningReceiveSocket *inSocket;
    UdpTransmitSocket outSocket;

    QMdiArea *mdiArea;
    QWidget *shredTree;

    //a sequence number for the OSC requests
    int seqRequest;
    int nBuffers;

    QUdpSocket *udpSocket;

    QMap<QString, int> maxShredRevision;
};

#endif // MAINWINDOW_H
