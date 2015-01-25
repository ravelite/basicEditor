#include "proctreeitem.h"

ProcTreeItem::ProcTreeItem(Process *p) :
    QTreeWidgetItem(RevTree::PROC_TYPE)
{
    setText(0, QString::number( p->id ) );
}
