#include "treevis.h"

TreeVis::TreeVis(Engine *eng, QWidget *parent) :
    QWidget(parent), engine(eng),
    unitSize(4), alpha(2)
{
    //for now, disable this code
    /*
    connect( engine, SIGNAL(revisionAdded(Revision*)),
             this, SLOT(treeChanged()) );
             */
}

void TreeVis::treeChanged()
{
    doLayout();
}

void TreeVis::doLayout()
{
    isSpinal.empty();

    //prelim: construct spines and label spinal nodes
    QList<Revision *> roots = engine->getRoots();

    Revision *root;
    foreach( root, roots )
    {
        QList<Revision *> spine;
        root->getSpine( spine );

        Revision *spinalNode;
        foreach( spinalNode, spine )
            isSpinal[spinalNode] = true;
    }

    //mark non-spinal nodes
    Revision *rev;
    foreach( rev, engine->getRevisions() )
    {
        if ( !isSpinal.contains(rev) )
            isSpinal[rev] = false;
    }

    //algorithm: layout each root with recursive alg.
    foreach( root, roots )
    {
        doLayoutRec(root);
    }

    //now arrange them relatively
}

/* alg 5.8.3 Ordered Trees from trees.pdf */
void TreeVis::doLayoutRec(Revision *rev)
{
    QList<Revision *> spine;
    rev->getSpine( spine );

    bool childPassedSpine = false;
    bool grandPassedSpine = false;

    QList<Revision *> leftChildSubtrees;
    QList<Revision *> rightChildSubtrees;
    QList<Revision *> leftGrandSubtrees;
    QList<Revision *> rightGrandSubtrees;

    //1. recursively layout non-spine children of root, As
    Revision *child;
    foreach( child, rev->children )
        if ( !isSpinal[child] ) {
            doLayoutRec(child);
            childPassedSpine ? rightChildSubtrees << child
                             : leftChildSubtrees << child;
        }
        else
            childPassedSpine = true;

    //1b. recursively layout non-spine children of next spine node, Bs
    if ( spine.size() > 2 ) {
        foreach( child, spine[1]->children )
            if ( !isSpinal[child] ) {
                doLayoutRec(child);
                grandPassedSpine ? rightGrandSubtrees << child
                                 : leftGrandSubtrees << child;
            }
            else
                grandPassedSpine = true;
    }

    //2. place v0 at the origen
    layout[rev] = QRect(0,0,0,0);

    //3. Place D(Ai+1),...,D(Ap) one above the other at unit vertical separations from each other,
    QList<Revision *>::const_iterator it = NULL;
    QList<Revision *>::const_iterator prev = NULL;
    for( it=rightChildSubtrees.constEnd(); it>=rightChildSubtrees.constBegin(); it-- )
    {
        if ( it==rightChildSubtrees.constEnd() ) {
            //last subtree goes under root
            layout[*it].moveTo( layout[rev].right() + unitSize,
                                layout[rev].top() + unitSize*alpha );
        }
        else {
            layout[*it].moveTo( layout[rev].right() + unitSize,
                                layout[*prev].bottom() + unitSize );
        }
        prev=it;
    }

    //4. Place D(Bj+1),...,D(Br) one above the other at unit vertical separations from each other
    for( it=rightGrandSubtrees.constEnd(); it>=rightGrandSubtrees.constBegin(); it-- )
    {
        if ( it==rightGrandSubtrees.constEnd() ) {
            if ( prev!=NULL )
                //last subtree goes under last subtree
                layout[*it].moveTo( layout[*prev].right() + unitSize,
                                    layout[*prev].bottom() + unitSize );
            else
                //goes under root
                layout[*it].moveTo( layout[rev].right() + unitSize,
                                    layout[rev].top() + unitSize*alpha );
        }
        else {
            layout[*it].moveTo( layout[*prev].right() + unitSize,
                                layout[*prev].bottom() + unitSize );
        }
        prev=it;
    }

    //5. Place v1 in the same horizontal channel as the bottom of D(Bj+1),
    //and one unit to the right of v0.
    Revision *v1 = NULL;
    if ( spine.size() > 2 ) {
        v1 = spine[1];
        if ( prev!=NULL )
            layout[v1].moveTo( layout[*prev].right() + unitSize,
                               layout[*prev].bottom() + unitSize );
        else
            layout[v1].moveTo( layout[rev].right() + unitSize,
                               layout[rev].top() + unitSize*alpha );
    }

    /* 6. Place D(B0), . . . ,D(Bj) one above the other at unit vertical separations from
    each other, such that D(Bj) is at the top, D(B0) is at the bottom, t0, . . . , tj are
    in the same vertical channel, and tj is one unit below, and one unit to the right
    of v1. */
    for ( it=leftGrandSubtrees.constEnd(); it>=leftGrandSubtrees.constBegin(); it-- )
    {
        if ( it==leftGrandSubtrees.constEnd() ) {
            if ( v1!=NULL )
                layout[*it].moveTo( layout[v1].right() + unitSize,
                                    layout[v1].bottom() + unitSize );
            else if ( prev != NULL )
                layout[*it].moveTo( layout[*prev].right() + unitSize,
                                    layout[*prev].bottom() + unitSize );
            else
                layout[*it].moveTo( layout[rev].right() + unitSize,
                                    layout[rev].bottom() + unitSize );
        } else {
            layout[*it].moveTo( layout[*prev].right() + unitSize,
                                layout[*prev].bottom() + unitSize );
        }
        prev=it;
    }

    //7. Place D(A0),...,D(Ai) one above the other at unit vertical separations from each other,
    for ( it=leftChildSubtrees.constEnd(); it>=leftChildSubtrees.constBegin(); it-- )
    {
        if ( it==leftChildSubtrees.constEnd() ) {
            if ( prev != NULL )
                layout[*it].moveTo( layout[*prev].right() + unitSize,
                                    layout[*prev].bottom() + unitSize );
            else
                layout[*it].moveTo( layout[rev].right() + unitSize,
                                    layout[rev].bottom() + unitSize );
        } else {
            layout[*it].moveTo( layout[*prev].right() + unitSize,
                                layout[*prev].bottom() + unitSize );
        }
        prev=it;
    }

    //8. Let β = h(D(B0)) + . . . + h(D(Bj)) + h(D(A0)) + . . . + h(D(Ai)) + i + j +


}


