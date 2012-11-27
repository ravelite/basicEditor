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
#include <QDateTime>

#include <QTextEdit>
#include "codeedit.h"
#include <iostream>

#include "revtree.h"
#include "codearea.h"

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
    codeArea( new CodeArea(this) ),
    seqRequest( 0 ), nBuffers( 0 )
{
    ui->setupUi(this);

    this->setCentralWidget(codeArea);

    shredTree = new RevTree();

    //connect shredTree for delete messages
    connect( shredTree, SIGNAL(requestRemoveProcess(Process*)),
             this, SLOT(killProcess(Process*)) );

    //make codeWindows respond to shredTree selection
    connect( shredTree, SIGNAL(selectedRevision(Revision*)),
             codeArea, SLOT(selectRevision(Revision*)) );

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
    delete codeArea;
    delete udpSocket;
    delete outSocket;
}

//TODO: this should go to a separate engine file
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

void MainWindow::addRevisionMain(Revision *r, QString fileText, int cursorPos)
{
    revisions << r; //track the new revision
    codeArea->addCodeWindow(r, fileText, cursorPos); //make a code window
    shredTree->addRevision(r); //add to tree UI
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
    addRevisionMain(r, fileText, 0);
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
    CodeEdit *edit = (CodeEdit *) codeArea->focusWidget();

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

Revision * MainWindow::findRevision(int revId) {
    Revision *curr;
    foreach (curr, revisions) {
        if ( curr->getID() == revId )
            return curr;
    }
    //in case we don't find anything
    return NULL;
}

Process * MainWindow::findProcess(int procId, int srcLang) {
    Process *curr;
    foreach (curr, processes) {
        if ( curr->id == procId &&
             curr->rev->srcLang == srcLang )
            return curr;
    }
    //in case we don't find anything
    return NULL;
}

void MainWindow::readPendingDatagrams()
{
    //TODO: make a handler for the bodies under here
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

                Revision *r = findRevision( edShrid );
                if (r!=NULL) {

                    //mark revision as shredded
                    r->hasShredded = true;

                    //make a new process and add to display
                    Process *p = new Process(r, shrid);

                    processes << p; //keep track of it

                    shredTree->addProcess(p);
                }

            } else if ( strcmp( m.AddressPattern(), "/shred/remove") ==0 ) {

                osc::int32 shrid;
                m.ArgumentStream() >> shrid >> osc::EndMessage;

                std::cout << "/shred/remove," << shrid << std::endl;

                Process *p = findProcess(shrid, Revision::SRCLANG_CHUCK);
                if ( p!=NULL ) {
                    processes.removeOne(p);

                    shredTree->removeProcess(p);
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
        addRevisionMain(r, bufferTextChanged, cursorPos);

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
