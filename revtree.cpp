#include "revtree.h"

RevTree::RevTree(QWidget *parent) :
    QTreeWidget(parent)
{
    setColumnCount(2);
}

void RevTree::addRevision(Revision &r)
{
    QStringList str;
    str << r.getDisplayName();
    str << QString::number( r.getID() );
    addTopLevelItem(new QTreeWidgetItem(str));
}

void RevTree::removeRevision(Revision &r)
{
}

void RevTree::selectRevision(Revision &r)
{
}

void RevTree::addProcess(Process &p)
{
}

void RevTree::removeProcess(Process &p)
{
}


