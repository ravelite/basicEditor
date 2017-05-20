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

/*
void CodeEdit::wheelEvent(QWheelEvent *ev)
{
    //first, CodeArea should update its fontSize
    //then, update of all of the windows

    CodeArea *area = (CodeArea*) this->parentWidget();

    if ( ev->modifiers() & Qt::ControlModifier ) {

        ev->accept();
        if ( ev->delta() > 0 )
            ++(area->fontSize);
        else if ( ev->delta() < 0 )
            --(area->fontSize);

         onZoomChanged( area->fontSize );

        ensureCursorVisible();
    }
    else
        QTextEdit::wheelEvent(ev);

    QTextEdit::wheelEvent(ev);
}
*/

void CodeEdit::listenChanges()
{
    connect( this, SIGNAL(textChanged()),
             this, SLOT(onTextChanged()) );
}

void CodeEdit::ignoreChanges()
{
    disconnect( this, SIGNAL(textChanged()) );
}

//change tabs to spaces
void CodeEdit::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Tab)
    {
        e->accept();
        this->insertPlainText("   ");
    }
    else {
        QTextEdit::keyPressEvent(e);
    }
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

void CodeEdit::onZoomChanged(int fontSize)
{
    /*int fontDiff = fontSize-fontPointSize();
    if (fontDiff > 0)
        zoomIn(fontDiff);
    if (fontDiff < 0)
        zoomIn(fontDiff);*/

   QFont f = font();
   f.setPointSize(fontSize);
   setFont(f);
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
