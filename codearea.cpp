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

    notifyNewRevision(r);

    edit->focusWidget();

    //listen changes
    edit->listenChanges();

}

//TODO: debug, this seems circular
void CodeArea::selectRevision(Revision *r)
{
    setActiveSubWindow( subWindowMap[r] );
}

void CodeArea::fireSelectRevision(QMdiSubWindow *sub)
{
    selectRevision( subWindowMap2[sub] );
}
