
#include <iostream>
#include <fstream>
#include "TunServer.hpp"
#include "IPHeader.hpp"
#include <memory>
#include "TunServer.hpp"
#include "IPserver.hpp"
#include <thread>
void testUDPPacketAndUnpack()
{
    std::string myText = "KJASHDHADHKJAHKDHKJADJKHAKJDHJKADJKHJDIK";
    int total = myText.size() + 32;
    IPHeader test1("10.0.0.1",
                   "10.0.0.2",
                    total);
    test1.headerpack();
    test1.addsum();
    std::vector<unsigned char> vector(total);
    std::vector<unsigned char> header = test1.header();
    std::copy(header.begin(),header.end(),vector.data());
    std::cout  << test1.printbytes() << std::endl;
    IPHeader test2(vector);

 
    std::cout  << test2.printbytes()<< std::endl;

}



int main(int argc, char* argv[]) {


   
    
    int i = 0; 

    std::string hostName = "";
    std::string nodeType = ""; 
    std::string tunName = "tun0";
    int hostPort = 0;
    std::string clientAddress ="";
    int clientPort = 5567;
    int mtu = 80;
    
    while (i < argc) { 
        std::cout << "Argument " << i + 1 << ": " << argv[i] << std::endl; 
        if( std::string(argv[i]) == "host" && (i+ 1) < argc)
        {
            hostName = argv[i + 1];
        }
        if( std::string(argv[i]) == "nodeType" && (i+ 1) < argc)
        {
            nodeType = argv[i + 1];
        }
        if( std::string(argv[i]) == "hostPort" && (i+ 1) < argc)
        {
            hostPort = std::stoi(argv[i + 1]);
        }
        if( std::string(argv[i]) == "clientAddress" && (i+ 1) < argc)
        {
            clientAddress = argv[i + 1];
        }
        if( std::string(argv[i]) == "clientPort" && (i+ 1) < argc)
        {
            clientPort = std::stoi(argv[i + 1]);
        }
        if( std::string(argv[i]) == "mtu" && (i+ 1) < argc)
        {
            mtu = std::stoi(argv[i + 1]);
        }
        i++; 
    }


    std::unique_ptr<IPServer> server(new IPServer(hostName,
                                                    clientAddress,
                                                    hostPort,
                                                    clientPort,
                                                    mtu,
                                                    tunName));

     std::thread tunThread (&IPServer::runTun, server.get());
     std::thread sendThread (&IPServer::run, server.get());
     std::thread listenThread (&IPServer::runListen, server.get());

     tunThread.join();

    return 0;
}

