#ifndef REVISION_H
#define REVISION_H

#include <QString>
#include <QList>
#include <QFileInfo>
#include <QMap>

/* Models a revision of a code file. */

class Revision
{

public:
    Revision (QString srcFilePath); //new
    Revision (Revision *prev); //branch

    static QMap<QString, int> maxPathRevision;

    enum SrcLangType { SrcLangChuck, SrcLangSc, SrcLangUnk };

    static SrcLangType chooseSrcLang(QString srcFilePath);

    QString getDisplayName();
    QString getBufferName();
    QString getSavePath();
    QString getLastSavedPath();
    int getID();

    Revision *parent;
    QList<Revision *> children;

    QString srcFilePath;
    bool textChangedSinceSave;
    int revNum;
    bool hasShredded;
    bool hasSaved;

    SrcLangType srcLang;

    void addChild(Revision *child);
    int countNodes();
    QList<Revision *> &getSpine(QList<Revision *> &);

    QString getShortName();
};

#endif // REVISION_H
