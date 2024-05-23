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
sockaddr_in m_servaddr; 
public:
Socket():m_works(true),length(80), buffer(80),m_listenAddress(""),m_listenPort(0)
{

    m_sd = socket(PF_INET, SOCK_RAW, IPPROTO_RAW);
    m_sin.sin_family = AF_INET;
    if(m_sd < 0)
    {
        m_works = false;

        std::cout <<"socket() error" << std::endl;
    }

}
Socket(std::string listenAddress, int portNumber):m_works(true),length(80), buffer(80),m_listenAddress("0.0.0.0"),m_listenPort(portNumber)
{

    memset(&m_servaddr, 0, sizeof(m_servaddr)); 
	memset(&m_sin, 0, sizeof(m_sin)); 

    inet_pton(AF_INET, m_listenAddress.data(), &servaddrAddressint);
    m_servaddr.sin_addr.s_addr = servaddrAddressint;
    m_servaddr.sin_port =m_listenPort;
    m_sd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    m_sin.sin_family = AF_INET;
    if(m_sd < 0)
    {
        m_works = false;

        std::cout <<"socket() error" << std::endl;
    }
    if ( bind(m_sd, (const struct sockaddr *)&m_servaddr, 
			sizeof(m_servaddr)) < 0 ) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 

}


void sendtoRaw(std::vector<unsigned char> packet , int destinationIP, int destinationPort)
{
    
    m_sin.sin_addr.s_addr  = destinationIP;
    m_sin.sin_port = destinationPort;
    
    //int returnValue = send(m_sd,packet.data(), packet.size(),0);
    int returnValue = sendto(m_sd,packet.data(),packet.size(),0,(struct sockaddr *)&m_sin,sizeof(m_sin));
    if(returnValue < 0 )
    {
        std::cout << "sendto failed" <<std::endl;
    }
}

IPHeader listen()
{  
   std::vector<unsigned char> buffer(1024);
   socklen_t len =sizeof(m_servaddr);
   int n = recvfrom(m_sd, buffer.data(), 1024,  
                 MSG_WAITALL, (struct sockaddr *) &m_servaddr, 
                &len);       
    buffer[n] = '\0'; 
    std::string value = "";
    IPHeader ipHeader(buffer);
    std::vector<unsigned char> body(ipHeader.totalLengh - 20);

    if(ipHeader.valid)
    {   
        std::vector<unsigned char> bufferWholeMessage(ipHeader.totalLengh - IPHeaderSize);
        std::copy(buffer.data() + IPHeaderSize,buffer.data() +ipHeader.totalLengh , bufferWholeMessage.data());
        ipHeader.m_body = bufferWholeMessage;
    }

    // std::string print = "";
    // for(auto& iter : body)
    // {

    //     print = print + std::to_string(char(iter));
    // }
    // std::cout << print << std::endl;
    // ipHeader.m_body = body;
    return ipHeader;
}
   
};
#endif
