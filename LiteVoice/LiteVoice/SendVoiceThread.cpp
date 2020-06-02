#include "SendVoiceThread.h"

SendVoiceThread::SendVoiceThread()
{
	//numClient = 0;
}

SendVoiceThread::SendVoiceThread(char* addrip)
{
	//numClient = 1;
	//SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(addrip);
	addr.sin_port = htons(9090);
	//addrs.push_back(addr);
}
SendVoiceThread::SendVoiceThread(char* addrip, int port)
{
	//numClient = 1;
	//SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(addrip);
	addr.sin_port = htons(port);
	//addrs.push_back(addr);
}

SendVoiceThread::~SendVoiceThread()
{
	closesocket(sender);
}

void SendVoiceThread::addClient(char* addrip)
{
	//numClient++;
	//SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(addrip);
	addr.sin_port = htons(9090);
	//addrs.push_back(addr);
}
void SendVoiceThread::addClient(char* addrip, int port)
{
	//numClient++;
	//SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(addrip);
	addr.sin_port = htons(port);
	//addrs.push_back(addr);
}
void SendVoiceThread::run() {
	char buf[256];
	while (1)
	{
		printf("Enter a string: ");
		gets_s(buf);
		/*for (int i = 0; i < addrs.size(); i++) {
			sendto(sender, buf, strlen(buf), 0, (SOCKADDR*)&addrs.at(i), sizeof(addrs.at(i)));
		}*/
		sendto(sender, buf, strlen(buf), 0, (SOCKADDR*)&addr, sizeof(addr));
	}
}