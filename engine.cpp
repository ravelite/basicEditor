#include "engine.h"

#include "osc/OscOutboundPacketStream.h"
#include "osc/OscReceivedElements.h"
#include "osc/OscPacketListener.h"

#include <iostream>

#define PORT_SEND_CHUCK 7000
#define PORT_RECV 7001
#define PORT_SEND_SC 57120

#define OUTPUT_BUFFER_SIZE 1024
#define ADDRESS "127.0.0.1"

#define HELLO "/hello"
#define CHUCK_NEW "/chuck/new"
#define CHUCK_REMOVE "/chuck/remove"
#define SC_NEW "/sc/new"
#define SC_REMOVE "/sc/remove"

Engine::Engine(QObject *parent) :
    QObject(parent),
    outSocket( new QUdpSocket(this) ),
    inSocket( new QUdpSocket(this) )
{
    if ( !inSocket->bind(QHostAddress::LocalHost, PORT_RECV) )
        std::cout << "Failed to bind port " << PORT_RECV << std::endl;

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

    p << osc::BeginMessage( HELLO ) <<
            "unaudicle is awake" << osc::EndMessage;

    //test signal just for ChucK, maybe this code is not needed anymore
    outSocket->writeDatagram( p.Data(), p.Size(), QHostAddress::LocalHost, PORT_SEND_CHUCK );
}

//TODO: investigate this to see if this should send different messages
void Engine::makeProcess(QString filePath, Revision *r)
{
    int destPort = -1;
    const char *command = NULL;

    switch( r->srcLang ) {
    case Revision::SrcLangChuck:
        destPort = PORT_SEND_CHUCK;
        command = CHUCK_NEW;
        break;
    case Revision::SrcLangSc:
        destPort = PORT_SEND_SC;
        command = SC_NEW;
        break;
    default:
        return;
    }

    char buffer[OUTPUT_BUFFER_SIZE];
    osc::OutboundPacketStream p( buffer, OUTPUT_BUFFER_SIZE );

    //for now, just use the source file path for hash
    p << osc::BeginMessage( command ) <<
         filePath.toAscii().constData() <<
         r->getID() << osc::EndMessage;

    outSocket->writeDatagram( p.Data(), p.Size(),
                              QHostAddress::LocalHost, destPort );
}

void Engine::killProcess(Process *p)
{
    //TODO: refactor this copied code with makeProcess
    int destPort = -1;
    const char *command = NULL;

    switch( p->rev->srcLang ) {
    case Revision::SrcLangChuck:
        destPort = PORT_SEND_CHUCK;
        command = CHUCK_REMOVE;
        break;
    case Revision::SrcLangSc:
        destPort = PORT_SEND_SC;
        command = SC_REMOVE;
        break;
    default:
        return;
    }

    //actually send the command
    int pid = p->id;

    char buffer[OUTPUT_BUFFER_SIZE];
    osc::OutboundPacketStream pstream( buffer, OUTPUT_BUFFER_SIZE );
    pstream << osc::BeginMessage( command ) << pid << osc::EndMessage;

    outSocket->writeDatagram( pstream.Data(), pstream.Size(),
                              QHostAddress::LocalHost, destPort );

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
            if ( strcmp( m.AddressPattern(), HELLO )==0 ) {

                //ignore the arguments, print some message
                std::cout << "got /hello message!" << std::endl;

            } else if ( strcmp( m.AddressPattern(), CHUCK_NEW )==0 ||
                        strcmp( m.AddressPattern(), SC_NEW )==0 ) {

                osc::int32 edShrid;
                osc::int32 shrid;
                m.ArgumentStream() >> edShrid >> shrid >> osc::EndMessage;

                std::cout << m.AddressPattern() << "," << edShrid << "," << shrid << std::endl;

                Revision *r = findRevision( edShrid );
                if (r!=NULL) {

                    //mark revision as shredded
                    r->hasShredded = true;

                    //make a new process and add to display
                    Process *p = new Process(r, shrid);

                    processes << p; //keep track of it

                    notifyNewProcess(p);
                }

            } else if ( strcmp( m.AddressPattern(), CHUCK_REMOVE )==0 ||
                        strcmp( m.AddressPattern(), SC_REMOVE )==0 ) {

                osc::int32 shrid;
                m.ArgumentStream() >> shrid >> osc::EndMessage;

                std::cout << m.AddressPattern() << "," << shrid << std::endl;

                Process *p = NULL;
                if ( strcmp( m.AddressPattern(), CHUCK_REMOVE )==0 )
                    p = findProcess(shrid, Revision::SrcLangChuck);
                else if ( strcmp( m.AddressPattern(), SC_REMOVE )==0 )
                    p = findProcess(shrid, Revision::SrcLangSc);

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

void Engine::addRevision(Revision *r)
{
    revisions << r;
    revisionAdded(r);
}

//for doing graph layout
QList<Revision *> Engine::getRoots()
{
    QList<Revision *> ret;
    Revision *rev;

    foreach( rev, revisions ) {
        if ( rev->parent == NULL )
            ret << rev;
    }
    return ret;
}

QList<Revision *> Engine::getRevisions()
{
    return revisions;
}

Process * Engine::findProcess(int procId, Revision::SrcLangType srcLang) {
    Process *curr;
    foreach (curr, processes) {
        if ( curr->id == procId &&
             curr->rev->srcLang == srcLang )
            return curr;
    }
    //in case we don't find anything
    return NULL;
}
