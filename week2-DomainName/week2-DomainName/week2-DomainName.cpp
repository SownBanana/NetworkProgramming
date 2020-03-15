// week2-DomainName.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>


using namespace std;

int main()
{
    //Khoi tao winsock
    WSADATA wsaData;
    WORD wVersion = MAKEWORD(2, 2);
    if (WSAStartup(wVersion, &wsaData)) {
        cout << "Something went wrong :((";
    }

    addrinfo* info;         // Lay info
    SOCKADDR_IN addr;

    int count = 1;
    //string input;
    char* domains = new char[500];
    char* domain;
    bool check_domain = true;
    bool check_dot = false;

    cout << "Enter Some Domain Names (separate by space): ";
    //getline(cin, input);
    cin.getline(domains, 500);
    //string to char*
    
    //strcpy(domains, input.c_str());
    domain = strtok(domains, " ");
    //Kiem tra ten mien hop le
    cout << "Checking Entered Domain Names..."<<endl;
    cout << "================================" << endl;
    while (domain != NULL) {
        cout <<domain << " : ";
        check_domain = true;
        check_dot = false;
        for (int i = 0; i < strlen(domain); i++) {
            if (domain[i] == 46) check_dot = true;
            else if (!((48 <= domain[i] && domain[i] <= 57) ||
                (65 <= domain[i] && domain[i] <= 90) ||
                (97 <= domain[i] && domain[i] <= 122) || domain[i] == 45)) {
                check_domain = false;
                break;
            }
        }
        if (!check_domain || !check_dot) {
            cout << "Not Valid" << endl;
            cout << "================================" << endl;
            break;
        }
        else cout << "Valid" << endl;
    
        cout << "loading..."<<endl;
        //Phan giai ten mien

        //const char* pdomain = domain.c_str();   //convert string to pcstr
        int ret = getaddrinfo(domain, "http", NULL, &info);
        if (ret == 0) {
            memcpy(&addr, info->ai_addr, info->ai_addrlen);
            cout << domain << " : " << inet_ntoa(addr.sin_addr) << endl;
        }
        else cout << domain << " : " << "Can't find the domain name!" << endl;
        cout << "================================" << endl;
        domain = strtok(NULL, " ");
    }
    delete[] domains;
    WSACleanup();
    return 0;
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
