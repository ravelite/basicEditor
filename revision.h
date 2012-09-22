#ifndef REVISION_H
#define REVISION_H

#include <QString>
#include <QList>
#include <QFileInfo>

/* Models a revision of a code file. */

class Revision
{

public:
    Revision (QString srcFilePath); //new
    Revision (Revision &prev, int revNum); //branch

    QString getDisplayName();
    QString getBufferName();
    QString getSavePath();
    QString getLastSavedPath();
    int getID();

    QString srcFilePath;
    bool textChangedSinceSave;
    int revNum;
    bool hasShredded;
    bool hasSaved;

};

#endif // REVISION_H
