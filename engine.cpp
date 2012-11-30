#include "engine.h"

#include "osc/OscOutboundPacketStream.h"
#include "osc/OscReceivedElements.h"
#include "osc/OscPacketListener.h"

#include <iostream>

#define PORT_SEND 7000
#define PORT_RECV 7001
#define OUTPUT_BUFFER_SIZE 1024
#define ADDRESS "127.0.0.1"

Engine::Engine(QObject *parent) :
    QObject(parent),
    outSocket( new QUdpSocket(this) ),
    inSocket( new QUdpSocket(this) )
{
    inSocket->bind(QHostAddress::LocalHost, PORT_RECV);

    connect(inSocket, SIGNAL(readyRead()),
            this, SLOT(readPendingDatagrams()));
}

Engine::~Engine()
{
    delete inSocket;
    delete outSocket;
}

void Engine::sendTestMessage() {

    //and send a test/wakeup message
    char buffer[OUTPUT_BUFFER_SIZE];
    osc::OutboundPacketStream p( buffer, OUTPUT_BUFFER_SIZE );

    p << osc::BeginMessage( "/hello" ) <<
            "unaudicle is awake" << osc::EndMessage;

    outSocket->writeDatagram( p.Data(), p.Size(), QHostAddress::LocalHost, PORT_SEND );
}

void Engine::shredFile(QString filePath, int revID) {

    char buffer[OUTPUT_BUFFER_SIZE];
    osc::OutboundPacketStream p( buffer, OUTPUT_BUFFER_SIZE );

    //for now, just use the source file path for hash
    p << osc::BeginMessage( "/shred/new" ) <<
         filePath.toAscii().constData() <<
         revID << osc::EndMessage;

    outSocket->writeDatagram( p.Data(), p.Size(), QHostAddress::LocalHost, PORT_SEND );
}

void Engine::readPendingDatagrams()
{
    //TODO: make a handler for the bodies under here
    while (inSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(inSocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        inSocket->readDatagram(datagram.data(), datagram.size(),
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

                    //TODO: connect this to something
                    //shredTree->addProcess(p);
                    notifyNewProcess(p);
                }

            } else if ( strcmp( m.AddressPattern(), "/shred/remove") ==0 ) {

                osc::int32 shrid;
                m.ArgumentStream() >> shrid >> osc::EndMessage;

                std::cout << "/shred/remove," << shrid << std::endl;

                Process *p = findProcess(shrid, Revision::SRCLANG_CHUCK);
                if ( p!=NULL ) {
                    processes.removeOne(p);

                    //shredTree->removeProcess(p);
                    notifyRemoveProcess(p);
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


Revision * Engine::findRevision(int revId) {
    Revision *curr;
    foreach (curr, revisions) {
        if ( curr->getID() == revId )
            return curr;
    }
    //in case we don't find anything
    return NULL;
}

Process * Engine::findProcess(int procId, int srcLang) {
    Process *curr;
    foreach (curr, processes) {
        if ( curr->id == procId &&
             curr->rev->srcLang == srcLang )
            return curr;
    }
    //in case we don't find anything
    return NULL;
}

void Engine::killChuckShred(Process *p)
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

void Engine::killProcess(Process *p)
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