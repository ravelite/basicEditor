#ifndef REVTREE_H
#define REVTREE_H

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QMap>

#include "revui.h"

class RevTree : public QTreeWidget, public Revui
{
    Q_OBJECT
public:
    RevTree(QWidget *parent = 0);

    static const int REV_TYPE;
    static const int PROC_TYPE;

public slots:
    void addRevision( Revision *r );
    void removeRevision( Revision *r );
    void selectRevision( Revision *r );
    void addProcess( Process *p );
    void removeProcess( Process *p );

    void itemActivate(QTreeWidgetItem *item);
    void itemClick(QTreeWidgetItem *item);

signals:
    void addedRevision( Revision *r );
    void removedRevision( Revision *r );
    void selectedRevision( Revision *r );
    void addedProcess( Process *p );
    //void removedProcess( Process *p );

    void requestRemoveProcess( Process *p );

private:
    QMap<Revision *, QTreeWidgetItem *> revMap;
    QMap<QTreeWidgetItem *, Revision *> revMapRight;

    QMap<Process *, QTreeWidgetItem *> procMap;
    QMap<QTreeWidgetItem *, Process *> procMapRight;

};

#endif // REVTREE_H
