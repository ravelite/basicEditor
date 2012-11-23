#ifndef REVUI_H
#define REVUI_H

#include "revision.h"
#include "process.h"

class Revui
{
public:
    Revui();

    //do things to change the UI
public slots:
    virtual void addRevision( Revision &r ) = 0;
    virtual void removeRevision( Revision &r ) = 0;
    virtual void selectRevision( Revision &r ) = 0;
    virtual void addProcess( Process &p ) = 0;
    virtual void removeProcess( Process &p ) = 0;

    //the UI sends messages to the engine
signals:
    void addedRevision( Revision &r );
    void removedRevision( Revision &r );
    void selectedRevision( Revision &r );
    void addedProcess( Process &p );
    void removedProcess( Process &p );

};

#endif // REVUI_H
