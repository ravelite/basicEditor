#ifndef REVISION_H
#define REVISION_H

#include <QString>
#include <QList>

/* Models a revision of a code file.
  Has at least a name, derived from the filename,
  as well as the text of the code,
  as well as some shreds associated with it. */

//struct shred {
//};

class Revision
{

public:
    Revision(QString name, QString code);

    QString name;
    QString code;
    QList<int> shreds;
};

#endif // REVISION_H
