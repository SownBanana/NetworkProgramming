// ThreadServerClient.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <string.h>
#include <string>

using namespace std;
#pragma
int main()
{
	FILE* fpdata = fopen("C:\\temp\\data.txt", "r");
	char password[50];
	char username[50];
	char aLine[256];
	while (fgets(aLine, 100, fpdata) != NULL) {
		printf("%s", aLine);
		sscanf(aLine, "%s", username);
		memcpy(password, aLine + strlen(username), 50);
		printf("Tk: %s MK: %s", username, password);
	}
}