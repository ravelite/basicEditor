#include "codeedit.h"
#include <QWheelEvent>
#include <iostream>

#include <QMdiSubWindow>
#include <QMdiArea>
#include "codearea.h"

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

void CodeEdit::listenChanges()
{
    connect( this, SIGNAL(textChanged()),
             this, SLOT(onTextChanged()) );
}

void CodeEdit::ignoreChanges()
{
    disconnect( this, SIGNAL(textChanged()) );
}

void CodeEdit::undoChanges()
{
    disconnect( this );
    undo(); //undo previous edit in this buffer
}

/*
void CodeEdit::inputMethodEvent(QInputMethodEvent *ev)
{
    std::cout << "input Method Event" << std::endl;
    QTextEdit::inputMethodEvent(ev);
}
*/

/*
void CodeEdit::changeEvent(QEvent *e)
{
    std::cout << "change " << std::endl;
    QTextEdit::changeEvent(e);
}
*/

void CodeEdit::updateTitle()
{
    QMdiSubWindow *sub = (QMdiSubWindow *)this->parent();

    if ( !rev->textChangedSinceSave ) {

        rev->textChangedSinceSave = true;
        sub->setWindowTitle( rev->getDisplayName() );

        //TODO: also update in shredTree UI
    }
}

void CodeEdit::onTextChanged()
{
    QMdiSubWindow *sub = (QMdiSubWindow *)this->parent();

    updateTitle();

    //if hasShredded and text changed (now!) make a new revision
    if ( rev->hasShredded && undoText.compare(toPlainText())!=0 ){

        //get info from the parent revision
        QString bufferTextChanged = toPlainText();
        int cursorPos = textCursor().position();

        //new revision
        Revision *r = new Revision( rev );
        CodeArea *area = (CodeArea *)sub->mdiArea();

        undoChanges();
        listenChanges();

        undoText = toPlainText();

        area->addCodeWindow(r, bufferTextChanged, cursorPos); //make a code window
    }
}

/*
void CodeEdit::keyPressEvent ( QKeyEvent * e ) {
    //std::cout << "key press event" << e->text().toStdString() <<std::endl;

    QTextEdit::keyPressEvent(e);

    if ( textChanged ) {

        QMdiSubWindow *sub = (QMdiSubWindow *)this->parent();

        updateTitle();

        //if hasShredded and text changed (now!) make a new revision
        if ( rev->hasShredded ){

            //get info from the parent revision
            QString bufferTextChanged = toPlainText();
            int cursorPos = textCursor().position();

            //new revision
            Revision *r = new Revision( rev );
            CodeArea *area = (CodeArea *)sub->mdiArea();

            undo();

            area->addCodeWindow(r, bufferTextChanged, cursorPos); //make a code window
        }

        textChanged = false;
    }

}*/
