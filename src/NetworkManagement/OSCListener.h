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

    /**
     * gets called when a OSC message comes in.
     * 
     * \param m message
     * \param remoteEndpoint
     */
    virtual void ProcessMessage(const osc::ReceivedMessage& m, const IpEndpointName& remoteEndpoint);

    
public: 

    /**
     * returns the static object of this class. creates new if called for the first time. (SINGLETON)
     * 
     * \return 
     */
    static OSCListener& instance();
    ~OSCListener() {}

    /**
     * starts listener.
     * 
     */
    void start();

    /**
     * true if the Record mode should be toggled.
     * 
     * \return m_toggleStatus
     */
    bool getToggleStatus();

private:
    OSCListener() {}							// verhindert, dass ein Objekt von außerhalb von Recorder erzeugt wird.         
                                                // protected, wenn man von der Klasse noch erben möchte

    OSCListener(const OSCListener&);				// verhindert, dass eine weitere Instanz via Kopier-Konstruktor erstellt werden kann
    OSCListener& operator = (const OSCListener&); //Verhindert weitere Instanz durch Kopie

    /**
     * saves if the Record mode should be toggled.
     */
    bool m_toggleStatus = false;

};