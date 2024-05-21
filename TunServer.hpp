#ifndef TUNSERVER
#define TUNSERVER

#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <net/route.h>

#include <unistd.h>
#include <queue>
#include "IPHeader.hpp"
#include <libexplain/ioctl.h>
#include <mutex>
#include <net/ethernet.h>
#define ifreq_offsetof(x)  offsetof(struct ifreq, x)
class TunServer
{
public:
    char* m_item;
    int m_fd;
    public:
    std::mutex m_incomeingQueue;
    std::queue<IPHeader> tunnelQueue;

    bool IsQueueEmpty()
    {
        std::lock_guard<std::mutex> lk(m_incomeingQueue);  
        return tunnelQueue.empty();
    }
    IPHeader getMessage()
    {
      std::lock_guard<std::mutex> lk(m_incomeingQueue);  
      IPHeader header = tunnelQueue.front();
      tunnelQueue.pop();
     
      return header;
    }

    TunServer(std::string tunName )
    {
        m_fd = 0;
        m_item = new char[128]();
       tunAlloc(tunName.data(), IFF_TUN);
       addRoute(tunName.data());
    }

    ~TunServer()
    {
       delete m_item;
    }
    void readfromtun()
        { 
            while(true)
            {
            std::vector<unsigned char> bufferHeader(IPHeaderSize+4);
            int result = read(m_fd,bufferHeader.data(),bufferHeader.size());
            if(result <= 0)
            {
                continue;
            }
            std::string print = "s";
            for(auto& iter : bufferHeader)
            {

            print = print + std::to_string(char(iter));
            }
            std::cout << print << std::endl;

            IPHeader header(bufferHeader);
            if(header.valid)
            {   
                std::vector<unsigned char> bufferWholeMessage(header.totalLengh);
                result = read(m_fd,bufferWholeMessage.data(),bufferWholeMessage.size());
                if(result <= 0)
                {
                    continue;
                }
                header.m_body = bufferWholeMessage;
                std::lock_guard<std::mutex> lk(m_incomeingQueue);
                tunnelQueue.push(header);
                }
            }

        }

    void addRoute(const char *dev) {

        struct ifreq ifr;
        struct sockaddr_in sai;
        int sockfd;                     /* socket fd we use to manipulate stuff with */
        int selector;
        unsigned char mask;

        char *p;


        /* Create a channel to the NET kernel. */
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);

        /* get interface name */
        memset(&ifr, 0, sizeof(ifr));
        strncpy(ifr.ifr_name, dev, IFNAMSIZ);

        memset(&sai, 0, sizeof(struct sockaddr));
        sai.sin_family = AF_INET;
        sai.sin_port = 5007;

        sai.sin_addr.s_addr = inet_addr("12.0.0.1");

        p = (char *) &sai;
        memcpy( (((char *)&ifr + ifreq_offsetof(ifr_addr) )),
                        p, sizeof(struct sockaddr));

        ioctl(sockfd, SIOCSIFADDR, &ifr);
        ioctl(sockfd, SIOCGIFFLAGS, &ifr);

        ifr.ifr_flags |= IFF_UP | IFF_RUNNING;
        // ifr.ifr_flags &= ~selector;  // unset something

        ioctl(sockfd, SIOCSIFFLAGS, &ifr);
         sai.sin_addr.s_addr = inet_addr("12.0.0.3");
        p = (char *) &sai;
        memcpy((((char *) &ifr + ifreq_offsetof(ifr_addr))), p,
               sizeof(struct sockaddr));
        ioctl(sockfd, SIOCSIFDSTADDR, &ifr);



        close(sockfd);
        
    }

    void tunAlloc(const char *dev, int flags) {

        struct ifreq ifr;
        int err;
        //std::string clonedev = "/dev/net/tun";

        /* Arguments taken by the function:
        *
        * char *dev: the name of an interface (or '\0'). MUST have enough
        *   space to hold the interface name if '\0' is passed
        * int flags: interface flags (eg, IFF_TUN etc.)
        */
         std::string clonedev = "/dev/net/tun";
        /* open the clone device */
         if( (m_fd = open(clonedev.data(), O_RDWR)) < 0 ) {
        //     std::cout << "success";
         }

        /* preparation of the struct ifr, of type "struct ifreq" */
        memset(&ifr, 0, sizeof(ifr));

        ifr.ifr_flags = flags;   /* IFF_TUN or IFF_TAP, plus maybe IFF_NO_PI */

        if (*dev) {
            /* if a device name was specified, put it in the structure; otherwise,
            * the kernel will try to allocate the "next" device of the
            * specified type */
            strncpy(ifr.ifr_name, dev, IFNAMSIZ);
        }

        /* try to create the device */
        if( (err = ioctl(m_fd, TUNSETIFF, (void *) &ifr)) < 0 ) {
            std::string testing = std::string(explain_ioctl(m_fd, TUNSETIFF, (void *) &ifr));
             std::cout << testing << std::endl;
            close(m_fd);
            //return err;
        }

        /* if the operation was successful, write back the name of the
        * interface to the variable "dev", so the caller can know
        * it. Note that the caller MUST reserve space in *dev (see calling
        * code below) */

        strcpy(m_item, ifr.ifr_name);

        /* this is the special file descriptor that the caller will use to talk
        * with the virtual interface */
        }
};

#endif