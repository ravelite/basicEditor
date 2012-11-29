#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMdiArea>
#include <QUdpSocket>
#include <QMap>
#include <QTreeWidget>
#include "revision.h"
#include "revui.h"
#include "revtree.h"
#include "process.h"
#include "codearea.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void killChuckShred(Process *p);
    void addRevisionMain(Revision *r);
    void killProcess(Process *p);

private slots:
    void on_actionOpen_triggered();
    void on_actionAdd_Shred_triggered();
    void readPendingDatagrams();
    void on_actionSave_triggered();
    void on_actionNew_triggered();

private:
    Ui::MainWindow *ui;

    QUdpSocket *outSocket;
    CodeArea *codeArea;
    //QWidget *shredTree;
    RevTree *shredTree;

    QUdpSocket *udpSocket;
    QString sessionName;
    QVariantMap macros;

    QList<Revision *> revisions;
    QList<Process *> processes;

    //a sequence number for the OSC requests
    int seqRequest;
    int nBuffers;

    //functions
    void sendTestMessage();
    void shredFile(QString filePath, int revID);
    bool saveFile(QString filePath, QString textContent);
    QString applyMacros(QString text);
    void createSessionDirectory();
    void loadMacros();
    Revision *findRevision(int revId);
    Process *findProcess(int procId, int procSrcType);
};

#endif // MAINWINDOW_H
