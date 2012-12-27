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

    void addRevision(Revision *);

    QList<Revision *> getRoots();

signals:
    void notifyNewProcess(Process *p);
    void notifyRemoveProcess(Process *p);
    void revisionAdded(Revision *r);
    
public slots:
    void killProcess(Process *p);
    void readPendingDatagrams();

private:
    QUdpSocket *outSocket;
    QUdpSocket *inSocket;

    //we know the roots have null parents
    QList<Revision *> revisions; //all revisions
    QList<Process *> processes; //all processes
    
};

#endif // ENGINE_H
