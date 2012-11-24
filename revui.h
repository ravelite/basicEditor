#ifndef REVUI_H
#define REVUI_H

#include "revision.h"
#include "process.h"

class Revui
{
public:
    //try not having a constructor

    //do things to change the UI
public slots:
    virtual void addRevision( Revision *r ) = 0;
    virtual void removeRevision( Revision *r ) = 0;
    virtual void selectRevision( Revision *r ) = 0;
    virtual void addProcess( Process *p ) = 0;
    virtual void removeProcess( Process *p ) = 0;

    //the UI sends messages to the engine
signals:
    virtual void addedRevision( Revision *r ) = 0;
    virtual void removedRevision( Revision *r ) = 0;
    virtual void selectedRevision( Revision *r ) = 0;
    virtual void addedProcess( Process *p ) = 0;
    virtual void removedProcess( Process *p ) = 0;

};

#endif // REVUI_H
