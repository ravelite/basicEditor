#ifndef REVTREEITEM_H
#define REVTREEITEM_H

#include <QTreeWidgetItem>
#include "revision.h"
#include "revtree.h"

class RevTreeItem : public QObject, public QTreeWidgetItem
{
    Q_OBJECT

public:
    explicit RevTreeItem(Revision *r);

public slots:
    void refreshName(void);

signals:

private:
    Revision *rev;
    
};

#endif // REVTREEITEM_H
