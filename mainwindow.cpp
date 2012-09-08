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
    mdiArea( new QMdiArea(this) )
{
    ui->setupUi(this);

    mdiArea->setViewMode(QMdiArea::TabbedView);
    mdiArea->setTabPosition(QTabWidget::East);
    mdiArea->setTabShape(QTabWidget::Triangular);

    this->setCentralWidget(mdiArea);

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

    QPlainTextEdit *textEdit = new QPlainTextEdit(fileText, mdiArea);
    textEdit->setProperty( "fileName", fileName );
    QMdiSubWindow *subWindow = mdiArea->addSubWindow( textEdit );
    subWindow->show();

    QFileInfo fileInfo( fileName );

    //try adding something to the toolbar
    ui->mainToolBar->addAction( fileInfo.baseName() );

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
            fileName.toAscii().constData() << osc::EndMessage;

    outSocket.Send( p.Data(), p.Size() );
}
