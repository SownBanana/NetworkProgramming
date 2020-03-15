// week2-DomainName.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;

int main()
{
    string domain;
    bool check_domain = true;
    bool check_dot = false;
    //Kiem tra ten mien hop le
    while (!check_domain || !check_dot) {
        check_domain = true;
        check_dot = false;
        cout << "Enter A Domain: ";
        cin >> domain;              //ten mien khong chua khoang trang
        for (int i = 0; i < domain.size(); i++) {
            if (domain[i] == 46) check_dot = true;
            else if (!((48 <= domain[i] && domain[i] <= 57) ||
                (65 <= domain[i] && domain[i] <= 90) ||
                (97 <= domain[i] && domain[i] <= 122) || domain[i] == 45)) {
                check_domain = false;
                break;
            }
        }
        if (!check_domain || !check_dot) {
            cout << "It's not a domain! Reenter The Domain.\n";
        }
    }
    cout << "DNS : IP"<<endl;
    //Khoi tao winsock
    WSADATA wsaData;
    WORD wVersion = MAKEWORD(2, 2);
    if (WSAStartup(wVersion, &wsaData)) {
        cout << "Something went wrong :((";
    }

    //Phan giai ten mien
    addrinfo* info;         // Lay info
    SOCKADDR_IN addr;     
    const char* pdomain = domain.c_str();   //convert string to pcstr
    int ret = getaddrinfo(pdomain, "http", NULL, &info);
    if (ret == 0) {
        memcpy(&addr, info->ai_addr, info->ai_addrlen);
        cout <<domain<< " : " << inet_ntoa(addr.sin_addr);
    }
    else cout << "Can't find the domain name!";
    
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
