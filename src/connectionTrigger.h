#ifndef CONNECTIONTRIGGER_H
#define CONNECTIONTRIGGER_H

#include <networkTrigger.h>
#include <vector>
#include <string>

namespace Kitsune
{
namespace Network
{
class TcpClient;
}
}

namespace ToriiGateway
{

class ConnectionTrigger : public Kitsune::Network::NetworkTrigger
{
public:
    ConnectionTrigger();
    ~ConnectionTrigger();

    void runTask(uint8_t* buffer,
                 const long bufferSize,
                 Kitsune::Network::TcpClient* client);

private:
    Kitsune::Network::TcpClient* m_mindClient = nullptr;
    Kitsune::Network::TcpClient* m_clientClient = nullptr;
    Kitsune::Network::TcpClient* m_monitoringClient = nullptr;

};

}

#endif // CONNECTIONTRIGGER_H
