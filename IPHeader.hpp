#ifndef IPHEADER
#define IPHEADER
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <vector>
#include <iostream>

#include <arpa/inet.h>
#include "Message.hpp"
#include "Constants.hpp"
class IPHeader :public Message
{
public:
int version;
int HLEN;
int TypeOfService;
int ECN;
int totalLengh;
int identification;
int flags;
int offset;
int timeToLive;
int protocal;
int m_headerchecksum;
int sourceIp;
int destinationIP;
bool valid;


IPHeader(std::vector<unsigned char>& message):Message(IPHeaderSize),
          version(0),
          HLEN(0),
          TypeOfService(0),
          totalLengh(0), //changes
          identification(0), 
          flags(0), //changes 
          offset(0), //changes THis needs to be updated if we break packet appart 
          timeToLive(0),
          protocal(0),
          m_headerchecksum(0), //changes
          sourceIp(0),
          destinationIP(0),
          valid(true)
{
  //first 4 bytes out of tunnel are not IPHeader more research required.
  std::copy(message.begin(),message.begin() + IPHeaderSize,m_header.data());
  unpack(version, 4);
  unpack(HLEN, 4);
  unpack(TypeOfService,8);
  unpack(totalLengh,16);
  unpack(identification,16);
  unpack(flags,3);
  unpack(offset,13);
  unpack(timeToLive,8);
  unpack(protocal,8);
  unpack(m_headerchecksum,16);
  unpack(sourceIp,32);
  unpack(destinationIP,32);


  //Hack to zero out checksum
  m_header[10] = 0;
  m_header[11] = 0;

  if(calculateCheckSum(m_headerchecksum))
  {
    valid = true;
  }

}


IPHeader( std::string sourceIpaddress,
          std::string destinationIpaddress,
          int totalLengh,
          int identification = 0,
          int flags = 0,
          int offset = 0):Message(20),
          version(4),
          HLEN(5),
          TypeOfService(0),
          ECN(0),
          totalLengh(totalLengh), //changes
          identification(1542), 
          flags(2), //changes 
          offset(0), //changes THis needs to be updated if we break packet appart 
          timeToLive(64),
          protocal(17),
          m_headerchecksum(0), //changes
          sourceIp(0),
          destinationIP(0)
          {
            inet_pton(AF_INET, sourceIpaddress.data(), &sourceIp);
            inet_pton(AF_INET, destinationIpaddress.data(), &destinationIP);

          }

bool calculateCheckSum(u_int16_t headerchecksum )
{
  u_int16_t test1 =headerchecksum;
  u_int16_t test = ~test1;

   u_int32_t total = 0;
   for(int i = 0; i < 20; i= i+2)
   {  
     
        u_int16_t value = m_header[i] << 8 ;
        value = value | m_header[i +1]; 
        total += value;
   } 
  u_int16_t checkSum = total & 0xFFFF;
  u_int16_t extra = total >> 16;
   checkSum = checkSum + extra;
  checkSum = ~checkSum;
   return (checkSum == headerchecksum);

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
  u_int16_t headerchecksum = total & 0xFFFF;
  u_int16_t extra = total >> 16;
  headerchecksum = headerchecksum + extra;
  headerchecksum = ~headerchecksum;
  pack(headerchecksum,80,16);
  m_headerchecksum = headerchecksum;
}

void headerpack()
{
    pack(version, 4);
    pack(HLEN, 4);
    pack(TypeOfService,8);
    pack(totalLengh,16);
    pack(identification,16);
    pack(flags,3);
    pack(offset,13);
    pack(timeToLive,8);
    pack(protocal,8);
    pack(0,16);
    pack(sourceIp,32);
    pack(destinationIP,32);
}

frame header()
{
    //we return a copy not the real thing.
    return m_header;
}


 std::string printbytes()
 {
    return this->headerprintbytes();
 }










};

#endif