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

    void listenChanges();
    void undoChanges();
    void ignoreChanges();

protected:
    void wheelEvent(QWheelEvent *e);

    //void inputMethodEvent(QInputMethodEvent *ev);
    //void changeEvent(QEvent *e);
    //void keyPressEvent ( QKeyEvent * e );
    
signals:
    
public slots:
    void onTextChanged();

private:
    void updateTitle();

    //save the text after undo, use it to filter textChanged signals
    QString undoText;
    
};

#endif // CODEEDIT_H
