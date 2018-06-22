//
//  IPFilter.cpp
//  CardServer
//
//  Created by chen andycl on 16/8/1.
//  Copyright © 2016年 CardServer. All rights reserved.
//

#include "IPFilter.h"
int IPFilter::CreateIPFilterSocket()
{
    nSockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(nSockfd < 0)
    {
        printf("CreateIPFilterSocket failed\n");
        return -1;
    }
    
    return 0;
}

void IPFilter::SetIPFilterServer(std::string strIP, unsigned short port)
{
    strServerHost = strIP;
    nServerPort = port;
}

void IPFilter::SendFilterIPPacket(unsigned int nAddr)
{
    socklen_t nsockLen;
    struct sockaddr_in dstAddr;
    memset(&dstAddr, 0, sizeof(struct sockaddr_in));
    
    dstAddr.sin_family = AF_INET;
    dstAddr.sin_port = htons(nServerPort);
    dstAddr.sin_addr.s_addr = inet_addr(strServerHost.c_str());
    
    
    IPPACKET packet;
    
    packet.addr = nAddr;
    packet.level = 0;
    nsockLen = sizeof(struct sockaddr_in);
    
    
    sendto(nSockfd, &packet, sizeof(IPPACKET), MSG_DONTWAIT, (struct sockaddr *)&dstAddr, nsockLen);

}

void IPFilter::SendFilterIPPacket(std::string strIP)
{

    unsigned int nAddr = (unsigned int)inet_addr(strIP.c_str());
    
    socklen_t nsockLen;
    struct sockaddr_in dstAddr;
    memset(&dstAddr, 0, sizeof(struct sockaddr_in));
    
    dstAddr.sin_family = AF_INET;
    dstAddr.sin_port = htons(nServerPort);
    dstAddr.sin_addr.s_addr = inet_addr(strServerHost.c_str());
    
    
    IPPACKET packet;
    
    packet.addr = nAddr;
    packet.level = 0;
    nsockLen = sizeof(struct sockaddr_in);
    
    
    sendto(nSockfd, &packet, sizeof(IPPACKET), MSG_DONTWAIT, (struct sockaddr *)&dstAddr, nsockLen);


}
