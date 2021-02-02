/*
    Example of two different ways to process received OSC messages using oscpack.
    Receives the messages from the SimpleSend.cpp example.
*/

#include <iostream>
#include <cstring>

#include "ConfigDllExportNetworkManagement.h"


#if defined(__BORLANDC__) // workaround for BCB4 release build intrinsics bug
namespace std {
    using ::__strcmp__;  // avoid error: E2316 '__strcmp__' is not a member of 'std'.
}
#endif

#include "osc/OscReceivedElements.h"
#include "osc/OscPacketListener.h"
#include "ip/UdpSocket.h"


#define PORT 7000

class NetworkManagement_DLL_import_export OSCListener : public osc::OscPacketListener
{
protected:

    virtual void ProcessMessage(const osc::ReceivedMessage& m, const IpEndpointName& remoteEndpoint);
    
public: 
    void start();
};