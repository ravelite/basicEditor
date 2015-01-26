#include "revtreeitem.h"

RevTreeItem::RevTreeItem(Revision *r) :
    QTreeWidgetItem(RevTree::REV_TYPE), rev(r)
{
    setText( 0, r->getShortName() );
}

void RevTreeItem::updateDisplay()
{
    setText( 0, rev->getShortName() );
}
