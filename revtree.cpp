#include "revtree.h"

#include <QTreeWidgetItemIterator>

const int RevTree::REV_TYPE = 1001;
const int RevTree::PROC_TYPE = 1002;

RevTree::RevTree(QWidget *parent) :
    QTreeWidget(parent),
    flattenTree(false)
{
    //set the columns for (Name, ID)
    setColumnCount(10);

    setColumnWidth(0, 150);
    for (int i=1; i<=columnCount(); i++)
        setColumnWidth(i, 20);

    //connect to item->pRev mapping
    connect( this, SIGNAL(itemActivated(QTreeWidgetItem*,int)),
             this, SLOT(itemActivate(QTreeWidgetItem*,int)) );

    //also for single clicks
    connect( this, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
             this, SLOT(itemClick(QTreeWidgetItem*,int)) );

    //options for appearance
    setIndentation(8);
    setHeaderHidden(true);

    //keep double-clicks from expanding/collapsing for all columns
    //though we can recreate this behavior
    setExpandsOnDoubleClick(false);

    //setWordWrap(true);
    //setRootIsDecorated(false);
    //setIndentation(0);
}

void RevTree::addRevision(Revision *r)
{
    //TODO: dirty hack for multiple adding of revs, fix
    if ( !revMap.contains(r) ) {
        QStringList str;
        //str << r->getDisplayName();
        str << r->getShortName();
        //str << QString::number( r->getID() );

        QTreeWidgetItem *item = new QTreeWidgetItem(str, REV_TYPE);

        if ( flattenTree || r->parent==NULL )
            addTopLevelItem(item);
        else {
            QTreeWidgetItem *parent = revMap[r->parent];
            parent->addChild(item);
        }

        //add the mapping for this item
        revMap[r] = item;
        revMapRight[item] = r;

        //resize column if needed
        if ( sizeHintForColumn(0) > columnWidth(0) )
            resizeColumnToContents(0);
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

    item->setHidden(true); //make it invisible

    //add the mapping for this item
    procMap[p] = item;
    procMapRight[item] = p;

    //change the display of the parent
    updateParentRevision( p->rev );
}

void RevTree::updateParentRevision( Revision *r )
{
    QTreeWidgetItem *item = revMap[r];
    QTreeWidgetItem *child = NULL;

    int procNum = 0;

    for (int i=0; i<item->childCount(); i++)
    {
        child = item->child(i);
        if ( child->type() == RevTree::PROC_TYPE )
        {
            item->setText(++procNum, child->text(0) );
        }
    }

    //clear the rest
    for (int i=procNum+1; i<columnCount(); i++)
        item->setText(i, "");
}

void RevTree::requestRemoveChildProcess(QTreeWidgetItem *item, int t)
{
    int childProcSeen=0;

    QTreeWidgetItem *child = NULL;
    for (int i=0; i<item->childCount(); i++)
    {
        child = item->child(i);
        if ( child->type() == RevTree::PROC_TYPE )
        {
            if ( t==childProcSeen )
                requestRemoveProcess(procMapRight[child]);
            ++childProcSeen;
        }
    }
}

int RevTree::getTreeLevel(QTreeWidgetItem *item)
{
    int i;
    while( item->parent() ) {
        item = item->parent();
        i++;
    }
    return i;
}

int RevTree::getMaxTreeLevel()
{
    int maxLevel = 0;
    QTreeWidgetItem *maxItem;

    QTreeWidgetItemIterator it(this);
    while (*it) {
        if ( getTreeLevel(*it) > maxLevel ) {
            maxLevel = getTreeLevel(*it);
            maxItem = *it;
        }

        ++it;
    }

    return maxLevel;
}

/* adjust the indent according to:
 * 1. available space, 2. depth of visible tree members */
void RevTree::adjustIndent()
{
    int width = columnWidth(0);
    int maxLevel = getMaxTreeLevel();

    setIndentation( width / maxLevel );

}

//TODO: I think this process is similar to add/removeRevision
//maybe we can refactor and use the same
void RevTree::removeProcess(Process *p)
{
    //remove the mapping
    QTreeWidgetItem *item = procMap[p];

    procMap.remove(p);
    procMapRight.remove(item);

    //remove it from the UI
    delete item;

    //update the children in columns
    updateParentRevision(p->rev);
}

/* this is double click */
void RevTree::itemActivate(QTreeWidgetItem *item, int col)
{
    if ( item->type() == REV_TYPE ) {
        if ( col==0 ) {
            selectedRevision( revMapRight[item] );
            item->setExpanded( !item->isExpanded() );
        }
        else
            requestRemoveChildProcess(item, col-1);
    }
    else if ( item->type() == PROC_TYPE )
        requestRemoveProcess( procMapRight[item] );

}

/* this is single click */
void RevTree::itemClick(QTreeWidgetItem *item, int col)
{
    if ( item->type() == REV_TYPE )
        selectedRevision( revMapRight[item] );
}

