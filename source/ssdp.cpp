#include "stdafx.h"
// ==========================================================================
//code=UTF8	tab=4
//
// Cybele:	Application SErver.
//
// 		ssdp.cpp
//		$Revision: 1.0 $
//		$Date: 2018/02/12 21:11:00 $
//
// ==========================================================================
//---------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#ifdef linux
#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
////////////////////////////////////////////////////////////
typedef int          SOCKET;
#define SOCKET_ERROR (-1)
#define SERROR(x) ((x) < 0 )
#else
////////////////////////////////////////////////////////////
#include <windows.h>
#include <conio.h>
#define SERROR(x) ((x) == INVALID_SOCKET)
#endif
#include "cbl_String.h"

#define SD_RECEIVE       0x00
#define SD_SEND          0x01
#define SD_BOTH          0x02
#define SSDP_IP_ADDRESS  "239.255.255.250"
#define SSDP_PORT        (1900)

const char buff[] = "M-SEARCH * HTTP/1.1\r\n"
"HOST: 239.255.255.250:1900\r\n"
"MAN: \"ssdp:discover\"\r\n"
"MX: 0\r\n"
"ST: udap:rootservice\r\n"
"USER-AGENT: CYBELE/1.0\r\n\r\n";

int    ssdp_client(wString& str, int loops);
int    sClose(SOCKET& socket);
#if 0
int sClose(SOCKET& socket)
{
	int ret;
#ifdef linux
	ret = shutdown(socket, SD_BOTH);
	if (ret != 0) {
		printf("shutdown error=%s,%d,%d\n", strerror(errno), errno, socket);
	}
	ret = close(socket);
	if (ret != 0) {
		printf("close error=%s,%d,%d\n", strerror(errno), errno, socket);
	}
#else
	ret = shutdown(socket, SD_BOTH);
	if (ret != 0) {
		sprintf("shutdown error=%s,%d,%d\n", strerror(errno), errno, socket);
	}
	ret = closesocket(socket);
	if (ret != 0) {
		sprintf("close error=%s,%d,%d\n", strerror(errno), errno, socket);
	}
#endif
	return ret;
}

//Function Call
int main()
{
#ifndef linux
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
	wString str;
	ssdp_client(str, 2);
	printf("%s\n", str.c_str());
#ifndef linux
	WSACleanup();
#endif
}
#endif
//SSDP
int ssdp_client(wString& str, int loops)
{
	char rcvdbuff[1000];
#ifdef linux
	unsigned int len;
#else
	int len;
#endif
	int Ret;
	str.clear();

	struct sockaddr_in their_addr;
	//struct hostent *he;
	SOCKET sock;
	//SOCKET作成
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (SERROR(sock)) return -1;

	their_addr.sin_family = AF_INET;
	their_addr.sin_addr.s_addr = inet_addr(SSDP_IP_ADDRESS);
	their_addr.sin_port = htons(SSDP_PORT);
	len = sizeof(struct sockaddr_in);

	for (auto i = 0; i < loops; i++) {
		//printf("buff:\n%s\n",buff);

		Ret = sendto(sock, buff, static_cast<int>(strlen(buff)), 0, reinterpret_cast<struct sockaddr*>(&their_addr), len);
		if (Ret < 0) {
			//printf("error in SENDTO() function");
			sClose(sock);
			return -1;
		}
		for (int j = 0; j < 10; j++) {
			//printf( "-------------%d----------\n",j);
			//Receiving Text from server
			//printf("\n\nwaiting to recv:\n");
			memset(rcvdbuff, 0, sizeof(rcvdbuff));

			// 受信のタイムアウト時間を設定
#ifdef linux
			//linuxではタイムアウトしないのでselectの実装にすること
			struct timeval send_tv;
			send_tv.tv_sec = 0;
			send_tv.tv_usec = 6;
			Ret = setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &send_tv, sizeof(send_tv));

#else
			int    timeout = 600;// 0.6 seconds
			Ret = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&timeout), sizeof(timeout));
#endif
			//printf( "%d===",Ret);
			if (Ret < 0) {
				sClose(sock);
				return -1;
			}
			//ssize_t recvfrom(int, void*, size_t, int, sockaddr*, socklen_t*)
			Ret = recvfrom(sock, rcvdbuff, sizeof(rcvdbuff), 0, (struct sockaddr*)&their_addr, &len);
			if (Ret < 0) {
				//printf("Error in Receiving=%d",Ret);
				break;
			}
			rcvdbuff[Ret] = '\0';
			if (str.find(rcvdbuff) == wString::npos) {
				if (str.length() == 0) {
					str += "[\"";
				}
				else {
					str += ",\"";
				}
				str += rcvdbuff;
				str += "\"";
			}
			//printf("RECEIVED MESSAGE FROM SERVER\t: %s\n",rcvdbuff);
		}
		//Delay for testing purpose
		//Sleep(3*1000);
	}
	sClose(sock);
	if (str.length() != 0) {
		str += "]";
	}
	return 0;
}

