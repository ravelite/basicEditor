#ifndef UNRESPONSELISTENER_H
#define UNRESPONSELISTENER_H

#include "osc/OscReceivedElements.h"
#include "osc/OscPacketListener.h"
#include "ip/UdpSocket.h"

class UnResponseListener : public osc::OscPacketListener {
protected:

    virtual void ProcessMessage( const osc::ReceivedMessage& m,
                const IpEndpointName& remoteEndpoint );
};

#endif // UNRESPONSELISTENER_H
