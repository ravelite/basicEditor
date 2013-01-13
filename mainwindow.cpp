#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMdiArea>
#include <QFileDialog>
#include <QMdiSubWindow>
#include <QPushButton>
#include <QAction>
#include <QDockWidget>
#include <QScrollArea>
#include <QGridLayout>
#include <QDateTime>

#include <QTextEdit>
#include "codeedit.h"
#include <iostream>

#include "revtree.h"
#include "codearea.h"

//TODO: separate appLogic with GUI
//TODO: move most of the subwindow logic into subclass of mdiArea

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    codeArea( new CodeArea(this) ),
    engine( new Engine(this )),
    seqRequest( 0 ), nBuffers( 0 )
{
    ui->setupUi(this);

    this->setCentralWidget(codeArea);

    shredTree = new RevTree();
    //treevis = new TreeVis(engine);

    //connect shredTree for delete messages
    connect( shredTree, SIGNAL(requestRemoveProcess(Process*)),
             engine, SLOT(killProcess(Process*)) );

    //make codeWindows respond to shredTree selection
    connect( shredTree, SIGNAL(selectedRevision(Revision*)),
             codeArea, SLOT(selectRevision(Revision*)) );

    //when revisions created by codeArea, add them
    connect( codeArea, SIGNAL(notifyNewRevision(Revision*)),
             this, SLOT(addRevisionMain(Revision*)) );

    //when selection changes in codeArea, change in shredTree
    connect( codeArea, SIGNAL(selectedRevision(Revision*)),
             shredTree, SLOT(selectRevision(Revision*)) );

    //when processes added or removed from engine
    connect( engine, SIGNAL(notifyNewProcess(Process*)),
             shredTree, SLOT(addProcess(Process*)) );
    connect( engine, SIGNAL(notifyRemoveProcess(Process*)),
             shredTree, SLOT(removeProcess(Process*)) );

    //when revisions added to engine
    connect( engine, SIGNAL(revisionAdded(Revision*)),
             shredTree, SLOT(addRevision(Revision*)) );

    QDockWidget *dock = new QDockWidget();
    dock->setWidget( shredTree );
    addDockWidget( Qt::RightDockWidgetArea, dock );

    /*
    QDockWidget *dock2 = new QDockWidget();
    dock2->setWidget( treevis );
    addDockWidget( Qt::BottomDockWidgetArea, dock2 );
    dock2->setFloating( true );
    */

    resize(800,500);

    engine->sendTestMessage();
    createSessionDirectory();
    loadMacros();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete codeArea;
}

void MainWindow::createSessionDirectory()
{
    //create a session directory
    QDir dir;
    sessionName =
       "session" + QDateTime::currentDateTime().toString( Qt::ISODate ).replace(':','.');
    if ( !dir.mkdir( sessionName ) )
    {
        std::cerr << "Problems creating session directory." << std::endl;
    }
}

void MainWindow::addRevisionMain(Revision *r)
{
    engine->addRevision(r);
}

void MainWindow::on_actionOpen_triggered()
{
    QString filePath = QFileDialog::getOpenFileName(this,
        tr("Open File"), "", tr("Chuck Files (*.ck)"));

    if ( filePath.isNull() ) return;

    QFile textFile(filePath);
    if (!textFile.open(QIODevice::ReadOnly | QIODevice::Text))
         return;

    QTextStream stream( &textFile );
    QString fileText = stream.readAll();

    Revision *r = new Revision( filePath );
    codeArea->addCodeWindow(r, fileText, 0);

    //engine notifies views that a revision has been added
    engine->addRevision(r);
}

bool MainWindow::saveFile(QString filePath, QString textContent)
{
    QFile file( filePath );
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream outStream(&file);
    outStream << textContent;

    file.close();

    return true;
}

void MainWindow::on_actionAdd_Shred_triggered()
{
    //get the text from the active buffer (but also the filename)
    //CodeEdit *edit = (CodeEdit *) codeArea->focusWidget();

    /* ATTN: doesn't quite work, order doesn't seem to change correctly
    //get the top stacked window
    QMdiSubWindow *sub = codeArea->subWindowList().last();
    CodeEdit *edit = (CodeEdit *) sub->widget();
    */

    //TODO: refactor, this seems to work as long as we keep a subwindow active
    QMdiSubWindow *sub = codeArea->activeSubWindow();

    if ( sub == NULL ) return;

    CodeEdit *edit = (CodeEdit *) sub->widget();

    if ( edit == NULL ) return;

    QString sessionRel = sessionName + "/" + edit->rev->getBufferName();

    QString progText = applyMacros( edit->toPlainText() );

    //strategy: save a copy in session directory
    if ( saveFile( sessionRel, progText ) ) {

        QFileInfo info(sessionRel);
        QString filePath = info.absoluteFilePath();

#ifdef WIN32
        //truncate the drive specifier from string
        filePath = filePath.split(':').at(1);
#endif

        engine->shredFile( filePath, edit->rev->getID() );
    }

}

void MainWindow::on_actionSave_triggered()
{
    QMdiSubWindow *sub = codeArea->currentSubWindow();

    if ( sub==NULL ) return;

    CodeEdit *edit = (CodeEdit *) sub->widget();

    if ( saveFile(edit->rev->getSavePath(), edit->toPlainText()) )
    {
        edit->rev->textChangedSinceSave = false;
        edit->rev->hasSaved = true;
        sub->setWindowTitle( edit->rev->getDisplayName() );
    }
}

void MainWindow::on_actionNew_triggered()
{
    /* search for nonextant file for the new file */
    int seqNum = 0;
    QString filePath;
    QFileInfo info;
    do {
        filePath = QString("file%1.ck").arg(QString::number(++seqNum));
        info.setFile(filePath);
    } while( info.exists() );

    Revision *r = new Revision( filePath );
    codeArea->addCodeWindow(r, "", 0);

    //engine notifies views that a revision has been added
    engine->addRevision(r);
}
