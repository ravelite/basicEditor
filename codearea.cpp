#include "codearea.h"
#include <QMdiArea>
#include <QAction>
#include "codeedit.h"
#include <QMdiSubWindow>

CodeArea::CodeArea(QWidget *parent) :
    QMdiArea(parent)
{
    //set up left and right actions to shuffle through buffers
    QAction *leftAction = new QAction(this);
    leftAction->setShortcut( tr("Alt+Left") );
    QAction *rightAction = new QAction(this);
    rightAction->setShortcut( tr("Alt+Right") );

    connect( leftAction, SIGNAL(triggered()),
             this, SLOT(activatePreviousSubWindow()) );
    connect( rightAction, SIGNAL(triggered()),
             this, SLOT(activateNextSubWindow()) );

    addAction(leftAction);
    addAction(rightAction);

    //forward selection changes in mdi
    connect( this, SIGNAL(subWindowActivated(QMdiSubWindow*)),
             this, SLOT(fireSelectRevision(QMdiSubWindow*)) );

    //BUG: something missing for selection changes
    //maybe because the onTextChanged is in the other class
}

void CodeArea::addCodeWindow(Revision *r, QString fileText, int cursorPos = 0)
{
    //setup the widget
    CodeEdit *edit = new CodeEdit(this);
    edit->setPlainText( fileText );
    edit->rev = r;

    //setup the subwindow
    QMdiSubWindow *subWindow = addSubWindow( edit );
    subWindow->showMaximized();
    subWindow->setWindowTitle( edit->rev->getBufferName() );

    //add to subwindow maps
    subWindowMap[r] = subWindow;
    subWindowMap2[subWindow] = r;

    //save the position, so that it may be restored
    if ( cursorPos > 0 )
    {
        QTextCursor cursor = edit->textCursor();
        cursor.setPosition( cursorPos );
        edit->setTextCursor( cursor );
    }

    //track changes in this buffer
    connect( edit, SIGNAL(textChanged()),
             this, SLOT(onTextChanged()) );

    //sets the focus after creating new window
    edit->setFocus();
}

void CodeArea::onTextChanged()
{
    CodeEdit *edit = (CodeEdit *) QObject::sender();
    QMdiSubWindow *sub = (QMdiSubWindow *)edit->parent();

    if ( !edit->rev->textChangedSinceSave ) {

        edit->rev->textChangedSinceSave = true;
        sub->setWindowTitle( edit->rev->getDisplayName() );

        //TODO: also update in shredTree UI
    }

    //if hasShredded and text changed (now!) make a new revision
    if ( edit->rev->hasShredded ){

        //get info from the parent revision
        QString bufferTextChanged = edit->toPlainText();
        int cursorPos = edit->textCursor().position();

        //disconnect textChanged() for parent
        disconnect(edit, SIGNAL(textChanged()),
                   this, SLOT(onTextChanged()));
        edit->undo(); //undo previous edit in old buffer
        connect( edit, SIGNAL(textChanged()),
                 this, SLOT(onTextChanged()));

        //new revision
        Revision *r = new Revision( edit->rev );
        addCodeWindow(r, bufferTextChanged, cursorPos); //make a code window
        notifyNewRevision(r);
    }

}

void CodeArea::selectRevision(Revision *r)
{
    setActiveSubWindow( subWindowMap[r] );
}

void CodeArea::fireSelectRevision(QMdiSubWindow *sub)
{
    selectRevision( subWindowMap2[sub] );
}
