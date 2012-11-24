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

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void addCodeWindow(Revision *r, QString fileText, int cursorPos);

public slots:
    void killShred(QTreeWidgetItem *item);

    void selectRevision(Revision *r);
    void fireSelectRevision(QMdiSubWindow *sub);

private slots:
    void on_actionOpen_triggered();
    void on_actionAdd_Shred_triggered();
    void readPendingDatagrams();
    void on_actionSave_triggered();
    void onTextChanged();
    void on_actionNew_triggered();

private:
    Ui::MainWindow *ui;

    QUdpSocket *outSocket;
    QMdiArea *mdiArea;
    //QWidget *shredTree;
    RevTree *shredTree;

    QMap<Revision *,QMdiSubWindow *> subWindowMap;
    QMap<QMdiSubWindow *, Revision *> subWindowMap2;

    QUdpSocket *udpSocket;
    QString sessionName;
    QVariantMap macros;

    QList<Revision *> revisions;

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
};

#endif // MAINWINDOW_H
