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

#define PORT_SEND 7000
#define PORT_RECV 7001
#define OUTPUT_BUFFER_SIZE 1024
#define ADDRESS "127.0.0.1"

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

    //set up the shredTree widget
    QGridLayout *grid = new QGridLayout();
    grid->setSpacing(0);
    grid->setContentsMargins(1,1,1,1);
    shredTree = new QWidget();
    shredTree->setLayout( grid );

    QDockWidget *dock = new QDockWidget();
    dock->setWidget( shredTree );
    addDockWidget( Qt::RightDockWidgetArea, dock );

    udpSocket->bind(QHostAddress::LocalHost, PORT_RECV);

    connect(udpSocket, SIGNAL(readyRead()),
            this, SLOT(readPendingDatagrams()));

    sendTestMessage();

    //create a session directory
    QDir dir;
    sessionName =
       "session" + QDateTime::currentDateTime().toString( Qt::ISODate ).replace(':','.');
    if ( !dir.mkdir( sessionName ) )
    {
        std::cerr << "Problems creating session directory." << std::endl;
    }

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

    QFileInfo fileInfo( filePath );

    CodeEdit *edit = new CodeEdit(mdiArea);
    edit->setPlainText( fileText );

    edit->rev = new Revision( filePath );

    maxShredRevision[ filePath ] = 0;

    QMdiSubWindow *subWindow = mdiArea->addSubWindow( edit );
    subWindow->showMaximized();
    subWindow->setWindowTitle( fileInfo.fileName() );

    QPushButton *b1 = new QPushButton( fileInfo.fileName() );
    b1->setMaximumHeight( 40 );
    b1->setProperty( "revID", edit->rev->getID() );

    QGridLayout *gridL = (QGridLayout *)shredTree->layout();
    gridL->addWidget(b1,++nBuffers,0, Qt::AlignLeft|Qt::AlignTop);
    gridL->setRowStretch(nBuffers, 0);

    //connect this to focus changes in mdiArea
    connect( subWindow, SIGNAL(aboutToActivate()),
             b1, SLOT(animateClick()) );

    //also connect buttons to activate subwindows
    connect( b1, SIGNAL(clicked()),
             subWindow, SLOT(setFocus()));

    connect( edit, SIGNAL(textChanged()),
             this, SLOT(onTextChanged()) );

    //add a widget to take up the rest of the space
    QWidget *spacer = new QWidget();
    gridL->addWidget(spacer,nBuffers+1,0,-1,0);
    gridL->setRowStretch(nBuffers+1, 1000);
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

void MainWindow::shredFile(QString filePath, int revID) {

    char buffer[OUTPUT_BUFFER_SIZE];
    osc::OutboundPacketStream p( buffer, OUTPUT_BUFFER_SIZE );

    //for now, just use the source file path for hash
    p << osc::BeginMessage( "/shred/new" ) <<
         filePath.toAscii().constData() <<
         revID << osc::EndMessage;

    outSocket->writeDatagram( p.Data(), p.Size(), QHostAddress::LocalHost, PORT_SEND );
}

void MainWindow::on_actionAdd_Shred_triggered()
{
    //get the text from the active buffer (but also the filename)
    CodeEdit *edit = (CodeEdit *) mdiArea->focusWidget();

    //QString filePath = edit->rev->getLastSavedPath();

    QString sessionRel = sessionName + "/" + edit->rev->getBufferName();

    //strategy: save a copy in session directory
    if ( saveFile( sessionRel, edit->toPlainText() ) ) {

        QFileInfo info(sessionRel);
        QString filePath = info.absoluteFilePath();

#ifdef WIN32
        //truncate the drive specifier from string
        filePath = filePath.split(':').at(1);
#endif

        shredFile( filePath, edit->rev->getID() );
    }

//#ifdef WIN32
//    //truncate the drive specifier from string
//    filePath = filePath.split(':').at(1);
//#endif

    //shredFile( filePath, edit->rev->getID() );
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

                QGridLayout *gridL = (QGridLayout *)shredTree->layout();

                for (int i=0; i<gridL->count(); i++) {
                    QWidget *w = gridL->itemAt(i)->widget();
                    QPushButton *b = qobject_cast<QPushButton *>(w);

                    if ( b==NULL ) continue;

                    if ( b->property("revID").toInt()==edShrid ) {
                        std::cout << "found a match" << std::endl;

                        int row, column, rowSpan, columnSpan;
                        gridL->getItemPosition( i, &row, &column, &rowSpan, &columnSpan );

                        //add a shred button at the next open position
                        int col = 1;
                        while( gridL->itemAtPosition(row,col) != 0 )
                            col++;

                        QPushButton *bS = new QPushButton( QString::number(shrid) );
                        bS->setMaximumWidth( 20 );
                        gridL->addWidget( bS, row, col, 1, 1 );

                        connect( bS, SIGNAL(clicked()),
                                 this, SLOT(killShred()));
                    }

                } //for all buttons in the grid

                QList<QMdiSubWindow *> windowList = mdiArea->subWindowList();
                QList<QMdiSubWindow *>::iterator i;
                for (i = windowList.begin(); i != windowList.end(); ++i) {

                    CodeEdit *edit = (CodeEdit *) (*i)->widget();

                    if ( edit->rev->getID()==edShrid ) {

                        edit->rev->hasShredded = true;

                        break;
                    }

                }

            } else if ( strcmp( m.AddressPattern(), "/shred/remove") ==0 ) {

                osc::int32 shrid;
                m.ArgumentStream() >> shrid >> osc::EndMessage;

                std::cout << "/shred/remove," << shrid << std::endl;

                QGridLayout *gridL = (QGridLayout *)shredTree->layout();

                for (int i=0; i<gridL->count(); i++) {
                    QWidget *w = gridL->itemAt(i)->widget();
                    QPushButton *b = qobject_cast<QPushButton *>(w);

                    if ( b==NULL ) continue;

                    int buttonShrid = b->text().toInt();

                    if ( shrid == buttonShrid ) {
                        gridL->removeWidget(b);
                        delete b;

                        gridL->invalidate();
                        //gridL->layout();
                    }

                } //for all buttons in the grid


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

void MainWindow::killShred()
{
    QPushButton *b1 = (QPushButton *) QObject::sender();
    osc::int32 shrid = b1->text().toInt();

    char buffer[OUTPUT_BUFFER_SIZE];
    osc::OutboundPacketStream p( buffer, OUTPUT_BUFFER_SIZE );

    p << osc::BeginMessage( "/shred/remove" ) <<
            shrid << osc::EndMessage;

    //outSocket.Send( p.Data(), p.Size() );
    outSocket->writeDatagram( p.Data(), p.Size(), QHostAddress::LocalHost, PORT_SEND );

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
    }

    //if hasShredded and text changed (now!) make a new revision
    if ( edit->rev->hasShredded ){

        QString filePath = edit->rev->srcFilePath;

        CodeEdit *edit2 = new CodeEdit(mdiArea);
        edit2->setPlainText( edit->toPlainText() );

        QTextCursor cursor = edit2->textCursor();
        cursor.setPosition( edit->textCursor().position() );
        edit2->setTextCursor( cursor );

        int thisRev = ++maxShredRevision[ filePath ];

        edit2->rev = new Revision( *(edit->rev), thisRev );

        QMdiSubWindow *subWindow = mdiArea->addSubWindow( edit2 );
        subWindow->showMaximized();
        subWindow->setWindowTitle( edit2->rev->getDisplayName() );

        QPushButton *b1 = new QPushButton( edit2->rev->getBufferName() );
        b1->setMaximumHeight( 40 );
        b1->setProperty( "revID", edit2->rev->getID() );

        QGridLayout *gridL = (QGridLayout *)shredTree->layout();
        gridL->addWidget(b1,++nBuffers,0, Qt::AlignLeft|Qt::AlignTop);
        gridL->setRowStretch(nBuffers, 0);

        //connect this to focus changes in mdiArea
        connect( subWindow, SIGNAL(aboutToActivate()),
                 b1, SLOT(animateClick()) );

        //also connect buttons to activate subwindows
        connect( b1, SIGNAL(clicked()),
                 subWindow, SLOT(setFocus()));

        connect( edit2, SIGNAL(textChanged()),
                 this, SLOT(onTextChanged()) );

        //add a widget to take up the rest of the space
        QWidget *spacer = new QWidget();
        gridL->addWidget(spacer,nBuffers+1,0,-1,0);
        gridL->setRowStretch(nBuffers+1, 1000);
    }

}
