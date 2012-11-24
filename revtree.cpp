#include "revtree.h"

RevTree::RevTree(QWidget *parent) :
    QTreeWidget(parent)
{
    //set the columns for (Name, ID)
    setColumnCount(2);

    //connect to item->pRev mapping
    connect( this, SIGNAL(itemActivated(QTreeWidgetItem*,int)),
             this, SLOT(fireSelectedRevision(QTreeWidgetItem*)) );

    //also for single clicks
    connect( this, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
             this, SLOT(fireSelectedRevision(QTreeWidgetItem*)) );
}

void RevTree::addRevision(Revision *r)
{
    QStringList str;
    str << r->getDisplayName();
    str << QString::number( r->getID() );

    QTreeWidgetItem *item = new QTreeWidgetItem(str);

    addTopLevelItem(item);

    //add the mapping for this item
    itemMap[r] = item;
    itemMapRight[item] = r;
}

void RevTree::removeRevision(Revision *r)
{
}

void RevTree::selectRevision(Revision *r)
{
    setCurrentItem( itemMap[r] );
}

void RevTree::addProcess(Process *p)
{
}

void RevTree::removeProcess(Process *p)
{
}

void RevTree::fireSelectedRevision(QTreeWidgetItem *item)
{
    selectedRevision( itemMapRight[item] );
}


