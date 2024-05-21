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
int checksum;


UDPHeader(int sourcePort,
          int destinationPort,
          int UDPLength):Message(8),
          sourcePort(sourcePort),
          destinationPort(destinationPort),
          UDPLength(UDPLength),
          checksum(0)
          {
          }
UDPHeader(std::vector<unsigned char> message):Message(UDPHeaderSize),
          sourcePort(0),
          destinationPort(0),
          UDPLength(0),
          checksum(0)
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
    pack(checksum,16);
}









};

#endif