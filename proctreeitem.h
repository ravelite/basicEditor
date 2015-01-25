#ifndef PROCTREEITEM_H
#define PROCTREEITEM_H

#include <QTreeWidgetItem>
#include "process.h"
#include "revtree.h"

class ProcTreeItem : public QTreeWidgetItem
{
    //Q_OBJECT

public:
    explicit ProcTreeItem(Process *p);
    
};

#endif // PROCTREEITEM_H
