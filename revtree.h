#ifndef REVTREE_H
#define REVTREE_H

#include <QTreeWidget>

#include "revui.h"

class RevTree : public QTreeWidget, public Revui
{
    Q_OBJECT
public:
    explicit RevTree(QWidget *parent = 0);

public slots:
    void addRevision( Revision &r );
    void removeRevision( Revision &r );
    void selectRevision( Revision &r );
    void addProcess( Process &p );
    void removeProcess( Process &p );

};

#endif // REVTREE_H
