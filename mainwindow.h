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
#include "engine.h"

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
    void addRevisionMain(Revision *r);

private slots:
    void on_actionOpen_triggered();
    void on_actionAdd_Shred_triggered();
    void on_actionSave_triggered();
    void on_actionNew_triggered();

    void on_actionChuckMode_triggered();

    void on_actionSCMode_triggered();

    void on_actionToggle_Mode_triggered();

private:
    Ui::MainWindow *ui;

    CodeArea *codeArea;
    //QWidget *shredTree;
    RevTree *shredTree;
    Engine *engine;

    QString sessionName;
    QVariantMap macros;

    //a sequence number for the OSC requests
    int seqRequest;
    int nBuffers;

    enum LangMode { chuckMode, scMode };
    LangMode langMode;

    //functions
    bool saveFile(QString filePath, QString textContent);
    QString applyMacros(QString text);
    void createSessionDirectory();
    void loadMacros();
    void updateModeChecks();
};

#endif // MAINWINDOW_H
