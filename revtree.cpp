#include "revtree.h"

const int RevTree::REV_TYPE = 1001;
const int RevTree::PROC_TYPE = 1002;

RevTree::RevTree(QWidget *parent) :
    QTreeWidget(parent)
{
    //set the columns for (Name, ID)
    setColumnCount(2);

    //connect to item->pRev mapping
    connect( this, SIGNAL(itemActivated(QTreeWidgetItem*,int)),
             this, SLOT(itemActivate(QTreeWidgetItem*)) );

    //also for single clicks
    connect( this, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
             this, SLOT(itemClick(QTreeWidgetItem*)) );
}

void RevTree::addRevision(Revision *r)
{
    //TODO: dirty hack for multiple adding of revs, fix
    if ( !revMap.contains(r) ) {
        QStringList str;
        str << r->getDisplayName();
        str << QString::number( r->getID() );

        QTreeWidgetItem *item = new QTreeWidgetItem(str, REV_TYPE);

        addTopLevelItem(item);

        //add the mapping for this item
        revMap[r] = item;
        revMapRight[item] = r;
    }
}

void RevTree::removeRevision(Revision *r)
{
    //remove the mappings
}

void RevTree::selectRevision(Revision *r)
{
    setCurrentItem( revMap[r] );
}

void RevTree::addProcess(Process *p)
{
    QStringList str;
    str << QString::number( p->id );
    str << "";

    QTreeWidgetItem *item =
            new QTreeWidgetItem(str, RevTree::PROC_TYPE);
    revMap[p->rev]->addChild( item );

    //add the mapping for this item
    procMap[p] = item;
    procMapRight[item] = p;
}

//TODO: I think this process is similir to add/removeRevision
//maybe we can refactor and use the same
void RevTree::removeProcess(Process *p)
{
    //remove the mapping
    QTreeWidgetItem *item = procMap[p];

    procMap.remove(p);
    procMapRight.remove(item);

    //remove it from the UI
    delete item;
}

void RevTree::itemActivate(QTreeWidgetItem *item)
{
    if ( item->type() == REV_TYPE )
        selectedRevision( revMapRight[item] );
    else if ( item->type() == PROC_TYPE )
        requestRemoveProcess( procMapRight[item] );

}

void RevTree::itemClick(QTreeWidgetItem *item)
{
    if ( item->type() == REV_TYPE )
        selectedRevision( revMapRight[item] );
}

