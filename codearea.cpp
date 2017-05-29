#include "codearea.h"
#include <QMdiArea>
#include <QAction>
#include "codeedit.h"
#include <QMdiSubWindow>
#include "superwordhighlighter.h"
#include "chuckhighlighter.h"

#include <iostream>

CodeArea::CodeArea(QWidget *parent) :
    QMdiArea(parent), fontSize(10) //TODO make a const
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

void CodeArea::addCodeWindow(Revision *r, QString fileText,
                             int cursorPos, int fontSize)
{
    //setup the widget
    CodeEdit *edit = new CodeEdit(this);
    edit->setPlainText( fileText );
    edit->rev = r;

    //set highlighter no matter what language, dirty hack
    //SuperWordHighlighter *wh =
    //        new SuperWordHighlighter(edit->document());

    if ( r->srcLang == Revision::SrcLangChuck )
    {
        ChuckHighlighter *ch =
                new ChuckHighlighter(edit->document());
    }

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

    if ( fontSize > 0 )
    {
        QFont f = edit->font();
        f.setPointSize(fontSize);
        edit->setFont(f);
    }

    notifyNewRevision(r);

    //listen changes
    edit->listenChanges();

    //edit->focusWidget();
    setActiveSubWindow( subWindow );

    //listen to zoom changes
    connect( this, SIGNAL(updateZoom(int)), edit, SLOT(onZoomChanged(int)) );

}

void CodeArea::wheelEvent(QWheelEvent *ev)
{
    if ( ev->modifiers() & Qt::ControlModifier ) {

        ev->accept();
        if ( ev->delta() > 0 )
        {
            ++fontSize;
            if (fontSize > 30)
                fontSize=30;

            updateZoom(fontSize);
        }
        else if ( ev->delta() < 0 )
        {
            --fontSize;
            if (fontSize < 6)
                fontSize=6;

            updateZoom(fontSize);
        }

    } else
        QMdiArea::wheelEvent(ev);
}

/*
void CodeArea::wheelEvent(QWheelEvent *ev)
{
    qDebug( "wheel event in CodeArea" );

    if ( ev->modifiers() & Qt::ControlModifier ) {

        ev->accept();
        if ( ev->delta() > 0 )
            ++fontSize;
        else if ( ev->delta() < 0 )
            --fontSize;

        //ensureCursorVisible();
    }
    else
        QMdiArea::wheelEvent(ev);
}
*/

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
