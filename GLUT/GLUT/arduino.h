#pragma once
#ifndef _ARDUINO_H_
#define _ARDUINO_H_
#include <Windows.h>
#include <string>

class Arduino {
public:
	BYTE com_data;
	bool Ret;
	HANDLE arduino_device;

	Arduino();
	~Arduino();

	void Set_arduino();

	void Send1_arduino(int data1);
	void Send2_arduino(int data2);

	void Send1_String(std::string& str) {
		DWORD dwSendSize;
		DWORD dwErrorMask;
		Ret = WriteFile(arduino_device, str.c_str(), str.size(), &dwSendSize, NULL);
	}

	void Send2_String(std::string& str) {
		DWORD dwSendSize;
		DWORD dwErrorMask;
		Ret = WriteFile(arduino_device, str.c_str(), str.size(), &dwSendSize, NULL);
	}

	void Send3_String(std::string& str) {
		DWORD dwSendSize;
		DWORD dwErrorMask;
		Ret = WriteFile(arduino_device, str.c_str(), str.size(), &dwSendSize, NULL);
	}

		/*
		int i = 0;
		std::string string = std::to_string(i);
		string += string;
		*/
};
#endif