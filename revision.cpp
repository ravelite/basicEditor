#include "revision.h"

#include <QHash>

//declare the static object
QMap<QString, int> Revision::maxPathRevision;

/* A new revision here */
Revision::Revision(QString srcFilePath) :
    parent(NULL),
    srcFilePath(srcFilePath),
    textChangedSinceSave(false),
    revNum(0),
    hasShredded(false),
    hasSaved(false)
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
    hasSaved(false)
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

void Revision::addChild(Revision *child)
{
    children << child;
}
