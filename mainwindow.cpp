#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "osc/OscOutboundPacketStream.h"
#include "osc/OscReceivedElements.h"
#include "osc/OscPacketListener.h"

#include <QMdiArea>
#include <QFileDialog>
#include <QMdiSubWindow>
#include <QPushButton>
#include <QAction>
#include <QDockWidget>
#include <QScrollArea>
#include <QGridLayout>

#include <QTextEdit>
#include <QDateTime>
#include "codeedit.h"
#include <iostream>

#include <QTreeWidgetItem>

#include "revtree.h"

#define PORT_SEND 7000
#define PORT_RECV 7001
#define OUTPUT_BUFFER_SIZE 1024
#define ADDRESS "127.0.0.1"

//TODO: separate appLogic with GUI

//TODO: move most of the subwindow logic into subclass of mdiArea

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    outSocket( new QUdpSocket(this) ),
    udpSocket( new QUdpSocket(this) ),
    mdiArea( new QMdiArea(this) ),
    seqRequest( 0 ), nBuffers( 0 )
{
    ui->setupUi(this);

    QAction *leftAction = new QAction(mdiArea);
    leftAction->setShortcut( tr("Alt+Left") );
    QAction *rightAction = new QAction(mdiArea);
    rightAction->setShortcut( tr("Alt+Right") );

    connect( leftAction, SIGNAL(triggered()),
             mdiArea, SLOT(activatePreviousSubWindow()) );
    connect( rightAction, SIGNAL(triggered()),
             mdiArea, SLOT(activateNextSubWindow()) );

    mdiArea->addAction(leftAction);
    mdiArea->addAction(rightAction);

    this->setCentralWidget(mdiArea);

    //forward selection changes in mdi
    connect( mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),
             this, SLOT(fireSelectRevision(QMdiSubWindow*)) );

    shredTree = new RevTree();

    //connect shredTree for delete messages
    connect( shredTree, SIGNAL(requestRemoveProcess(Process*)),
             this, SLOT(killProcess(Process*)) );

    //make codeWindows respond to shredTree selection
    connect( shredTree, SIGNAL(selectedRevision(Revision*)),
             this, SLOT(selectRevision(Revision*)) );

    QDockWidget *dock = new QDockWidget();
    dock->setWidget( shredTree );
    addDockWidget( Qt::RightDockWidgetArea, dock );

    udpSocket->bind(QHostAddress::LocalHost, PORT_RECV);

    connect(udpSocket, SIGNAL(readyRead()),
            this, SLOT(readPendingDatagrams()));

    sendTestMessage();
    createSessionDirectory();
    loadMacros();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete mdiArea;
    delete udpSocket;
    delete outSocket;
}

void MainWindow::sendTestMessage() {

    //and send a test/wakeup message
    char buffer[OUTPUT_BUFFER_SIZE];
    osc::OutboundPacketStream p( buffer, OUTPUT_BUFFER_SIZE );

    p << osc::BeginMessage( "/hello" ) <<
            "unaudicle is awake" << osc::EndMessage;

    outSocket->writeDatagram( p.Data(), p.Size(), QHostAddress::LocalHost, PORT_SEND );
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

void MainWindow::addCodeWindow(Revision *r, QString fileText, int cursorPos = 0)
{
    CodeEdit *edit = new CodeEdit(mdiArea);
    edit->setPlainText( fileText );

    edit->rev = r;
    revisions << edit->rev;

    QMdiSubWindow *subWindow = mdiArea->addSubWindow( edit );
    subWindow->showMaximized();
    subWindow->setWindowTitle( edit->rev->getBufferName() );

    shredTree->addRevision( edit->rev );

    //add to subwindow maps
    subWindowMap[r] = subWindow;
    subWindowMap2[subWindow] = r;

    if ( cursorPos > 0 )
    {
        QTextCursor cursor = edit->textCursor();
        cursor.setPosition( cursorPos );
        edit->setTextCursor( cursor );
    }

    connect( edit, SIGNAL(textChanged()),
             this, SLOT(onTextChanged()) );
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

    addCodeWindow(r, fileText);
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
    CodeEdit *edit = (CodeEdit *) mdiArea->focusWidget();

    //QString filePath = edit->rev->getLastSavedPath();

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

        shredFile( filePath, edit->rev->getID() );
    }

}

void MainWindow::shredFile(QString filePath, int revID) {

    char buffer[OUTPUT_BUFFER_SIZE];
    osc::OutboundPacketStream p( buffer, OUTPUT_BUFFER_SIZE );

    //for now, just use the source file path for hash
    p << osc::BeginMessage( "/shred/new" ) <<
         filePath.toAscii().constData() <<
         revID << osc::EndMessage;

    outSocket->writeDatagram( p.Data(), p.Size(), QHostAddress::LocalHost, PORT_SEND );
}

void MainWindow::readPendingDatagrams()
{
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        udpSocket->readDatagram(datagram.data(), datagram.size(),
                                &sender, &senderPort);

        //process the datagram
        osc::ReceivedMessage m( osc::ReceivedPacket( datagram.data(), datagram.size()));

        try{
            if ( strcmp( m.AddressPattern(), "/hello" )==0 ) {

                //ignore the arguments, print some message
                std::cout << "got /hello message!" << std::endl;

            } else if ( strcmp( m.AddressPattern(), "/shred/new") ==0 ) {

                osc::int32 edShrid;
                osc::int32 shrid;
                m.ArgumentStream() >> edShrid >> shrid >> osc::EndMessage;

                std::cout << "/shred/new," << edShrid << "," << shrid << std::endl;

                //go through all revisions looking for revID
                QList<Revision *>::const_iterator curr;
                for (curr=revisions.constBegin(); curr!=revisions.constEnd();
                     ++curr)
                {
                    Revision *r = *curr;

                    //std::cout << "Rev " << r->getID() << std::endl;

                    if ( edShrid==r->getID() ) {

                        //std::cout << "Found the revision." << std::endl;

                        //mark revision as shredded
                        r->hasShredded = true;

                        //make a new process and add to display
                        Process *p = new Process(r, shrid);

                        shredTree->addProcess(p);

                        /*
                        QString revStr = QString::number( r->getID() );

                        QList<QTreeWidgetItem *> res =
                        shredTree->findItems(revStr, Qt::MatchExactly, 1);

                        if ( res.size() > 0 ) {

                            //std::cout << "Found the widgetItem." << std::endl;

                            QStringList str;
                            str << QString::number( shrid );
                            str << "";
                            QTreeWidgetItem *item =
                                    new QTreeWidgetItem(str, RevTree::PROC_TYPE);
                            res[0]->addChild( item );
                        }*/
                    }
                }

            } else if ( strcmp( m.AddressPattern(), "/shred/remove") ==0 ) {

                osc::int32 shrid;
                m.ArgumentStream() >> shrid >> osc::EndMessage;

                std::cout << "/shred/remove," << shrid << std::endl;

                QString revStr = QString::number( shrid );

                QList<QTreeWidgetItem *> res =
                shredTree->findItems(revStr, Qt::MatchExactly|Qt::MatchRecursive, 0);

                std::cout << "trying to remove: " << shrid << std::endl;
                std::cout << "found items to remove: " << res.size() << std::endl;

                if ( res.size() > 0 ) {
                    //shredTree->removeItemWidget(res[0], 0);

                    //seems evil, but I guess this is the qt way
                    delete res[0];
                }

            } else { //any other message

                std::cout << "got message at " << m.AddressPattern() << std::endl;

            }

        }catch( osc::Exception& e ){
            // any parsing errors such as unexpected argument types, or
            // missing arguments get thrown as exceptions.
            std::cout << "error while parsing message: "
                      << m.AddressPattern() << ": " << e.what() << "\n";
        }
    }
}

void MainWindow::killChuckShred(Process *p)
{
    //copied from above, TODO refactor this
    int shrid = p->id;

    char buffer[OUTPUT_BUFFER_SIZE];
    osc::OutboundPacketStream pstream( buffer, OUTPUT_BUFFER_SIZE );

    pstream << osc::BeginMessage( "/shred/remove" ) <<
            shrid << osc::EndMessage;

    //outSocket.Send( p.Data(), p.Size() );
    outSocket->writeDatagram( pstream.Data(), pstream.Size(),
                              QHostAddress::LocalHost, PORT_SEND );
}

void MainWindow::killProcess(Process *p)
{
    if( p->rev->srcLang == Revision::SRCLANG_CHUCK )
    {
        killChuckShred( p );
    }
    else if ( p->rev->srcLang == Revision::SRCLANG_SC )
    {

    }
    else {

    }

}

void MainWindow::selectRevision(Revision *r)
{
    mdiArea->setActiveSubWindow( subWindowMap[r] );
}

void MainWindow::fireSelectRevision(QMdiSubWindow *sub)
{
    shredTree->selectRevision( subWindowMap2[sub] );
}

void MainWindow::on_actionSave_triggered()
{
    QMdiSubWindow *sub = mdiArea->currentSubWindow();

    if ( sub==NULL ) return;

    CodeEdit *edit = (CodeEdit *) sub->widget();

    if ( saveFile(edit->rev->getSavePath(), edit->toPlainText()) )
    {
        edit->rev->textChangedSinceSave = false;
        edit->rev->hasSaved = true;
        sub->setWindowTitle( edit->rev->getDisplayName() );
    }
}

void MainWindow::onTextChanged()
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

        //new revision
        Revision *r = new Revision( edit->rev );

        addCodeWindow( r, bufferTextChanged, cursorPos );

        //disconnect textChanged() for parent
        disconnect(edit, SIGNAL(textChanged()),
                   this, SLOT(onTextChanged()));
        edit->undo(); //undo previous edit in old buffer
        connect( edit, SIGNAL(textChanged()),
                 this, SLOT(onTextChanged()));

    }

}

void MainWindow::on_actionNew_triggered()
{

}
