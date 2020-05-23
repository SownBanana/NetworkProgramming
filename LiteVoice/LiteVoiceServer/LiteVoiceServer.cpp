// LiteVoiceServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32")
#pragma warning (disable:4996)

// Khai báo cấu trúc dữ liệu socket
typedef struct _PER_HANDLE_DATA
{
	SOCKET Socket;
	SOCKADDR_STORAGE ClientAddr;
} PER_HANDLE_DATA, * LPPER_HANDLE_DATA;

// Khai báo cấu trúc dữ liệu overlapped
typedef struct _PER_IO_DATA
{
	OVERLAPPED Overlapped;
	WSABUF DataBuf;
	char buf[1024];
} PER_IO_DATA, * LPPER_IO_DATA;

//Khai báo cấu trúc Client
typedef struct CLIENT {
	SOCKET client;
	const char* id;
	bool isLogin;
} CLIENT;

int numClients;
CLIENT clients[40];

int numGroups;
CLIENT groups[40][40];
char* groupNames[40];

int numBlocks;
SOCKET blockList[80];  // even : block, even+1 : is blocked by even. ex: blockList[4] = 280, bloackList[5] = 300 is 300 is blocked by 280

const char* first = "CONNECT ERROR Type [HELP] for more information.\n";
const char* connectP = "CONNECT";
const char* disconnectP = "DISCONNECT";
const char* sendP = "SEND";
const char* messidP = "MESSAGE";
const char* messallP = "MESSAGE_ALL";
const char* messgrpP = "MESSAGE_GROUP";
const char* userP = "USER";
const char* listP = "LIST";
const char* blockP = "BLOCK";
const char* unBlockP = "UNBLOCK";
const char* groupP = "GROUP";
const char* delGroupP = "DEL_GROUP";
const char* outGroupP = "OUT_GROUP";

const char* errorP = "ERROR";
const char* okP = "OK";

const char* helpP =
"[CONNECT] ID : Login with ID\n"
"[DISCONNECT] : Logout\n\n"

"[GROUP] group_name user list : Make a group_name group with user in user list\n"
"[OUT_GROUP] group_name : Leave group_name group\n"
"[DEL_GROUP] group_name : Delete group_name\n\n"

"[SEND] ALL message : Send message to all user\n"
"[SEND] ID message : Send message to a user defined\n"
"[SEND] GROUP GROUP_ID message : Send message to a group defined\n\n"

"[BLOCK] ID : Block a user defined\n"
"[UNBLOCK] ID : Unblock a user defined\n\n"

"[LIST] : Get the your friends list\n"
"[LIST] BLOCK : Get the your blocks list\n"
"[LIST] GROUP : Get the your groups list\n\n";

bool checkValidNameSyntax(char*);
void broadcastMess(SOCKET, const char*);
void removeClient(SOCKET);
int checkBlock(SOCKET, SOCKET);
bool unBlock(SOCKET, SOCKET);
bool unBlockAll(SOCKET);
CLIENT getClient(char*);
CLIENT* getClient(SOCKET);

DWORD WINAPI ServerWorkerThread(LPVOID);

int main()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	//Tạo đối tượng CompletionPort
	HANDLE CompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	// Lấy số lượng processor của hệ thống và tạo thread tương ứng
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);
	int countProcess;
	for (countProcess = 0; countProcess < systemInfo.dwNumberOfProcessors; countProcess++)
		CreateThread(NULL, 0, ServerWorkerThread, CompletionPort, 0, NULL);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(2699);

	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	bind(listener, (SOCKADDR*)&addr, sizeof(addr));

	//Cấu trúc client Socket
	PER_HANDLE_DATA* perHandleData = NULL;
	//Cấu trúc overlapped buf
	PER_IO_DATA* perIoData = NULL;

	DWORD bytesRecived, flags;

	numClients = 0;

	listen(listener, 5);

	// Chap nhan ket noi va truyen nhan du lieu
	printf_s("Waiting for client...\n");
	while (true) {
		//Khai báo biến lưu trữ thông tin client kết nối đến
		SOCKADDR_IN clientAddr;
		int addrLen = sizeof(clientAddr);
		//Chấp nhận kết nối
		SOCKET client = accept(listener, (SOCKADDR*)&clientAddr, &addrLen);

		//Gửi câu mở đầu
		send(client, first, strlen(first), 0);
		clients[numClients].client = client;
		clients[numClients].id = "";
		clients[numClients++].isLogin = false;

		//Cấp phát bộ nhớ cho cấu trúc lưu client socket và đại chỉ
		perHandleData = (LPPER_HANDLE_DATA)GlobalAlloc(GPTR, sizeof(PER_HANDLE_DATA));
		perHandleData->Socket = client;
		memcpy(&perHandleData->ClientAddr, &clientAddr, addrLen);

		CreateIoCompletionPort((HANDLE)client, CompletionPort, (DWORD)perHandleData, 0);

		//Cấp phát bộ nhớ cho cấu trúc lưu receive data
		perIoData = (LPPER_IO_DATA)GlobalAlloc(GPTR, sizeof(PER_IO_DATA));
		perIoData->DataBuf.buf = perIoData->buf;
		perIoData->DataBuf.len = sizeof(perIoData->buf);

		//Nhận dữ liệu lần đầu
		bytesRecived = 0;
		flags = 0;

		WSARecv(client, &(perIoData->DataBuf), 1, &bytesRecived, &flags, &(perIoData->Overlapped), NULL);
	}

	closesocket(listener);
	WSACleanup();
}

DWORD WINAPI ServerWorkerThread(LPVOID lpParam) {
	HANDLE CompletionPort = (HANDLE)lpParam;

	PER_HANDLE_DATA* pHandle = NULL;
	PER_IO_DATA* pIoData = NULL;
	DWORD bytesRecived, flags = 0;

	while (true) {
		int ret = GetQueuedCompletionStatus(CompletionPort, &bytesRecived, (PULONG_PTR)&pHandle, (LPOVERLAPPED*)&pIoData, INFINITE);
		if (bytesRecived == 0) {
			removeClient(pHandle->Socket);
			closesocket(pHandle->Socket);
			GlobalFree(pHandle);
			GlobalFree(pIoData);
			continue;
		}
		//Xử lý dữ liệu
		if (pIoData->buf[bytesRecived - 1] == '\n') pIoData->buf[bytesRecived - 1] = 0;
		else if (bytesRecived < pIoData->DataBuf.len) pIoData->buf[bytesRecived] = 0;

		SOCKADDR_IN clientAddr;
		memcpy(&clientAddr, &pHandle->ClientAddr, sizeof(clientAddr));
		printf_s("Client %s:%d : %s\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), pIoData->buf);

		//Cắt protocol
		char cmd[11], stt[50], tmp[50];
		char sendBuf[256];
		ret = sscanf(pIoData->buf, "%s %s %s", cmd, stt, tmp);

		CLIENT* client = getClient(pHandle->Socket);

		//Xử lý protocol
		if (strcmp(cmd, "HELP") == 0) {
			send(pHandle->Socket, helpP, strlen(helpP), 0);
		}
		//Xử lý chưa đăng nhập
		else if (client->isLogin == false) {
			if (strcmp(cmd, "CONNECT") == 0) {
				if (ret == 2 && strcmp(stt, "") != 0 && strcmp(stt, "ALL") != 0) {
					//Kiểm tra tên đã được sử dụng chưa
					bool isUsed = false;
					for (int i = 0; i < numClients; i++) {
						if (strcmp(stt, clients[i].id) == 0) {
							sprintf(sendBuf, "%s %s %s\n", connectP, errorP, "Name is used.");
							send(client->client, sendBuf, strlen(sendBuf), 0);
							isUsed = true;
							break;
						}
					}
					//Lưu tên và chuyển trạng thái đã đăng nhập
					if (!isUsed) {
						client->id = (char*)malloc(strlen(stt) + 1);
						memcpy((void*)client->id, stt, strlen(stt) + 1);
						client->isLogin = true;
						//Gửi cho client
						sprintf(sendBuf, "%s %s\n", connectP, okP);
						send(client->client, sendBuf, strlen(sendBuf), 0);
						//Thông báo các client khác
						sprintf(sendBuf, "%s %s %s\n", userP, connectP, stt);
						broadcastMess(client->client, sendBuf);
					}
				}
				//Sai cú pháp CONNECT
				else {
					sprintf(sendBuf, "%s %s %s\n", connectP, errorP, "Require login first.");
					send(client->client, sendBuf, strlen(sendBuf), 0);
				}
			}
			else {
				sprintf(sendBuf, "%s %s %s\n", connectP, errorP, "Require login first.");
				send(client->client, sendBuf, strlen(sendBuf), 0);
			}
		}
		//Xử lý đã đăng nhập
		else {
			//DISCONNECT
			if (strcmp(cmd, "DISCONNECT") == 0) {
				//Đúng cú pháp => đăng xuất
				if (ret == 1) {
					//Gửi client
					sprintf(sendBuf, "%s %s\n", disconnectP, okP);
					send(client->client, sendBuf, strlen(sendBuf), 0);
					//Thông báo client khác
					sprintf(sendBuf, "%s %s %s\n", userP, disconnectP, client->id);
					broadcastMess(client->client, sendBuf);
					client->isLogin = false;
					client->id = "";
				}
				else {
					sprintf(sendBuf, "%s %s %s\n", disconnectP, errorP, "Wrong [DISCONNECT] Syntax.");
					send(client->client, sendBuf, strlen(sendBuf), 0);
				}
			}
			//CONNECT
			else if (strcmp(cmd, "CONNECT") == 0) {
				sprintf(sendBuf, "%s %s %s%s\n", connectP, errorP, "you_are_logined_with_id_", client->id);
				send(client->client, sendBuf, strlen(sendBuf), 0);
			}
			//ADDFRIEND
			else if (strcmp(cmd, "ADDFRIEND") == 0) {
			}
			//SEND
			else if (strcmp(cmd, "SEND") == 0) {
				//đúng cú pháp SEND
				if (ret > 2) {
					//Gửi tất cả
					if (strcmp(stt, "ALL") == 0) {
						sprintf(sendBuf, "%s %s %s", messallP, client->id, pIoData->buf + 9);
						broadcastMess(client->client, sendBuf);
						sprintf(sendBuf, "%s %s\n", sendP, okP);
						send(client->client, sendBuf, strlen(sendBuf), 0);
					}
					//Gửi cho group
					else if (strcmp(stt, "GROUP") == 0) {
						//Lấy vị trí group
						int i;
						for (i = 0; i < numGroups; i++) {
							if (strcmp(tmp, groupNames[i]) == 0) break;
						}
						if (i == numGroups) {
							sprintf(sendBuf, "%s %s %s%s\n", sendP, errorP, "can't_find_group_", tmp);
							send(client->client, sendBuf, strlen(sendBuf), 0);
						}
						//Kiểm tra có là thành viên
						else {
							bool isMem = false;
							for (int j = 0; j < 40; j++) {
								if (groups[i][j].client == NULL) break;
								if (groups[i][j].client == client->client) {
									isMem = true;
									break;
								}
							}
							if (!isMem) {
								sprintf(sendBuf, "%s %s %s%s\n", sendP, errorP, "you_is_not_a_member_of_", tmp);
								send(client->client, sendBuf, strlen(sendBuf), 0);
								continue;
							}
							sprintf(sendBuf, "%s %s\n", sendP, okP);
							send(client->client, sendBuf, strlen(sendBuf), 0);
							sprintf(sendBuf, "%s %s %s %s", messgrpP, tmp, client->id, pIoData->buf + strlen(tmp) + 12);
							for (int j = 0; j < 40; j++) {
								if (groups[i][j].client == NULL) break;
								if (groups[i][j].client == client->client) continue;
								if (checkBlock(client->client, groups[i][j].client) == 0)
									send(groups[i][j].client, sendBuf, strlen(sendBuf), 0);
							}
						}
					}
					//Gửi cho 1 id
					else {
						bool isSend = false;
						for (int i = 0; i < numClients; i++) {
							if (strcmp(stt, clients[i].id) == 0 && clients[i].isLogin) {
								//Kiem tra có block không
								if (checkBlock(client->client, clients[i].client)) {
									//Block
									if (checkBlock(client->client, clients[i].client) == 1) {
										sprintf(sendBuf, "%s %s %s%s\n", sendP, errorP, "you_blocked_", stt);
										send(client->client, sendBuf, strlen(sendBuf), 0);
										//Bị Block
									}
									else if (checkBlock(client->client, clients[i].client) == 2) {
										sprintf(sendBuf, "%s %s %s%s\n", sendP, errorP, stt, "_blocked_you");
										send(client->client, sendBuf, strlen(sendBuf), 0);
									}
									isSend = true;
								}
								//Gửi
								else {
									sprintf(sendBuf, "%s %s %s", messidP, client->id, pIoData->buf + strlen(stt) + 6);
									send(clients[i].client, sendBuf, strlen(sendBuf), 0);
									sprintf(sendBuf, "%s %s\n", sendP, okP);
									send(client->client, sendBuf, strlen(sendBuf), 0);
									isSend = true;
									break;
								}
							}
						}
						if (!isSend) {
							sprintf(sendBuf, "%s %s %s%s\n", sendP, errorP, "can't_find_id_", stt);
							send(client->client, sendBuf, strlen(sendBuf), 0);
						}
					}
				}
				//sai cú pháp SEND
				else {
					sprintf(sendBuf, "%s %s %s\n", sendP, errorP, "wrong_[SEND]_syntax");
					send(client->client, sendBuf, strlen(sendBuf), 0);
				}
			}
			//LIST
			else if (strcmp(cmd, "LIST") == 0) {
				if (ret == 1) {
					//Lấy danh sách user vào list[]
					char list[250] = "";
					for (int i = 0; i < numClients; i++) {
						if (clients[i].client != client->client && clients[i].isLogin) {
							sprintf(list, "%s %s", list, clients[i].id);
						}
					}
					//Gửi list
					sprintf(sendBuf, "%s %s%s\n", listP, okP, list);
					send(client->client, sendBuf, strlen(sendBuf), 0);
				}
				else if (ret == 2) {
					//Lấy list group đã tham gia
					if (strcmp(stt, "GROUP") == 0) {
						char list[250] = "";
						for (int i = 0; i < numGroups; i++) {
							for (int j = 0; j < 40; j++) {
								if (groups[i][j].client == NULL) break;
								if (groups[i][j].client == client->client) {
									sprintf(list, "%s %s", list, groupNames[i]);
									break;
								}
							}
						}
						//Gửi list
						sprintf(sendBuf, "%s %s%s\n", listP, okP, list);
						send(client->client, sendBuf, strlen(sendBuf), 0);
					}
					//List block
					if (strcmp(stt, "BLOCK") == 0) {
						char list[250] = "";
						for (int i = 0; i < numClients; i++) {
							int c = checkBlock(client->client, clients[i].client);
							if (c == 1) sprintf(list, "%s %s|block", list, clients[i].id);
							else if (c == 2) sprintf(list, "%s %s|be_blocked", list, clients[i].id);
						}
						//Gửi list
						sprintf(sendBuf, "%s %s %s\n", listP, okP, list);
						send(client->client, sendBuf, strlen(sendBuf), 0);
					}
				}
				else if (ret == 3) {
					if (strcmp(stt, "GROUP") == 0) {
						char list[250] = "";
						//Lấy vị trí group
						int i;
						for (i = 0; i < numGroups; i++) {
							if (strcmp(tmp, groupNames[i]) == 0) break;
						}
						if (i == numGroups) {
							sprintf(sendBuf, "%s %s %s%s\n", listP, errorP, "Can't find group ", tmp);
							send(client->client, sendBuf, strlen(sendBuf), 0);
						}
						//Kiểm tra có là thành viên
						else {
							bool isMem = false;
							for (int j = 0; j < 40; j++) {
								if (groups[i][j].client == NULL) break;
								if (groups[i][j].client == client->client) {
									isMem = true;
									break;
								}
							}
							if (!isMem) {
								sprintf(sendBuf, "%s %s %s%s\n", listP, errorP, "You are not a member of ", tmp);
								send(client->client, sendBuf, strlen(sendBuf), 0);
								continue;
							}

							for (int j = 0; j < 40; j++) {
								if (groups[i][j].client == NULL) break;
								if (groups[i][j].client == client->client) continue;
								sprintf(list, "%s %s", list, groups[i][j].id);
							}
						}
						//Gửi list
						sprintf(sendBuf, "%s %s%s\n", listP, okP, list);
						send(client->client, sendBuf, strlen(sendBuf), 0);
					}
				}
				else {
					sprintf(sendBuf, "%s %s %s\n", listP, errorP, "wrong_[LIST]_syntax");
					send(client->client, sendBuf, strlen(sendBuf), 0);
				}
			}
			//BLOCK
			else if (strcmp(cmd, "BLOCK") == 0) {
				if (ret == 2) {
					bool isBlock = false;
					for (int i = 0; i < numClients; i++) {
						if (strcmp(stt, clients[i].id) == 0) {
							blockList[numBlocks] = client->client;
							blockList[numBlocks + 1] = clients[i].client;
							numBlocks += 2;
							sprintf(sendBuf, "%s %s\n", blockP, okP);
							send(client->client, sendBuf, strlen(sendBuf), 0);
							isBlock = true;
							break;
						}
					}
					if (!isBlock) {
						sprintf(sendBuf, "%s %s %s%s\n", blockP, errorP, "can't_find_user_", stt);
						send(client->client, sendBuf, strlen(sendBuf), 0);
					}
				}
				else {
					sprintf(sendBuf, "%s %s %s\n", blockP, errorP, "wrong_[BLOCK]_syntax");
					send(client->client, sendBuf, strlen(sendBuf), 0);
				}
			}
			//UNBLOCK
			else if (strcmp(cmd, "UNBLOCK") == 0) {
				if (ret == 2) {
					bool isUnBlock = false;
					for (int i = 0; i < numClients; i++) {
						if (strcmp(stt, clients[i].id) == 0) {
							isUnBlock = unBlock(client->client, clients[i].client);
							sprintf(sendBuf, "%s %s\n", unBlockP, okP);
							send(client->client, sendBuf, strlen(sendBuf), 0);
							break;
						}
					}
					if (!isUnBlock) {
						sprintf(sendBuf, "%s %s %s%s\n", unBlockP, errorP, "you_don't_block_", stt);
						send(client->client, sendBuf, strlen(sendBuf), 0);
					}
				}
				else {
					sprintf(sendBuf, "%s %s %s\n", unBlockP, errorP, "wrong_[UNBLOCK]_syntax");
					send(client->client, sendBuf, strlen(sendBuf), 0);
				}
			}
			//Tạo group
			else if (strcmp(cmd, "GROUP") == 0) {
				if (ret >= 3) {
					//Kiểm tra tên group
					bool isUsed = false;
					for (int i = 0; i < numGroups; i++) {
						if (strcmp(stt, groupNames[i]) == 0) {
							sprintf(sendBuf, "%s %s %s\n", groupP, errorP, "group_name_is_used.");
							send(client->client, sendBuf, strlen(sendBuf), 0);
							isUsed = true;
							break;
						}
					}
					//Tạo group
					if (!isUsed) {
						int index = 0;  //vị trí client trong group

						groupNames[numGroups] = (char*)malloc(strlen(stt) + 1);
						memcpy(groupNames[numGroups], stt, strlen(stt) + 1);
						//Thêm user tạo group
						groups[numGroups][index].id = client->id;
						groups[numGroups][index++].client = client->client;
						//Thêm các user khác

						char* user = strtok(pIoData->buf + strlen(stt) + 7, " ");
						while (user) {
							if (user[strlen(user) - 1] == '\n') user[strlen(user) - 1] = 0;
							CLIENT tmpCln = getClient(user);
							if (tmpCln.isLogin) {				//không thêm các user không connect
								groups[numGroups][index].id = tmpCln.id;
								groups[numGroups][index++].client = tmpCln.client;
							}
							user = strtok(NULL, " ");
						}

						sprintf(sendBuf, "%s %s\n", groupP, okP);
						send(client->client, sendBuf, strlen(sendBuf), 0);
						numGroups++;

						sprintf(sendBuf, "%s %s %s\n", groupP, stt, client->id);
						for (int i = 0; i < 40; i++) {
							if (groups[numGroups - 1][i].client == NULL) break;
							send(groups[numGroups - 1][i].client, sendBuf, strlen(sendBuf), 0);
						}
					}
				}
				else {
					sprintf(sendBuf, "%s %s %s\n", groupP, errorP, "wrong_[GROUP]_syntax");
					send(client->client, sendBuf, strlen(sendBuf), 0);
				}
			}
			//Xoá group
			else if (strcmp(cmd, "DEL_GROUP") == 0) {
				if (ret == 2) {
					//Lấy vị trí group
					int i;
					for (i = 0; i < numGroups; i++) {
						if (strcmp(stt, groupNames[i]) == 0) break;
					}
					if (i == numGroups) {
						sprintf(sendBuf, "%s %s %s%s\n", delGroupP, errorP, "can't_find_group_", stt);
						send(client->client, sendBuf, strlen(sendBuf), 0);
						continue;
					}
					//Kiểm tra có là thành viên
					else {
						bool isMem = false;
						for (int j = 0; j < 40; j++) {
							if (groups[i][j].client == NULL) break;
							if (groups[i][j].client == client->client) {
								isMem = true;
								break;
							}
						}
						if (!isMem) {
							sprintf(sendBuf, "%s %s %s%s\n", delGroupP, errorP, "you_is_not_a_member_of_", tmp);
							send(client->client, sendBuf, strlen(sendBuf), 0);
							continue;
						}
					}
					//Xoá group
					for (int j = 0; j < 40; j++) {
						if (groups[i][j].client == NULL) break;
						groups[i][j].client = NULL;
					}
					if (i == numGroups - 1) {
						numGroups -= 1;
						sprintf(sendBuf, "%s %s\n", delGroupP, okP);
						send(client->client, sendBuf, strlen(sendBuf), 0);
					}
					else {
						for (int j = 0; j < 40; j++) {
							if (groups[numGroups - 1][j].client == NULL) break;
							groups[i][j] = groups[numGroups - 1][j];
						}
						groupNames[i] = groupNames[numGroups - 1];
						numGroups -= 1;
						sprintf(sendBuf, "%s %s\n", delGroupP, okP);
						send(client->client, sendBuf, strlen(sendBuf), 0);
					}
				}
				else {
					sprintf(sendBuf, "%s %s %s\n", delGroupP, errorP, "wrong_[DEL_GROUP]_syntax");
					send(client->client, sendBuf, strlen(sendBuf), 0);
				}
			}
			//Out group
			else if (strcmp(cmd, "OUT_GROUP") == 0) {
				if (ret == 2) {
					//Lấy vị trí group
					int i;
					for (i = 0; i < numGroups; i++) {
						if (strcmp(stt, groupNames[i]) == 0) break;
					}
					if (i == numGroups) {
						sprintf(sendBuf, "%s %s %s%s\n", outGroupP, errorP, "can't_find_group_", stt);
						send(client->client, sendBuf, strlen(sendBuf), 0);
					}
					//Kiểm tra có là thành viên
					else {
						bool isMem = false;
						int index = 0;		//vị trí trong group
						int j;
						for (j = 0; j < 40; j++) {
							if (groups[i][j].client == NULL) break;
							if (groups[i][j].client == client->client) {
								isMem = true;
								index = j;
							}
						}
						j -= 1;
						if (!isMem) {
							sprintf(sendBuf, "%s %s %s%s\n", outGroupP, errorP, "you_is_not_a_member_of_", tmp);
							send(client->client, sendBuf, strlen(sendBuf), 0);
						}
						else {
							CLIENT* grp = groups[i];
							grp[index] = grp[j];
							grp[j].client = NULL;
							grp[j].id = NULL;
							sprintf(sendBuf, "%s %s\n", outGroupP, okP);
							send(client->client, sendBuf, strlen(sendBuf), 0);
						}
					}
				}
				else {
					sprintf(sendBuf, "%s %s %s\n", delGroupP, errorP, "wrong_[DEL_GROUP]_syntax");
					send(client->client, sendBuf, strlen(sendBuf), 0);
				}
			}
			//Sai syntax
			else {
				sprintf(sendBuf, "%s %s\n", errorP, "this_syntax_is_not_support");
				send(client->client, sendBuf, strlen(sendBuf), 0);
			}
		}

		// Khởi tạo lại cấu trúc Overlapped
		flags = 0;
		memset(&(pIoData->Overlapped), 0, sizeof(OVERLAPPED));
		//Yêu cầu nhận tiếp dữ liệu
		WSARecv(pHandle->Socket, &(pIoData->DataBuf), 1, &bytesRecived, &flags, &pIoData->Overlapped, NULL);
	}
}

void removeClient(SOCKET client) {
	//Remove block data
	for (int i = 0; i < numBlocks; i += 2) {
		if (blockList[i] == client) {
			blockList[i] = blockList[numBlocks - 2];
			blockList[i + 1] = blockList[numBlocks - 1];
			numBlocks -= 2;
			i -= 2;
		}
	}
	for (int i = 1; i < numBlocks; i += 2) {
		if (blockList[i] == client) {
			blockList[i] = blockList[numBlocks - 1];
			blockList[i - 1] = blockList[numBlocks - 2];
			numBlocks -= 2;
			i -= 2;
		}
	}
	//Remove Client
	int i;
	for (i = 0; i < numClients; i++) {
		if (clients[i].client == client) break;
	}
	if (i == (numClients - 1)) numClients -= 1;
	else {
		clients[i] = clients[numClients - 1];
		numClients -= 1;
	}
}

void broadcastMess(SOCKET dst, const char* mess) {
	for (int i = 0; i <= numClients; i++) {
		if (clients[i].client != dst && clients[i].isLogin && !checkBlock(dst, clients[i].client))
			send(clients[i].client, mess, strlen(mess), 0);
	}
}

int checkBlock(SOCKET cln, SOCKET other) {	//0 : normal  - 1 : cln block other - 2: other block cln
	for (int i = 0; i < numBlocks; i += 2) {
		if (blockList[i] == cln && blockList[i + 1] == other) return 1;
		if (blockList[i] == other && blockList[i + 1] == cln) return 2;
	}
	return 0;
}
bool unBlock(SOCKET cln, SOCKET other) {
	int i;
	for (int i = 0; i < numBlocks; i += 2) {
		if (blockList[i] == cln && blockList[i + 1] == other) {
			blockList[i] = blockList[numBlocks - 2];
			blockList[i + 1] = blockList[numBlocks - 1];
			numBlocks -= 2;
			return true;
		}
	}
	return false;
}

CLIENT getClient(char* id) {
	CLIENT rs;
	rs.id = id;
	rs.client = NULL;
	rs.isLogin = false;
	for (int i = 0; i < numClients; i++) {
		if (strcmp(id, clients[i].id) == 0) return clients[i];
	}
	return rs;
}

CLIENT* getClient(SOCKET sock) {
	for (int i = 0; i < numClients; i++) {
		if (sock == clients[i].client) return &clients[i];
	}
	return NULL;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started:
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file