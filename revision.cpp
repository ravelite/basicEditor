#include "revision.h"

#include <QHash>

/* A new revision here */
Revision::Revision(QString srcFilePath) :
    srcFilePath(srcFilePath),
    textChangedSinceSave(false),
    revNum(0),
    hasShredded(false),
    hasSaved(false)
{
}

/* branch a previous revision */
Revision::Revision(Revision &prev, int revNum) :
    srcFilePath( prev.srcFilePath ),
    textChangedSinceSave( true ),
    revNum( revNum ),
    hasShredded( false ),
    hasSaved(false)
{
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

