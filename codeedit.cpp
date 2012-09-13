#include "codeedit.h"

#include <QWheelEvent>

CodeEdit::CodeEdit(QWidget *parent) :
    QTextEdit(parent)
{

}

void CodeEdit::wheelEvent(QWheelEvent *ev)
{
    ev->accept();

    if ( ev->modifiers() & Qt::ControlModifier ) {

        ev->accept();
        if ( ev->delta() > 0 )
            zoomIn( 1 );
        else if ( ev->delta() < 0 )
            zoomOut( 1 );
    }
    else
        ev->ignore();

    QTextEdit::wheelEvent(ev);
}
