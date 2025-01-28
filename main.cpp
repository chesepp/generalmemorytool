#include <iostream>
#include <Windows.h>
#include <stdlib.h>
#include <psapi.h>
#include <string>
#include <sstream>
#include <tlhelp32.h>
#include <psapi.h>
#include "addressdatatype.h"
#include "memoryscanner.h"
using namespace std;

int choice = 0;
DWORD GetModule(HANDLE pHandle, string programname)
{
	HMODULE hMods[1024];
	//HANDLE pHandle = pHandle;
	DWORD cbNeeded;
	unsigned int i;

	if (EnumProcessModules(pHandle, hMods, sizeof(hMods), &cbNeeded))
	{
		vector < pair<wstring, uintptr_t>> modules;
		for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
		{
			TCHAR szModName[MAX_PATH];
			if (GetModuleFileNameEx(pHandle, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR)))
			{
				wstring wstrModName = szModName;
				modules.push_back({ wstrModName, reinterpret_cast<uintptr_t>(hMods[i]) });
				//you will need to change this to the name of the exe of the foreign process
				wostringstream conv;
				conv << programname.c_str();
				wstring pname(conv.str());
				wstring wstrModContain = pname;
				uintptr_t baseaddr = reinterpret_cast<uintptr_t>(hMods[i]);
				//wcout << L"Module " << i << L": " << szModName << endl;
				if (wstrModName.find(wstrModContain) != string::npos)
				{
					return static_cast<DWORD>(baseaddr);
				}
			}
		}
		int selectedIndex = -1;
		wcout << L"No module found matching the name \"" << wstring(programname.begin(), programname.end()) << L"\"." << endl;
		wcout << L"Please select a module by index:" << endl;

		for (size_t j = 0; j < modules.size(); j++) {
			wcout << L"[" << j << L"] " << modules[j].first << endl;
		}

		while (true) {
			cout << "Enter module index: ";
			cin >> selectedIndex;

			if (selectedIndex >= 0 && static_cast<size_t>(selectedIndex) < modules.size()) {
				return static_cast<DWORD>(modules[selectedIndex].second);
			}
			else {
				cout << "Invalid index. Try again." << endl;
			}
		}

	}
    cerr << "Failed to enumerate process modules. Error: " << GetLastError() << endl;
	return 0;
}
bool isValidMemoryAddress(DWORD address, HANDLE pHandle) {
	MEMORY_BASIC_INFORMATION mbi;
	if (VirtualQueryEx(pHandle, LPVOID(address), &mbi, sizeof(mbi))) {
		return mbi.State == MEM_COMMIT;
	}
	else {
		return false;
	}
}
void BackToMenu()
{
	choice = 0;
	system("cls");

}
void waitforkeypress() {
	bool waiting = true;
	cout << "press F5 to go back to menu" << endl;
	while (waiting)
	{
		if (GetAsyncKeyState(VK_F5))
		{
			break;
		}
		else {
			Sleep(500);
		}
	}
}

bool is64Bit = true;

int main() {
	string applicationName;
	cout << "Enter Application Name: ";
	getline(cin, applicationName);
	cout << endl;
	const char* windowname = applicationName.c_str();
	HWND hwnd = FindWindowA(NULL, windowname);
	DWORD procId;
	GetWindowThreadProcessId(hwnd, &procId);
	if (procId != 0) {
		cout << "Window Does Exist" << endl;
	}
	else
	{
		cout << "Process not open or wrong process name given" << endl;
		return 0;
	}
	HANDLE pHandle = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);
	if (pHandle == INVALID_HANDLE_VALUE) {
		cout << "cannot open process, program handle = INVALID_HANDLE_VALUE" << endl;
		return 0;
	}
	else if (pHandle == NULL)
	{
		cout << "cannot open process, program handle = NULL" << endl;
		return 0;
	}
	//menu
	DWORD BaseAddress = GetModule(pHandle, applicationName);
	cout << "Program Base Address: " << hex << uppercase << "0x"<<  BaseAddress << endl;
	while (true) {
		if (choice == 0)
		{
			cout << "1. Read Memory from address" << endl << "2. Write Memory from address" << endl << "3. Scan Memory From Value" << endl << "4. Address Calculator" << endl;
			cin >> choice;
		}
		else if (choice == 1) {
			//read memory address


			string inputaddress;
			char yesno;
			cout << "Read Memory From Base Address?(y/n): ";
			cin >> yesno;
			if (yesno == 'y') {
				cout << "Input Memory Address from 0x" << hex << BaseAddress << ": " << dec;
			}
			else if (yesno == 'n')
			{
				cout << "Input Memory Address: ";
			}
			cin >> inputaddress;
			DWORD readaddress = strtoul(inputaddress.c_str(), NULL, 16);
			cout << readaddress << endl;
			bool isValidMemory;
			if (yesno == 'y') {
				isValidMemory = isValidMemoryAddress(BaseAddress + readaddress, pHandle);
			}
			else if (yesno == 'n')
			{
				isValidMemory = isValidMemoryAddress(readaddress, pHandle);
			}
			cout << "valid address:" << isValidMemory << endl;
			if (isValidMemory == 1)
			{
				cout << "Expected Data Type:" << endl << "1. Byte" << endl << "2. Array Of Bytes" << endl << "3. Integer" << endl << "4. Double" << endl << "5. Float" << endl << "6. String" << endl << "7. Boolean" << endl << "8. Dword" << endl;
				int datatypeindex;
				cin >> datatypeindex;
				if (yesno == 'y') {
					IdentifyDataType(pHandle, BaseAddress + readaddress, datatypeindex);
				}
				else if (yesno == 'n') {
					IdentifyDataType(pHandle, readaddress, datatypeindex);
				}
				Sleep(1000);
			}
			waitforkeypress();
			BackToMenu();
		}
		else if (choice == 2) {
			//write memory address
			char writeyesno;
			cout << "Write from Base Address?(y/n): ";
			cin >> writeyesno;
			if (writeyesno == 'y')
			{
				cout << "Input memory address 0x" << hex << BaseAddress << " + :" << dec;
			}
			else if (writeyesno == 'n')
			{
				cout << "Input memory address: ";
			}
			string writeaddrstr;
			cin >> writeaddrstr;
			DWORD writeaddress = strtoul(writeaddrstr.c_str(), NULL, 16);
			bool isValidMemory = false;
			if (writeyesno == 'y') {
				isValidMemory = isValidMemoryAddress(BaseAddress + writeaddress, pHandle);
			}
			else if (writeyesno == 'n')
			{
				isValidMemory = isValidMemoryAddress(writeaddress, pHandle);
			}
			if (isValidMemory == 1)
			{
				cout << "Data Type:" << endl << "1. Byte" << endl << "2. Array Of Bytes" << endl << "3. Integer" << endl << "4. Double" << endl << "5. Float" << endl << "6. String" << endl << "7. Boolean" << endl;
				int datatypeindex;
				cin >> datatypeindex;
				cout << "Input Value to Write to Address" << endl;
				if (datatypeindex == 2) {
					cout << "If data type is an array of bytes, press space between each value" << endl;
				}
				string inputvalue;
				cin >> inputvalue;
				if (writeyesno == 'y') {
					WriteToMemory((BaseAddress + writeaddress), pHandle, datatypeindex, inputvalue);
					cout << "Value of: " << inputvalue << " applied to address of: " << (BaseAddress + writeaddress) << endl;
				}
				else if (writeyesno == 'n') {
					WriteToMemory((writeaddress), pHandle, datatypeindex, inputvalue);
					cout << "Value of: " << inputvalue << " applied to address of: " << (writeaddress) << endl;
				}

			}
			else
			{
				cout << "Address Given is not valid" << endl;
			}
			waitforkeypress();
			BackToMenu();
		}
		else if (choice == 3) {
			//scan memory from value
			ScanMemory(pHandle,BaseAddress);
			waitforkeypress();
			BackToMenu();
		}
		else if (choice == 4)
		{
			string add1;
			cout << "Enter an address: ";
			cin >> add1;
			DWORD add1converted = strtoul(add1.c_str(), NULL, 16);
			string add2;
			cout << "Enter an address to add: ";
			cin >> add2;
			DWORD add2converted = strtoul(add2.c_str(), NULL, 16);
			DWORD product = add1converted + add2converted;
			cout << "sum of addresses: " << hex << "0x" << uppercase << product << endl;
			waitforkeypress();
			BackToMenu();
		}
		else {
			cout << "choose a valid menu number" << endl;
			choice = 0;
		}

	}
	CloseHandle(pHandle);
	return 0;
}