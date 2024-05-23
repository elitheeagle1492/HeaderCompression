#ifndef PACKET
#define PACKET


#include "IPHeader.hpp"
#include "UDPHeader.hpp"
class Packet
{

public: 
IPHeader ipheader;
UDPHeader udpheader;
std::vector<unsigned char> m_payload;
Packet(std::string sourceIpaddress,
       std::string destinationIpaddress,
       int sourcePort,
       int destinationPort,
       int totalLengh,
       std::vector<unsigned char>& payload):ipheader(sourceIpaddress,destinationIpaddress,totalLengh),
       udpheader("","",sourcePort,
           destinationPort,
           totalLengh - 20),
           m_payload(totalLengh, 0)
{
    std::copy(ipheader.m_header.begin(), ipheader.m_header.end(),m_payload.data());
    std::copy(udpheader.m_header.begin(), udpheader.m_header.end(),m_payload.data() +20);
    std::copy(payload.begin(),payload.end(),m_payload.data() + 28);
}



};

#endif