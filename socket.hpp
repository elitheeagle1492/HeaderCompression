#ifndef SOCKET
#define SOCKET

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include "Packet.hpp"
class Socket

{
int m_sd;
bool m_works;
sockaddr_in m_sin;
int length;
std::vector<unsigned char> buffer;
std::string m_listenAddress;
int m_listenPort;
int servaddrAddressint;
struct sockaddr_in  servaddr; 
public:
Socket():m_works(true),length(80), buffer(80),m_listenAddress(""),m_listenPort(0)
{

    m_sd = socket(PF_INET, SOCK_RAW, IPPROTO_UDP);
    m_sin.sin_family = AF_INET;
    if(m_sd < 0)
    {
        m_works = false;

        std::cout <<"socket() error" << std::endl;
    }

}
Socket(std::string listenAddress, int portNumber):m_works(true),length(80), buffer(80),m_listenAddress(listenAddress),m_listenPort(portNumber)
{

    inet_pton(AF_INET, m_listenAddress.data(), &servaddrAddressint);
    servaddr.sin_addr.s_addr = servaddrAddressint;
    servaddr.sin_port =m_listenPort;
    m_sd = socket(PF_INET, SOCK_RAW, IPPROTO_UDP);
    m_sin.sin_family = AF_INET;
    if(m_sd < 0)
    {
        m_works = false;

        std::cout <<"socket() error" << std::endl;
    }

}


void send(std::vector<unsigned char> packet , int destinationIP, int destinationPort)
{
    
    m_sin.sin_addr.s_addr  = destinationIP;
    m_sin.sin_port = destinationPort;
    int returnValue = sendto(m_sd,packet.data(),packet.size(),0,(struct sockaddr *)&m_sin,sizeof(m_sin));
    if(returnValue < 0 )
    {
        std::cout << "sendto failed" <<std::endl;
    }
}

IPHeader listen()
{  
   size_t messageLength = 21;
   std::vector<unsigned char> buffer(messageLength);
   socklen_t len;
   int n = recvfrom(m_sd, buffer.data(), messageLength,  
                 MSG_WAITALL, (struct sockaddr *) &servaddr, 
                &len);
    if(n > 21)
    {
        std::cout << "we have an issue" << std::endl;
    }             
    buffer[n] = '\0'; 
    std::string value = "";
    IPHeader ipHeader(buffer);
    std::vector<unsigned char> body(ipHeader.totalLengh - 20);
    int numberRecv = 0;
    int bufferSize =body.size();
    while (numberRecv < body.size())
    {
    socklen_t len;
    int n = recvfrom(m_sd, body.data() +numberRecv, bufferSize,  
                 MSG_WAITALL, (struct sockaddr *) &servaddr, 
                &len);
    numberRecv  += (n);
    bufferSize  -= ( n);

    }
    std::string print = "";
    for(auto& iter : body)
    {

        print = print + std::to_string(char(iter));
    }
    std::cout << print << std::endl;
    ipHeader.m_body = body;
    return ipHeader;
}
   
};
#endif
