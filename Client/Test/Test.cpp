// Test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <stdio.h>
#include <Windows.h>
#include <string>
#include <tchar.h>

#pragma comment(lib, "user32.lib")
#pragma warning (disable:4996)

using namespace std;

int main()
{
	FILE* fp = fopen("P:\\Test\\test.png", "rb");
	char* buf;
	fseek(fp, 0, SEEK_END);
	long len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	buf = (char*)calloc(len + 1, sizeof(char));
	//data = (char*)calloc(len + 1, sizeof(char));
	//int ret = fread(data, 1, len, fp);
	//data[len] = 0;
	int ret;
	char data[256];
	while (true) {
		ret = fread(data, 1, sizeof(data), fp);
		if (ret <= 0) break;
		printf("%s", buf);
		memcpy(buf + , )
	}

	fclose(fp);
}