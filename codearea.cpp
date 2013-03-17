#include "codearea.h"
#include <QMdiArea>
#include <QAction>
#include "codeedit.h"
#include <QMdiSubWindow>
#include "superwordhighlighter.h"

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

    //setup subWindowList() to return stacking order
    //this is to facilitate always getting top window for execution
    setActivationOrder( QMdiArea::StackingOrder );
}

void CodeArea::addCodeWindow(Revision *r, QString fileText, int cursorPos = 0)
{
    //setup the widget
    CodeEdit *edit = new CodeEdit(this);
    edit->setPlainText( fileText );
    edit->rev = r;

    //set highlighter no matter what language, dirty hack
    SuperWordHighlighter *wh =
            new SuperWordHighlighter(edit->document());

    //setup the subwindow
    QMdiSubWindow *subWindow = addSubWindow( edit );
    subWindow->showMaximized();
    subWindow->setWindowTitle( edit->rev->getBufferName() );

    //don't delete windows when closed
    subWindow->setAttribute(Qt::WA_DeleteOnClose, false);

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

    notifyNewRevision(r);

    //listen changes
    edit->listenChanges();

    //edit->focusWidget();
    setActiveSubWindow( subWindow );

}

//TODO: debug, this seems circular, maybe not necessary
void CodeArea::selectRevision(Revision *r)
{
    QMdiSubWindow *sub = subWindowMap[r];

    //if we closed the window and it is invisible, make it visible
    if ( sub && !sub->isVisible() )
    {
        sub->show(); sub->widget()->show();
    }

    setActiveSubWindow( sub );
}

void CodeArea::fireSelectRevision(QMdiSubWindow *sub)
{
    selectRevision( subWindowMap2[sub] );

    //also notify other views
    selectedRevision( subWindowMap2[sub] );
}
