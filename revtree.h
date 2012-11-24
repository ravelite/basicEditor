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

public slots:
    void addRevision( Revision *r );
    void removeRevision( Revision *r );
    void selectRevision( Revision *r );
    void addProcess( Process *p );
    void removeProcess( Process *p );

    void fireSelectedRevision(QTreeWidgetItem *);

signals:
    void addedRevision( Revision *r );
    void removedRevision( Revision *r );
    void selectedRevision( Revision *r );
    void addedProcess( Process *p );
    void removedProcess( Process *p );

private:
    QMap<Revision *, QTreeWidgetItem *> itemMap;
    QMap<QTreeWidgetItem *, Revision *> itemMapRight;

};

#endif // REVTREE_H
