#include "codeedit.h"

#include <QWheelEvent>

#include <iostream>

CodeEdit::CodeEdit(QWidget *parent) :
    QTextEdit(parent)
{
    QFont fixedFont("Courier", 10);
    setFont( fixedFont );

}

void CodeEdit::wheelEvent(QWheelEvent *ev)
{
    if ( ev->modifiers() & Qt::ControlModifier ) {

        ev->accept();
        if ( ev->delta() > 0 )
            zoomIn( 1 );
        else if ( ev->delta() < 0 )
            zoomOut( 1 );

        ensureCursorVisible();
    }
    else
        QTextEdit::wheelEvent(ev);

}

/*
void CodeEdit::inputMethodEvent(QInputMethodEvent *ev)
{
    std::cout << "input Method Event" << std::endl;
    QTextEdit::inputMethodEvent(ev);
}
*/
