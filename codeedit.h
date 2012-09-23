#ifndef CODEEDIT_H
#define CODEEDIT_H

#include <QTextEdit>

#include "revision.h"

class CodeEdit : public QTextEdit
{
    Q_OBJECT
public:
    CodeEdit(QWidget *parent = 0);

    Revision *rev;

protected:
    void wheelEvent(QWheelEvent *e);
    //void inputMethodEvent(QInputMethodEvent *ev);
    
signals:
    
public slots:
    
};

#endif // CODEEDIT_H
