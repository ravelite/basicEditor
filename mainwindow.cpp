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

#include <iostream>

#define PORT_SEND 7000
#define PORT_RECV 7001
#define OUTPUT_BUFFER_SIZE 1024
#define ADDRESS "127.0.0.1"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    listener(),
    //open up a receiving port
    inSocket( new UdpListeningReceiveSocket( IpEndpointName( IpEndpointName::ANY_ADDRESS, PORT_RECV ),
              &listener )),
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
    QtConcurrent::run( inSocket, &UdpListeningReceiveSocket::RunUntilSigInt );

    //and send a test/wakeup message
    char buffer[OUTPUT_BUFFER_SIZE];
    osc::OutboundPacketStream p( buffer, OUTPUT_BUFFER_SIZE );

    p << osc::BeginMessage( "/hello" ) <<
            "unaudicle is awake" << osc::EndMessage;

    outSocket.Send( p.Data(), p.Size() );
}

MainWindow::~MainWindow()
{
    inSocket->AsynchronousBreak();
    delete inSocket;

    delete ui;
    delete mdiArea;
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open File"), "", tr("Chuck Files (*.ck)"));

    if ( fileName.isNull() ) return;

    QFile textFile(fileName);
    if (!textFile.open(QIODevice::ReadOnly | QIODevice::Text))
         return;

    QTextStream stream( &textFile );
    QString fileText = stream.readAll();

    QFileInfo fileInfo( fileName );

    QPlainTextEdit *textEdit = new QPlainTextEdit(fileText, mdiArea);
    textEdit->setProperty( "fileName", fileName );
    QMdiSubWindow *subWindow = mdiArea->addSubWindow( textEdit );
    subWindow->showMaximized();
    subWindow->setWindowTitle( fileInfo.baseName() );

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

    QPushButton *b1 = new QPushButton( fileInfo.baseName());
    b1->setMaximumHeight( 40 );
    QGridLayout *gridL = (QGridLayout *)shredTree->layout();
    gridL->addWidget(b1,++nBuffers,0, Qt::AlignLeft|Qt::AlignTop);
    gridL->setRowStretch(nBuffers, 0);

    //add a widget to take up the rest of the space
    QWidget *spacer = new QWidget();
    gridL->addWidget(spacer,nBuffers+1,0,-1,0);
    gridL->setRowStretch(nBuffers+1, 1000);
}

void MainWindow::on_actionAdd_Shred_triggered()
{
    //get the text from the active buffer (but also the filename)
    QPlainTextEdit *textEdit = (QPlainTextEdit *) mdiArea->focusWidget();
    QVariant v = textEdit->property("fileName");

    //and send a message to the compiler
    char buffer[OUTPUT_BUFFER_SIZE];
    osc::OutboundPacketStream p( buffer, OUTPUT_BUFFER_SIZE );

#ifdef WIN32
    //truncate the drive specifier from string
    QString fileName = v.toString().split(':').at(1);
#endif

    p << osc::BeginMessage( "/shred/new" ) <<
            fileName.toAscii().constData() <<
            ++seqRequest << osc::EndMessage;

    outSocket.Send( p.Data(), p.Size() );
}
