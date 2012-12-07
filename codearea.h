#ifndef CODEAREA_H
#define CODEAREA_H

#include <QMdiArea>
#include "revision.h"

/* Encapsulates the UI behaviors of the code area (for buffers).
 * Also includes some of the key bindings.
 * Just not the main revision/application logic.
 */

class CodeArea : public QMdiArea
{
    Q_OBJECT
public:
    explicit CodeArea(QWidget *parent = 0);
    
    void addCodeWindow(Revision *r, QString fileText, int cursorPos);

signals:
    void notifyNewRevision(Revision *r);
    
public slots:
    void selectRevision(Revision *r);
    void fireSelectRevision(QMdiSubWindow *sub);
    //void onTextChanged();

private:
    QMap<Revision *,QMdiSubWindow *> subWindowMap;
    QMap<QMdiSubWindow *, Revision *> subWindowMap2;
    
};

#endif // CODEAREA_H
