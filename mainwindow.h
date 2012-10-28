#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMdiArea>
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

    void on_actionNew_triggered();

private:
    Ui::MainWindow *ui;

    QUdpSocket *outSocket;
    QMdiArea *mdiArea;
    QWidget *shredTree;
    QUdpSocket *udpSocket;
    QMap<QString, int> maxShredRevision;
    QString sessionName;
    QVariantMap macros;

    //a sequence number for the OSC requests
    int seqRequest;
    int nBuffers;

    //functions
    void sendTestMessage();
    void shredFile(QString filePath, int revID);
    bool saveFile(QString filePath, QString textContent);
    QString applyMacros(QString text);
};

#endif // MAINWINDOW_H
