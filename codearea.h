#ifndef CODEAREA_H
#define CODEAREA_H

#include <QMdiArea>

#include <QWheelEvent>

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
    
    void addCodeWindow(Revision *r, QString fileText, int cursorPos=0, int fontSize=-1);

    int fontSize; //global font size

protected:
    void wheelEvent(QWheelEvent *e);

signals:
    void notifyNewRevision(Revision *r);
    void selectedRevision(Revision *r);
    void updateZoom(int fontSize);
    
public slots:
    void selectRevision(Revision *r);

    //necessary so selection changes can be forwarded from mdiArea
    void fireSelectRevision(QMdiSubWindow *sub);

private:
    QMap<Revision *,QMdiSubWindow *> subWindowMap;
    QMap<QMdiSubWindow *, Revision *> subWindowMap2;


    
};

#endif // CODEAREA_H
