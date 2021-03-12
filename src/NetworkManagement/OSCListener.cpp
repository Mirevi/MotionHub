#include "OSCListener.h"


void OSCListener::ProcessMessage(const osc::ReceivedMessage& m, const IpEndpointName& remoteEndpoint)
{

    (void)remoteEndpoint; // suppress unused parameter warning

    try
    {
        // example of parsing single messages. osc::OsckPacketListener
        // handles the bundle traversal.

        if (std::strcmp(m.AddressPattern(), "/mh/Recorder/") == 0)
        {
            // example #2 -- argument iterator interface, supports
            // reflection for overloaded messages (eg you can call 
            // (*arg)->IsBool() to check if a bool was passed etc).
            osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
            std::string a1 = (arg++)->AsString();



           if (a1 == "toggleRec")
           {
               //it has to be the instance because it is called via osc::OscPacketListener, which calls a function of another object
               OSCListener::instance().m_toggleStatus = true;

           }
        }
    }
    catch (osc::Exception& e)
    {
        // any parsing errors such as unexpected argument types, or 
        // missing arguments get thrown as exceptions.
        std::cout << "error while parsing message: " << m.AddressPattern() << ": " << e.what() << "\n";
    }
}


void OSCListener::start()
{
    //connecting listener with Port
    OSCListener listener;
    UdpListeningReceiveSocket s(IpEndpointName(IpEndpointName::ANY_ADDRESS, 6161), &listener);

    s.RunUntilSigInt();

}

bool OSCListener::getToggleStatus()
{
    //saves the state so it can be reseted
    bool status = m_toggleStatus;
    m_toggleStatus = false;

    return status;
}


OSCListener& OSCListener::instance()
{
    static OSCListener _instance;
    return _instance;
}

