#ifndef UDPHEADER
#define UDPHEADER
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <vector>
#include <iostream>
#include <cmath>
#include <arpa/inet.h>
#include "Message.hpp"


class UDPHeader :public Message 
{

public:

int sourcePort;
int destinationPort;
int UDPLength;
int m_headerchecksum;
int sourceIp;
int destinationIP;

UDPHeader(
          std::string sourceIpaddress,
          std::string destinationIpaddress,
          int sourcePort,
          int destinationPort,
          int UDPLength ):Message(UDPpseudoHeaderSize),
          sourcePort(sourcePort),
          destinationPort(destinationPort),
          UDPLength(UDPLength),
          m_headerchecksum(0)
          {
            inet_pton(AF_INET, sourceIpaddress.data(), &sourceIp);
            inet_pton(AF_INET, destinationIpaddress.data(), &destinationIP);
            pack(sourceIp, 32);
            pack(destinationIP, 32);
            pack(0, 8);
            pack(17, 8);
            pack(UDPLength,16);
            pack(sourcePort, 16);
            pack(destinationPort, 16);
            pack(UDPLength,16);
            pack(m_headerchecksum,16);
   
          }
UDPHeader(std::vector<unsigned char> message):Message(UDPHeaderSize),
          sourcePort(0),
          destinationPort(0),
          UDPLength(0),
          m_headerchecksum(0)
{
  std::copy(message.begin()  +IPHeaderSize,message.begin() + CombinedHeaderSize,m_header.data());
  unpack(sourcePort,16);
  unpack(destinationPort, 16);
  unpack(UDPLength,16);
}

void headerpack()
{
    pack(sourcePort, 16);
    pack(destinationPort, 16);
    pack(UDPLength,16);
    pack(m_headerchecksum,16);
}


void addsum()
{
   u_int32_t total = 0;

     for(int i = 0; i < 20; i= i+2)
   {  
     
        u_int16_t value = m_header[i] << 8 ;
        value = value | m_header[i +1]; 
        total += value;
   } 


  for(int i = 0; i < m_body.size(); i= i+2)
   {  
     
        u_int16_t value = m_body[i] << 8 ;
        if(i+1 <  m_body.size())
        {
         value = value | m_body[i +1];
        } 
        total += value;
   } 
  //  if(m_body.size() % 2 != 0)
  //  {
  //     u_int16_t value = m_body[m_body.size() - 1] << 8 ;
  //     total += value;
  //  }


   u_int16_t headerchecksum = total & 0xFFFF;
  u_int16_t extra = total >> 16;
  headerchecksum = headerchecksum + extra;
  headerchecksum = ~headerchecksum;
  pack(headerchecksum,144,16);
  m_headerchecksum = headerchecksum;
}






};

#endif