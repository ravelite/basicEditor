#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "osc/OscOutboundPacketStream.h"
#include "osc/OscReceivedElements.h"
#include "osc/OscPacketListener.h"
#include "ip/UdpSocket.h"

#include <QtConcurrentRun>
#include <QMdiArea>
#include <QFileDialog>
#include <QPlainTextEdit>
#include <QMdiSubWindow>
#include <QPushButton>
#include <QAction>
#include <QDockWidget>
#include <QScrollArea>

#include <QHBoxLayout>
#include <QTextEdit>

#include "codeedit.h"

#include <iostream>

#define PORT_SEND 7000
#define PORT_RECV 7001
#define OUTPUT_BUFFER_SIZE 1024
#define ADDRESS "127.0.0.1"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    //listener(),
    //open up a receiving port
    //inSocket( new UdpListeningReceiveSocket( IpEndpointName( IpEndpointName::ANY_ADDRESS, PORT_RECV ),
    //          &listener )),
    outSocket( IpEndpointName( ADDRESS, PORT_SEND )),
    mdiArea( new QMdiArea(this) ),
    seqRequest( 0 ), nBuffers( 0 )
{
    ui->setupUi(this);

    //mdiArea->setViewMode(QMdiArea::TabbedView);
    //mdiArea->setTabPosition(QTabWidget::East);
    //mdiArea->setTabShape(QTabWidget::Triangular);

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

    //set up the recieving socket
    //QtConcurrent::run( inSocket, &UdpListeningReceiveSocket::RunUntilSigInt );

    //and send a test/wakeup message
    char buffer[OUTPUT_BUFFER_SIZE];
    osc::OutboundPacketStream p( buffer, OUTPUT_BUFFER_SIZE );

    p << osc::BeginMessage( "/hello" ) <<
            "unaudicle is awake" << osc::EndMessage;

    outSocket.Send( p.Data(), p.Size() );

    //
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress::LocalHost, PORT_RECV);

    connect(udpSocket, SIGNAL(readyRead()),
            this, SLOT(readPendingDatagrams()));
}

MainWindow::~MainWindow()
{
    //inSocket->AsynchronousBreak();
    //delete inSocket;

    delete ui;
    delete mdiArea;

    delete udpSocket;
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

    QFont fixedFont("Courier", 10);

    CodeEdit *textEdit = new CodeEdit(mdiArea);
    textEdit->setPlainText( fileText );
    textEdit->setFont( fixedFont );

    textEdit->setProperty( "filePath", filePath );
    QMdiSubWindow *subWindow = mdiArea->addSubWindow( textEdit );
    subWindow->showMaximized();
    subWindow->setWindowTitle( fileInfo.fileName() );

    //try adding something to the toolbar
    //ui->mainToolBar->addAction( fileInfo.baseName() );
    //ui->mainToolBar->addWidget( new QPushButton("Hello") );

    /*
    //QHBoxLayout *box = new QHBoxLayout();
    QGridLayout *grid = new QGridLayout();
    grid->setSpacing(0);
    QWidget *widget = new QWidget();
    QPushButton *b1 = new QPushButton( "1" );
    QPushButton *b2 = new QPushButton( "2" );
    QPushButton *b3 = new QPushButton( "3" );
    QPushButton *b4 = new QPushButton( "4" );
    QPushButton *b5 = new QPushButton( "5" );
    b1->setMaximumWidth(20);
    b2->setMaximumWidth(20);
    b3->setMaximumWidth(20);
    b4->setMaximumWidth(20);
    b5->setMaximumWidth(20);

    grid->addWidget( new QPushButton( fileInfo.baseName()), 0, 0 );
    grid->addWidget( b1, 0, 1 );
    grid->addWidget( b2, 0, 2 );
    grid->addWidget( b3, 0, 3 );
    grid->addWidget( b4, 0, 4 );
    grid->addWidget( b5, 0, 5 );
    widget->setLayout( grid );
    widget->setMaximumHeight( 60 );

    ui->mainToolBar->addWidget( widget );
    */

    QPushButton *b1 = new QPushButton( fileInfo.fileName() );
    b1->setMaximumHeight( 40 );
    QGridLayout *gridL = (QGridLayout *)shredTree->layout();
    gridL->addWidget(b1,++nBuffers,0, Qt::AlignLeft|Qt::AlignTop);
    gridL->setRowStretch(nBuffers, 0);

    //connect this to focus changes in mdiArea
    connect( subWindow, SIGNAL(aboutToActivate()),
             b1, SLOT(animateClick()) );

    //also connect buttons to activate subwindows
    connect( b1, SIGNAL(clicked()),
             subWindow, SLOT(setFocus()));

    //add a widget to take up the rest of the space
    QWidget *spacer = new QWidget();
    gridL->addWidget(spacer,nBuffers+1,0,-1,0);
    gridL->setRowStretch(nBuffers+1, 1000);
}

void MainWindow::on_actionAdd_Shred_triggered()
{
    //get the text from the active buffer (but also the filename)
    CodeEdit *textEdit = (CodeEdit *) mdiArea->focusWidget();
    QVariant v = textEdit->property("filePath");

    //and send a message to the compiler
    char buffer[OUTPUT_BUFFER_SIZE];
    osc::OutboundPacketStream p( buffer, OUTPUT_BUFFER_SIZE );


    QString fileName = QFileInfo(v.toString()).fileName();
#ifdef WIN32
    //truncate the drive specifier from string
    QString filePath = v.toString().split(':').at(1);
#endif

    p << osc::BeginMessage( "/shred/new" ) <<
            filePath.toAscii().constData() <<
    //        ++seqRequest << osc::EndMessage;
         (int) qHash(fileName) << osc::EndMessage;

    outSocket.Send( p.Data(), p.Size() );
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

                //try all of the button/filenames to find something to associate
                /* for (int i=0; i<nBuffers; i++) {
                    QPushButton *b = (QPushButton *)gridL->itemAtPosition(i,0);
                    QString fileName = b->text();
                    if ( qHash(fileName)==edShrid ) {
                        std::cout << "found a match" << std::endl;
                    }
                } */

                /*
                QObjectList children = shredTree->children();
                for (int i=0; i<children.length(); i++) {

                    QObject *curr = children.at(i);
                    if ( !curr->isWidgetType() ) break;

                    QPushButton *b = qobject_cast<QPushButton *>(curr);

                    if ( b!=NULL ) {

                        QString fileName = b->text();
                        if ( qHash(fileName)==edShrid ) {
                            std::cout << "found a match" << std::endl;
                        }
                    }
                }*/
                for (int i=0; i<gridL->count(); i++) {
                    QWidget *w = gridL->itemAt(i)->widget();

                    if ( w != NULL ) {

                        QPushButton *b = qobject_cast<QPushButton *>(w);

                        if ( b!=NULL ) {

                            QString fileName = b->text();
                            if ( qHash(fileName)==edShrid ) {
                                std::cout << "found a match" << std::endl;

                                int row, column, rowSpan, columnSpan;
                                gridL->getItemPosition( i, &row, &column, &rowSpan, &columnSpan );

                                //add a shred button at the next open position
                                int col = 1;
                                while( gridL->itemAtPosition(row,col) != 0 )
                                    col++;

                                QPushButton *bS = new QPushButton( QString::number(shrid) );
                                gridL->addWidget( bS, row, col, 1, 1 );
                            }
                        }
                    }
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
