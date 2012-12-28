#include "revision.h"

#include <QHash>

//declare the static object
QMap<QString, int> Revision::maxPathRevision;

const int Revision::SRCLANG_CHUCK = 1;
const int Revision::SRCLANG_SC = 2;
const int Revision::SRCLANG_UNKNOWN = 3;

/* A new revision here */
Revision::Revision(QString srcFilePath) :
    parent(NULL),
    srcFilePath(srcFilePath),
    textChangedSinceSave(false),
    revNum(0),
    hasShredded(false),
    hasSaved(false),
    srcLang( chooseSrcLang(srcFilePath) )
{
    //assume no other revisions from this src path
    maxPathRevision[ srcFilePath ] = 0;
}

/* branch a previous revision */
Revision::Revision(Revision *prev) :
    parent(prev),
    srcFilePath( prev->srcFilePath ),
    textChangedSinceSave( true ),
    revNum( ++maxPathRevision[ srcFilePath ] ),
    hasShredded( false ),
    hasSaved(false),
    srcLang( prev->srcLang ) //srcLang is the same
{
    parent->addChild(this);
}

QString Revision::getBufferName()
{
    QString ret;

    QFileInfo info(srcFilePath);

    if ( revNum > 0 ) {
        ret = info.completeBaseName() + "." +
                QString::number( revNum ) +  "." +
                info.suffix();
    }
    else {
        ret = info.fileName();
    }

    return ret;
}

/* a superabbreviated name that cuts the base name */
QString Revision::getShortName()
{
    QString ret;

    QFileInfo info(srcFilePath);

    if ( revNum > 0 ) {
        ret = "_." +
              QString::number( revNum ) +  "." +
              info.suffix();
    }
    else {
        ret = info.fileName();
    }

    return ret;
}

QString Revision::getDisplayName()
{
    QString ret = getBufferName();

    if ( textChangedSinceSave )
        ret += "*";

    return ret;
}

QString Revision::getSavePath()
{
    QFileInfo info(srcFilePath);

    return info.path() + "/" + getBufferName();
}

QString Revision::getLastSavedPath()
{
    if ( hasSaved )
        return getSavePath();
    else
        return srcFilePath;
}

/* get a unique id for this revision,
  derived from the path + the buffer name */
int Revision::getID()
{
    return qHash(getSavePath());
}

/* r is one of the roots of the forest */
QList<Revision *> & Revision::getSpine(QList<Revision *> &spine)
{
    spine << this;

    int childCount = -1;
    Revision *child;

    int maxCount = -1;
    Revision *maxChild = NULL;

    if ( children.size() <= 0 )
        return spine;

    foreach( child, children )
    {
        childCount = child->countNodes();
        if ( childCount > maxCount ) {
            maxCount = childCount;
            maxChild = child;
        }
    }

    return maxChild->getSpine(spine);
}

/* count nodes in this subtree */
//TODO: could memoise this, but probably not necessary
int Revision::countNodes()
{
    int sum = 1;
    Revision *child;
    foreach( child, children )
        sum += child->countNodes();

    return sum;
}


void Revision::addChild(Revision *child)
{
    children << child;
}

int Revision::chooseSrcLang(QString srcFilePath)
{
    QFileInfo info(srcFilePath);

    if ( info.suffix().compare("ck")==0 ) {
        return SRCLANG_CHUCK;
    }
    else if ( info.suffix().compare("sc")==0 ) {
        return SRCLANG_SC;
    }
    else {
        return SRCLANG_UNKNOWN;
    }
}
