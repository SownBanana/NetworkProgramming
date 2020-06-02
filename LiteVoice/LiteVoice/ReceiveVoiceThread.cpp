#include "ReceiveVoiceThread.h"

ReceiveVoiceThread::ReceiveVoiceThread()
{
	receiver = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(9090);

	bind(receiver, (SOCKADDR*)&addr, sizeof(addr));
}

void ReceiveVoiceThread::run() {
	while (1)
	{
		char buf[256];
		int ret;
		ret = recvfrom(receiver, buf, sizeof(buf), 0, NULL, NULL);
		//Xử lý dl nhận
	}
}

ReceiveVoiceThread::~ReceiveVoiceThread()
{
	closesocket(receiver);
}