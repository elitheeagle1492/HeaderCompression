#ifndef IPSERVER
#define IPSERVER

#include "TunServer.hpp"
#include <memory>
#include "socket.hpp"
typedef std::vector<unsigned char> byteVector;

class IPServer
{

std::unique_ptr<TunServer> m_tunserver;
std::unique_ptr<Socket> m_server;
std::unique_ptr<Socket> m_client;
std::string m_hostName;
std::string m_clientAddress;
int m_hostPort;
int m_clientPort;
int m_mtu;
size_t m_packetSize;
size_t m_headerSize;
size_t m_udpHeaderSize;
size_t m_ipHeaderSize;
// IPHeader m_ipClient; 
// UDPHeader m_udpClient;
public:


    IPServer(std::string hostName,
             std::string clientAddress,
             int hostPort,
             int clientPort,
             int mtu,
             std::string tunServerName):
             m_tunserver(new TunServer(tunServerName)),
             m_server(new Socket()),
             m_client(new Socket(clientAddress,clientPort)),
             m_hostName(hostName),
             m_clientAddress(clientAddress),
             m_hostPort(hostPort),
             m_clientPort(clientPort),
             m_mtu(mtu),
             m_headerSize(28),
             m_udpHeaderSize(8),
             m_ipHeaderSize(20)
    {

        if(m_mtu <=28 ||( m_mtu % 8) != 0)
        {
            std::cout << "MTU is to small using default 80" << std::endl;
            m_mtu = 80;
        }
        else
        {
            m_packetSize = m_mtu - 28;
        }
    }

 void runTun()
    {
        m_tunserver->readfromtun();
    }

    void run()
    {
        while(true)
        {
            if(!m_tunserver->IsQueueEmpty())
            {
                IPHeader message = m_tunserver->getMessage();
                fragment(message.m_body);

            }
        }
    }
     void runListen()
    {
        while(true)
        {
                //Assume no dropped packets.
          std::vector<IPHeader> fragments; 
         
          int flags = 0;
          while (flags != 2)
          {
             IPHeader message = m_client->listen();
             flags = message.flags;
             fragments.push_back(message);
           
          }
          defragment(fragments);   
              
        }
    }

void defragment(std::vector<IPHeader>& data)  
{   byteVector message;
    for(auto& iter : data)
    {
        message.insert(message.end(),iter.m_body.begin(),iter.m_body.end());
    }
    IPHeader ipHeader(message);
    UDPHeader UdpHeader(message);

    m_server->send(message,ipHeader.destinationIP,UdpHeader.destinationPort);
}


void fragment(byteVector& data)
{
    
    size_t byteCount = data.size();

    size_t numberOfPacks = byteCount/m_mtu + ((byteCount % m_mtu) != 0);
    int fragmentCount = 0;
    int identification = 0;
    size_t packed = 0;
    while(byteCount > 0)
    {

            size_t length = std::min(m_packetSize,byteCount);
            byteCount -= length;
       
            int flags = 2;
            if(byteCount != 0)
            {
                flags = 3;
            }
            IPHeader headerInstance(m_hostName, m_clientAddress, length + m_headerSize,identification,flags,fragmentCount);
            headerInstance.headerpack();
            headerInstance.addsum();

            fragmentCount = (length + m_udpHeaderSize) / 8;

            UDPHeader udpHeader(m_hostPort,m_clientPort, length + m_udpHeaderSize);

            byteVector message(length + m_headerSize);

            std::copy(headerInstance.m_header.begin(), headerInstance.m_header.end(), message.data());

            std::copy(udpHeader.m_header.begin(), udpHeader.m_header.end(), message.data() + m_ipHeaderSize);

            std::copy(data.begin() + packed, data.begin() + length, message.data() + m_headerSize);

            packed +=length;
            identification++;

            m_server->send(message, headerInstance.destinationIP, m_clientPort);
    }
}
};



#endif