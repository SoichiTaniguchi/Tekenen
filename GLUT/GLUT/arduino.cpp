#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include "arduino.h"

Arduino::Arduino() : com_data(1), Ret(false) {
	//this->com_data = 1;
	//this->Ret = false;
	Set_arduino();
}

Arduino::~Arduino() {

}

void Arduino::Set_arduino() {
	//1.ポートをオープン
	arduino_device = CreateFileA("COM7", GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (arduino_device == INVALID_HANDLE_VALUE) {
		printf("PORT COULD NOT OPEN\n");
		system("PAUSE");
		exit(0);
	}
	//2.送受信バッファ初期化
	Ret = SetupComm(arduino_device, 1024, 1024);
	if (!Ret) {
		printf("SET UP FAILED\n");
		CloseHandle(arduino_device);
		system("PAUSE");
		exit(0);
	}
	Ret = PurgeComm(arduino_device, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	if (!Ret) {
		printf("CLEAR FAILED\n");
		CloseHandle(arduino_device);
		exit(0);
	}
	//3.基本通信条件の設定
	DCB dcb;
	GetCommState(arduino_device, &dcb);
	dcb.DCBlength = sizeof(DCB);
	dcb.BaudRate = 9600;
	dcb.fBinary = TRUE;
	dcb.ByteSize = 8;
	dcb.fParity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;

	Ret = SetCommState(arduino_device, &dcb);
	if (!Ret) {
		printf("SetCommState FAILED\n");
		CloseHandle(arduino_device);
		system("PAUSE");
		exit(0);
	}
}


void Arduino::Send1_arduino(int data1)
{	
	DWORD dwSendSize;
	DWORD dwErrorMask;
	Ret = WriteFile(arduino_device, &data1, sizeof(data1), &dwSendSize, NULL);
}

void Arduino::Send2_arduino(int data2)
{
	DWORD dwSendSize;
	DWORD dwErrorMask;
	Ret = WriteFile(arduino_device, &data2, sizeof(data2), &dwSendSize, NULL);
}


