#include <iostream>

#include "osc/OscReceivedElements.h"
#include "osc/OscPacketListener.h"
#include "ip/UdpSocket.h"

#include "UnResponseListener.h"

void UnResponseListener::ProcessMessage( const osc::ReceivedMessage& m,
                                         const IpEndpointName& remoteEndpoint )
{
    try{
        if ( strcmp( m.AddressPattern(), "/hello" )==0 ) {

            //ignore the arguments, print some message
            std::cout << "got /hello message!" << std::endl;

        } else if ( strcmp( m.AddressPattern(), "/shred/new") ==0 ) {

            osc::int32 edShrid;
            osc::int32 shrid;
            m.ArgumentStream() >> edShrid >> shrid >> osc::EndMessage;

            std::cout << "/shred/new," << edShrid << "," << shrid << std::endl;

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
