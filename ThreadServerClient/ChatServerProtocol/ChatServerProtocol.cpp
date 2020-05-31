// ChatServerProtocol.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <string>
using namespace std;

#pragma comment(lib, "ws2_32")
typedef struct CLIENT {
	SOCKET client;
	const char* id;
	bool isLogin;
} CLIENT;
//SOCKET clients[40];
//char* username[40];
int numberOfClients;
CLIENT clients[40];

int numGroup;
CLIENT groups[40][40];
char* groupNames[40];

int numBlock;
SOCKET blockList[80];  // even : block, even+1 : is blocked by even. ex: blockList[4] = 280, bloackList[5] = 300 is 300 is blocked by 280

DWORD WINAPI clientThread(LPVOID);
bool checkValidNameSyntax(char*);
void broadcastMess(SOCKET, const char*);
void removeClient(SOCKET);
int checkBlock(SOCKET, SOCKET);
bool unBlock(SOCKET, SOCKET);
bool unBlockAll(SOCKET);
CLIENT getClient(char*);

int main()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(9000);

	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	bind(listener, (SOCKADDR*)&addr, sizeof(addr));
	listen(listener, 5);

	// Chap nhan ket noi va truyen nhan du lieu
	printf("Waiting for a client\n");
	numberOfClients = 0;
	numBlock = 0;
	numGroup = 0;
	while (true)
	{
		SOCKET client = accept(listener, NULL, NULL);
		printf("Client arrive\n");
		CreateThread(0, 0, clientThread, &client, 0, 0);
	}

	closesocket(listener);
	WSACleanup();
}

//Xử lý client
DWORD WINAPI clientThread(LPVOID lpParam) {
	const char* first = "CONNECT ERROR Type [HELP] for more information.\n";
	const char* connect = "CONNECT";
	const char* disconnect = "DISCONNECT";
	const char* sendP = "SEND";
	const char* messid = "MESSAGE";
	const char* messall = "MESSAGE_ALL";
	const char* messgrp = "MESSAGE_GROUP";
	const char* user = "USER";
	const char* listP = "LIST";
	const char* blockP = "BLOCK";
	const char* unBlockP = "UNBLOCK";
	const char* groupP = "GROUP";
	const char* delGroupP = "DEL_GROUP";
	const char* outGroupP = "OUT_GROUP";

	const char* error = "ERROR";
	const char* ok = "OK";

	const char* help = "[CONNECT] ID : Login with ID\n"
		"[DISCONNECT] : Logout\n"
		"[SEND] ALL message : Send message to all user\n[GROUP] group_name user list : Make a group_name group with user in user list\n[OUT_GROUP] group_name : Leave group_name group\n[DEL_GROUP] group_name : Delete group_name\n[SEND] ID message : Send message to a user defined\n[SEND] GROUP_ID message : Send message to a group defined\n[BLOCK] ID : Block a user defined\n[UNBLOCK] ID : Unblock a user defined\n[LIST] : Get the user list\n";

	SOCKET client = *(SOCKET*)lpParam;

	clients[numberOfClients].client = client;
	clients[numberOfClients].id = "";
	clients[numberOfClients].isLogin = false;

	CLIENT* thisClient = &clients[numberOfClients];
	numberOfClients += 1;

	int ret;
	char buf[1024];
	char sendBuf[1024];

	//Xử lý protocol
	char cmd[11], stt[50], tmp[50];
	send(client, first, strlen(first), 0);
	while (true) {
		ret = recv(client, buf, sizeof(buf), 0);
		if (ret < 0) {
			if (thisClient->isLogin) {
				thisClient->isLogin = false;
				//Thông báo client khác
				sprintf(buf, "%s %s %s\n", user, disconnect, thisClient->id);
				broadcastMess(client, buf);
			}
			removeClient(client);
			return 0;
		}
		if (ret < sizeof(buf)) buf[ret] = '\0';
		printf("%s\n", buf);
		ret = sscanf(buf, "%s %s %s", cmd, stt, tmp);

		if (strcmp(cmd, "HELP") == 0) {
			send(client, help, strlen(help), 0);
		}
		//Xử lý chưa đăng nhập
		else if (thisClient->isLogin == false) {
			if (strcmp(cmd, "CONNECT") == 0) {
				if (ret == 2 && strcmp(stt, "") != 0 && strcmp(stt, "ALL") != 0) {
					//Kiểm tra tên đã được sử dụng chưa
					bool isUsed = false;
					for (int i = 0; i < numberOfClients; i++) {
						if (strcmp(stt, clients[i].id) == 0) {
							sprintf(buf, "%s %s %s\n", connect, error, "Name is used.");
							send(client, buf, strlen(buf), 0);
							isUsed = true;
							break;
						}
					}
					//Lưu tên và chuyển trạng thái đã đăng nhập
					if (!isUsed) {
						thisClient->id = (char*)malloc(strlen(stt) + 1);
						memcpy((void*)thisClient->id, stt, strlen(stt) + 1);
						thisClient->isLogin = true;
						//Gửi cho client
						sprintf(buf, "%s %s\n", connect, ok);
						send(client, buf, strlen(buf), 0);
						//Thông báo các client khác
						sprintf(buf, "%s %s %s\n", user, connect, stt);
						broadcastMess(client, buf);
					}
				}
				//Sai cú pháp CONNECT
				else {
					sprintf(buf, "%s %s %s\n", connect, error, "Require login first.");
					send(client, buf, strlen(buf), 0);
				}
			}
			//else if (strcmp(cmd, "DISCONNECT") == 0) {
			//	sprintf(buf, "%s %s %s\n", disconnect, error, "You haven't logged in yet.");
			//	send(client, buf, strlen(buf), 0);
			//}
			else {
				sprintf(buf, "%s %s %s\n", connect, error, "Require login first.");
				send(client, buf, strlen(buf), 0);
			}
		}
		//Xử lý đã đăng nhập
		else {
			//DISCONNECT
			if (strcmp(cmd, "DISCONNECT") == 0) {
				//Đúng cú pháp => đăng xuất
				if (ret == 1) {
					//Gửi client
					sprintf(buf, "%s %s\n", disconnect, ok);
					send(client, buf, strlen(buf), 0);
					//Thông báo client khác
					sprintf(sendBuf, "%s %s %s\n", user, disconnect, thisClient->id);
					broadcastMess(client, sendBuf);
					thisClient->isLogin = false;
					thisClient->id = "";
				}
				else {
					sprintf(buf, "%s %s %s\n", disconnect, error, "Wrong [DISCONNECT] Syntax.");
					send(client, buf, strlen(buf), 0);
				}
			}
			//CONNECT
			else if (strcmp(cmd, "CONNECT") == 0) {
				sprintf(buf, "%s %s %s%s\n", connect, error, "you_are_logined_with_id_", thisClient->id);
				send(client, buf, strlen(buf), 0);
			}
			//SEND
			else if (strcmp(cmd, "SEND") == 0) {
				//đúng cú pháp SEND
				if (ret > 2) {
					//Gửi tất cả
					if (strcmp(stt, "ALL") == 0) {
						sprintf(sendBuf, "%s %s %s", messall, thisClient->id, buf + 9);
						broadcastMess(client, sendBuf);
						sprintf(sendBuf, "%s %s\n", sendP, ok);
						send(client, sendBuf, strlen(sendBuf), 0);
					}
					//Gửi cho group
					else if (strcmp(stt, "GROUP") == 0) {
						//Lấy vị trí group
						int i;
						for (i = 0; i < numGroup; i++) {
							if (strcmp(tmp, groupNames[i]) == 0) break;
						}
						if (i == numGroup) {
							sprintf(sendBuf, "%s %s %s%s\n", sendP, error, "can't_find_group_", tmp);
							send(client, sendBuf, strlen(sendBuf), 0);
						}
						//Kiểm tra có là thành viên
						else {
							bool isMem = false;
							for (int j = 0; j < 40; j++) {
								if (groups[i][j].client == NULL) break;
								if (groups[i][j].client == client) {
									isMem = true;
									break;
								}
							}
							if (!isMem) {
								sprintf(sendBuf, "%s %s %s%s\n", sendP, error, "you_is_not_a_member_of_", tmp);
								send(client, sendBuf, strlen(sendBuf), 0);
								continue;
							}
							sprintf(sendBuf, "%s %s\n", sendP, ok);
							send(client, sendBuf, strlen(sendBuf), 0);
							sprintf(sendBuf, "%s %s %s %s", messgrp, tmp, thisClient->id, buf + strlen(tmp) + 12);
							for (int j = 0; j < 40; j++) {
								if (groups[i][j].client == NULL) break;
								if (groups[i][j].client == client) continue;
								if (checkBlock(client, groups[i][j].client) == 0)
									send(groups[i][j].client, sendBuf, strlen(sendBuf), 0);
							}
						}
					}
					//Gửi cho 1 id
					else {
						bool isSend = false;
						for (int i = 0; i < numberOfClients; i++) {
							if (strcmp(stt, clients[i].id) == 0 && clients[i].isLogin) {
								//Kiem tra có block không
								if (checkBlock(client, clients[i].client)) {
									//Block
									if (checkBlock(client, clients[i].client) == 1) {
										sprintf(sendBuf, "%s %s %s%s\n", sendP, error, "you_blocked_", stt);
										send(client, sendBuf, strlen(sendBuf), 0);
										//Bị Block
									}
									else if (checkBlock(client, clients[i].client) == 2) {
										sprintf(sendBuf, "%s %s %s%s\n", sendP, error, stt, "_blocked_you");
										send(client, sendBuf, strlen(sendBuf), 0);
									}
									isSend = true;
								}
								//Gửi
								else {
									sprintf(sendBuf, "%s %s %s", messid, thisClient->id, buf + strlen(stt) + 6);
									send(clients[i].client, sendBuf, strlen(sendBuf), 0);
									sprintf(sendBuf, "%s %s\n", sendP, ok);
									send(client, sendBuf, strlen(sendBuf), 0);
									isSend = true;
									break;
								}
							}
						}
						if (!isSend) {
							sprintf(sendBuf, "%s %s %s%s\n", sendP, error, "can't_find_id_", stt);
							send(client, sendBuf, strlen(sendBuf), 0);
						}
					}
				}
				//sai cú pháp SEND
				else {
					sprintf(sendBuf, "%s %s %s\n", sendP, error, "wrong_[SEND]_syntax");
					send(client, sendBuf, strlen(sendBuf), 0);
				}
			}
			//LIST
			else if (strcmp(cmd, "LIST") == 0) {
				if (ret == 1) {
					//Lấy danh sách user vào list[]
					char list[250] = "";
					for (int i = 0; i < numberOfClients; i++) {
						if (clients[i].client != client && clients[i].isLogin) {
							sprintf(list, "%s %s", list, clients[i].id);
						}
					}
					//Gửi list
					sprintf(sendBuf, "%s %s%s\n", listP, ok, list);
					send(client, sendBuf, strlen(sendBuf), 0);
				}
				else if (ret == 2) {
					//Lấy list group đã tham gia
					if (strcmp(stt, "GROUP") == 0) {
						char list[250] = "";
						for (int i = 0; i < numGroup; i++) {
							for (int j = 0; j < 40; j++) {
								if (groups[i][j].client == NULL) break;
								if (groups[i][j].client == client) {
									sprintf(list, "%s %s", list, groupNames[i]);
									break;
								}
							}
						}
						//Gửi list
						sprintf(sendBuf, "%s %s%s\n", listP, ok, list);
						send(client, sendBuf, strlen(sendBuf), 0);
					}
					//List block
					if (strcmp(stt, "BLOCK") == 0) {
						char list[250] = "";
						for (int i = 0; i < numberOfClients; i++) {
							int c = checkBlock(client, clients[i].client);
							if (c == 1) sprintf(list, "%s %s|block", list, clients[i].id);
							else if (c == 2) sprintf(list, "%s %s|be_blocked", list, clients[i].id);
						}
						//Gửi list
						sprintf(sendBuf, "%s %s %s\n", listP, ok, list);
						send(client, sendBuf, strlen(sendBuf), 0);
					}
				}
				else if (ret == 3) {
					if (strcmp(stt, "GROUP") == 0) {
						char list[250] = "";
						//Lấy vị trí group
						int i;
						for (i = 0; i < numGroup; i++) {
							if (strcmp(tmp, groupNames[i]) == 0) break;
						}
						if (i == numGroup) {
							sprintf(sendBuf, "%s %s %s%s\n", listP, error, "Can't find group ", tmp);
							send(client, sendBuf, strlen(sendBuf), 0);
						}
						//Kiểm tra có là thành viên
						else {
							bool isMem = false;
							for (int j = 0; j < 40; j++) {
								if (groups[i][j].client == NULL) break;
								if (groups[i][j].client == client) {
									isMem = true;
									break;
								}
							}
							if (!isMem) {
								sprintf(sendBuf, "%s %s %s%s\n", listP, error, "You are not a member of ", tmp);
								send(client, sendBuf, strlen(sendBuf), 0);
								continue;
							}

							for (int j = 0; j < 40; j++) {
								if (groups[i][j].client == NULL) break;
								if (groups[i][j].client == client) continue;
								sprintf(list, "%s %s", list, groups[i][j].id);
							}
						}
						//Gửi list
						sprintf(sendBuf, "%s %s%s\n", listP, ok, list);
						send(client, sendBuf, strlen(sendBuf), 0);
					}
				}
				else {
					sprintf(sendBuf, "%s %s %s\n", listP, error, "wrong_[LIST]_syntax");
					send(client, sendBuf, strlen(sendBuf), 0);
				}
			}
			//BLOCK
			else if (strcmp(cmd, "BLOCK") == 0) {
				if (ret == 2) {
					bool isBlock = false;
					for (int i = 0; i < numberOfClients; i++) {
						if (strcmp(stt, clients[i].id) == 0) {
							blockList[numBlock] = client;
							blockList[numBlock + 1] = clients[i].client;
							numBlock += 2;
							sprintf(sendBuf, "%s %s\n", blockP, ok);
							send(client, sendBuf, strlen(sendBuf), 0);
							isBlock = true;
							break;
						}
					}
					if (!isBlock) {
						sprintf(sendBuf, "%s %s %s%s\n", blockP, error, "can't_find_user_", stt);
						send(client, sendBuf, strlen(sendBuf), 0);
					}
				}
				else {
					sprintf(sendBuf, "%s %s %s\n", blockP, error, "wrong_[BLOCK]_syntax");
					send(client, sendBuf, strlen(sendBuf), 0);
				}
			}
			//UNBLOCK
			else if (strcmp(cmd, "UNBLOCK") == 0) {
				if (ret == 2) {
					bool isUnBlock = false;
					for (int i = 0; i < numberOfClients; i++) {
						if (strcmp(stt, clients[i].id) == 0) {
							isUnBlock = unBlock(client, clients[i].client);
							sprintf(sendBuf, "%s %s\n", unBlockP, ok);
							send(client, sendBuf, strlen(sendBuf), 0);
							break;
						}
					}
					if (!isUnBlock) {
						sprintf(sendBuf, "%s %s %s%s\n", unBlockP, error, "you_don't_block_", stt);
						send(client, sendBuf, strlen(sendBuf), 0);
					}
				}
				else {
					sprintf(sendBuf, "%s %s %s\n", unBlockP, error, "wrong_[UNBLOCK]_syntax");
					send(client, sendBuf, strlen(sendBuf), 0);
				}
			}
			//Tạo group
			else if (strcmp(cmd, "GROUP") == 0) {
				if (ret >= 3) {
					//Kiểm tra tên group
					bool isUsed = false;
					for (int i = 0; i < numGroup; i++) {
						if (strcmp(stt, groupNames[i]) == 0) {
							sprintf(buf, "%s %s %s\n", groupP, error, "group_name_is_used.");
							send(client, buf, strlen(buf), 0);
							isUsed = true;
							break;
						}
					}
					//Tạo group
					if (!isUsed) {
						int index = 0;  //vị trí client trong group

						groupNames[numGroup] = (char*)malloc(strlen(stt) + 1);
						memcpy(groupNames[numGroup], stt, strlen(stt) + 1);
						//Thêm user tạo group
						groups[numGroup][index].id = thisClient->id;
						groups[numGroup][index++].client = client;
						//Thêm các user khác

						char* user = strtok(buf + strlen(stt) + 7, " ");
						while (user) {
							if (user[strlen(user) - 1] == '\n') user[strlen(user) - 1] = 0;
							CLIENT tmpCln = getClient(user);
							if (tmpCln.isLogin) {				//không thêm các user không connect
								groups[numGroup][index].id = tmpCln.id;
								groups[numGroup][index++].client = tmpCln.client;
							}
							user = strtok(NULL, " ");
						}

						sprintf(buf, "%s %s\n", groupP, ok);
						send(client, buf, strlen(buf), 0);
						numGroup++;

						sprintf(buf, "%s %s %s\n", groupP, stt, thisClient->id);
						for (int i = 0; i < 40; i++) {
							if (groups[numGroup - 1][i].client == NULL) break;
							send(groups[numGroup - 1][i].client, buf, strlen(buf), 0);
						}
					}
				}
				else {
					sprintf(sendBuf, "%s %s %s\n", groupP, error, "wrong_[GROUP]_syntax");
					send(client, sendBuf, strlen(sendBuf), 0);
				}
			}
			//Xoá group
			else if (strcmp(cmd, "DEL_GROUP") == 0) {
				if (ret == 2) {
					//Lấy vị trí group
					int i;
					for (i = 0; i < numGroup; i++) {
						if (strcmp(stt, groupNames[i]) == 0) break;
					}
					if (i == numGroup) {
						sprintf(sendBuf, "%s %s %s%s\n", delGroupP, error, "can't_find_group_", stt);
						send(client, sendBuf, strlen(sendBuf), 0);
						continue;
					}
					//Kiểm tra có là thành viên
					else {
						bool isMem = false;
						for (int j = 0; j < 40; j++) {
							if (groups[i][j].client == NULL) break;
							if (groups[i][j].client == client) {
								isMem = true;
								break;
							}
						}
						if (!isMem) {
							sprintf(sendBuf, "%s %s %s%s\n", delGroupP, error, "you_is_not_a_member_of_", tmp);
							send(client, sendBuf, strlen(sendBuf), 0);
							continue;
						}
					}
					//Xoá group
					for (int j = 0; j < 40; j++) {
						if (groups[i][j].client == NULL) break;
						groups[i][j].client = NULL;
					}
					if (i == numGroup - 1) {
						numGroup -= 1;
						sprintf(sendBuf, "%s %s\n", delGroupP, ok);
						send(client, sendBuf, strlen(sendBuf), 0);
					}
					else {
						for (int j = 0; j < 40; j++) {
							if (groups[numGroup - 1][j].client == NULL) break;
							groups[i][j] = groups[numGroup - 1][j];
						}
						groupNames[i] = groupNames[numGroup - 1];
						numGroup -= 1;
						sprintf(sendBuf, "%s %s\n", delGroupP, ok);
						send(client, sendBuf, strlen(sendBuf), 0);
					}
				}
				else {
					sprintf(sendBuf, "%s %s %s\n", delGroupP, error, "wrong_[DEL_GROUP]_syntax");
					send(client, sendBuf, strlen(sendBuf), 0);
				}
			}
			//Out group
			else if (strcmp(cmd, "OUT_GROUP") == 0) {
				if (ret == 2) {
					//Lấy vị trí group
					int i;
					for (i = 0; i < numGroup; i++) {
						if (strcmp(stt, groupNames[i]) == 0) break;
					}
					if (i == numGroup) {
						sprintf(sendBuf, "%s %s %s%s\n", outGroupP, error, "can't_find_group_", stt);
						send(client, sendBuf, strlen(sendBuf), 0);
					}
					//Kiểm tra có là thành viên
					else {
						bool isMem = false;
						int index = 0;		//vị trí trong group
						int j;
						for (j = 0; j < 40; j++) {
							if (groups[i][j].client == NULL) break;
							if (groups[i][j].client == client) {
								isMem = true;
								index = j;
							}
						}
						j -= 1;
						if (!isMem) {
							sprintf(sendBuf, "%s %s %s%s\n", outGroupP, error, "you_is_not_a_member_of_", tmp);
							send(client, sendBuf, strlen(sendBuf), 0);
						}
						else {
							CLIENT* grp = groups[i];
							grp[index] = grp[j];
							grp[j].client = NULL;
							grp[j].id = NULL;
							sprintf(sendBuf, "%s %s\n", outGroupP, ok);
							send(client, sendBuf, strlen(sendBuf), 0);
						}
					}
				}
				else {
					sprintf(sendBuf, "%s %s %s\n", delGroupP, error, "wrong_[DEL_GROUP]_syntax");
					send(client, sendBuf, strlen(sendBuf), 0);
				}
			}
			//Sai syntax
			else {
				sprintf(sendBuf, "%s %s\n", error, "this_syntax_is_not_support");
				send(client, sendBuf, strlen(sendBuf), 0);
			}
		}
	}
}

void removeClient(SOCKET client) {
	//Remove block data
	for (int i = 0; i < numBlock; i += 2) {
		if (blockList[i] == client) {
			blockList[i] = blockList[numBlock - 2];
			blockList[i + 1] = blockList[numBlock - 1];
			numBlock -= 2;
			i -= 2;
		}
	}
	for (int i = 1; i < numBlock; i += 2) {
		if (blockList[i] == client) {
			blockList[i] = blockList[numBlock - 1];
			blockList[i - 1] = blockList[numBlock - 2];
			numBlock -= 2;
			i -= 2;
		}
	}
	//Remove Client
	int i;
	for (i = 0; i < numberOfClients; i++) {
		if (clients[i].client == client) break;
	}
	if (i == (numberOfClients - 1)) numberOfClients -= 1;
	else {
		clients[i] = clients[numberOfClients - 1];
		numberOfClients -= 1;
	}
}

void broadcastMess(SOCKET dst, const char* mess) {
	for (int i = 0; i <= numberOfClients; i++) {
		if (clients[i].client != dst && clients[i].isLogin && !checkBlock(dst, clients[i].client))
			send(clients[i].client, mess, strlen(mess), 0);
	}
}

int checkBlock(SOCKET cln, SOCKET other) {	//0 : normal  - 1 : cln block other - 2: other block cln
	for (int i = 0; i < numBlock; i += 2) {
		if (blockList[i] == cln && blockList[i + 1] == other) return 1;
		if (blockList[i] == other && blockList[i + 1] == cln) return 2;
	}
	return 0;
}
bool unBlock(SOCKET cln, SOCKET other) {
	int i;
	for (int i = 0; i < numBlock; i += 2) {
		if (blockList[i] == cln && blockList[i + 1] == other) {
			blockList[i] = blockList[numBlock - 2];
			blockList[i + 1] = blockList[numBlock - 1];
			numBlock -= 2;
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
	for (int i = 0; i < numberOfClients; i++) {
		if (strcmp(id, clients[i].id) == 0) return clients[i];
	}
	return rs;
}