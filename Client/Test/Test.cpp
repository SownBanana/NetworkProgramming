// Test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <stdio.h>
#include <Windows.h>
#include <string>
#include <tchar.h>

#pragma comment(lib, "user32.lib")

using namespace std;

int main()
{
	DWORD bufSize = 100;
	char listDisk[100];
	int numberofDisk = GetLogicalDriveStrings(bufSize, (LPWSTR)listDisk);
	wchar_t disk[3];
	__int64 avail, total, free;
	bool checkInsert;
	int size = numberofDisk/4;
	int* sizeofdisk = new int[size];
	int* freeofdisk = new int[size];
	int index = 0;
	for (int i = 0; i < numberofDisk * 2; i+=8) {
		cout << listDisk[i] <<": ";
		disk[0] = listDisk[i];
		disk[1] = listDisk[i + 2];
		disk[2] = 0;
		//wcout << disk;
		checkInsert = GetDiskFreeSpaceEx(disk, (PULARGE_INTEGER)&avail, (PULARGE_INTEGER)&total, (PULARGE_INTEGER)&free);
		if (checkInsert) {
			sizeofdisk[index] = total / (1024 * 1024 * 1024);
			freeofdisk[index++] = free / (1024 * 1024 * 1024);
		}
		else {
			sizeofdisk[index] = 0;
			freeofdisk[index++] = 0;
		}
	}
	for (int i = 0; i < size; i++) {
		cout <<"\n"<< listDisk[i*8] << listDisk[i*8 + 2] << listDisk[i*8 + 4] << "  " << sizeofdisk[i] <<"   "<< freeofdisk[i]<< " GB";
	}
}
