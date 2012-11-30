#ifndef ENGINE_H
#define ENGINE_H

#include <QObject>
#include <QUdpSocket>

#include "revision.h"
#include "process.h"

class Engine : public QObject
{
    Q_OBJECT
public:
    explicit Engine(QObject *parent = 0);
    ~Engine();

    void sendTestMessage();
    void shredFile(QString filePath, int revID);
    void killChuckShred(Process *p);
    Process *findProcess(int procId, int srcLang);
    Revision *findRevision(int revId);

    QList<Revision *> revisions;
    QList<Process *> processes;

signals:
    void notifyNewProcess(Process *p);
    void notifyRemoveProcess(Process *p);
    
public slots:
    void killProcess(Process *p);
    void readPendingDatagrams();

private:
    QUdpSocket *outSocket;
    QUdpSocket *inSocket;
    
};

#endif // ENGINE_H
