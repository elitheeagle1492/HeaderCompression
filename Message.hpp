#ifndef MESSAGE
#define MESSAGE


#include <vector>
#include <cmath>
#include <bitset>
typedef std::vector<unsigned char> frame;
class Message
{
    
private:



protected:

size_t m_bitIndex;
public:
std::vector<unsigned char> m_header;
std::vector<unsigned char> m_body;
frame header()
{
    //we return a copy not the real thing.
    return m_header;
}

Message(size_t headerSize):m_header(headerSize),m_bitIndex(0)
{

}

void unpack(int& location, int size)
{
    auto start  = m_header.data() + (m_bitIndex / 8) ;
    int count = size;
    while(count != 0)
    {
        int remain = m_bitIndex % 8;
        int bitstart = 8 - remain;
        int bitsToget = std::min(bitstart, count);
        int shift = bitstart - bitsToget;
        int mask = std::pow(2,bitsToget) - 1;
        count -= bitsToget;
        location = location | ((((*start) & (mask << shift)) >> shift) << count);
  
        m_bitIndex += bitsToget;
        start  = m_header.data() + (m_bitIndex / 8);
    }


}

void pack(int value,int location ,int size)
{

    size_t pointerIndex = location/8;
    size_t offset = location % 8;
    unsigned char* byte = (m_header.data() + pointerIndex) ;
    while(size != 0)
    {
        size_t offset = location % 8;
        int space = 8 - offset;
        int bitsneeded = std::min(size, space);
        int mask = std::pow(2,bitsneeded) - 1;
        int shift = size - bitsneeded;
        int copyedValue =  (value & (mask << shift)) >> shift;
        int byteshift = space - bitsneeded;
        (*byte) = (*byte) | (copyedValue << byteshift);
        size -= space;
        location +=space;
        byte  = m_header.data() + (location / 8);
    }
}


void pack(int value, int size)
{
    size_t pointerIndex = m_bitIndex/8; 
    size_t offset = m_bitIndex % 8;
    unsigned char* byte = (m_header.data() + pointerIndex) ;
    while(size != 0)
    {
        int space = 8 - offset;
        int bitsneeded = std::min(size, space);
        int mask = std::pow(2,space) - 1;
        int shift = size - bitsneeded;
        int copyedValue = (value & (mask << shift)) >> shift;
        int byteshift = space - bitsneeded;
        *byte = (*byte | (copyedValue << byteshift));
        size -= bitsneeded;
        m_bitIndex +=bitsneeded;
        byte  = m_header.data() + (m_bitIndex / 8);
    }
}



std::string headerprintbytes()
{
    std::string print = "s";
    for(auto& iter : m_header)
    {
    std::bitset<8> bits(iter);
      print = print + bits.to_string();
    }
    return print;
}


};


#endif 


