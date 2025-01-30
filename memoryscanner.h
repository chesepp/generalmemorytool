#pragma once
#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <typeinfo>

using namespace std;

template <typename T>
vector<uintptr_t> MemoryScanner(HANDLE hProcess, DWORD baseAddress, T targetValue) {
    vector<uintptr_t> matchingAddresses;
    MEMORY_BASIC_INFORMATION mbi;
    uintptr_t currentAddress = baseAddress;

    while (VirtualQueryEx(hProcess, reinterpret_cast<LPCVOID>(currentAddress), &mbi, sizeof(mbi)) == sizeof(mbi)) {
        if (mbi.State == MEM_COMMIT && (mbi.Protect & (PAGE_READWRITE | PAGE_READONLY))) {
            SIZE_T regionSize = mbi.RegionSize;
            vector<BYTE> buffer(regionSize);
            SIZE_T bytesRead;

            if (ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(currentAddress), buffer.data(), buffer.size(), &bytesRead)) {
                for (SIZE_T i = 0; i < bytesRead - sizeof(T); i++) {
                    T* value = reinterpret_cast<T*>(&buffer[i]);
                    if (*value == targetValue) {
                        matchingAddresses.push_back(currentAddress + i);
                    }
                }
            }
            else {
                cerr << "Failed to read memory at address 0x" << hex << currentAddress << ". Error: " << GetLastError() << endl;
            }
        }
        currentAddress += mbi.RegionSize;
    }

    return matchingAddresses;
}
template <typename T>
vector<uintptr_t> NextScanner(HANDLE hProcess, const vector<uintptr_t>& addresses, T targetValue) {
    vector<uintptr_t> matchingAddresses;
    SIZE_T bytesRead;
    T value;

    for (uintptr_t address : addresses) {
        if (ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(address), &value, sizeof(T), &bytesRead) && bytesRead == sizeof(T)) {
            if (value == targetValue) {
                matchingAddresses.push_back(address);
            }
        }
    }

    return matchingAddresses;
}
vector<uintptr_t> ScanMemory(HANDLE hProcess, DWORD BaseAddress, int choice, int scanType) {
    vector<uintptr_t> results;
    
    if (scanType == 1) {
        switch (choice) {
        case 1: {
            int target;
            cout << "Enter the integer value to scan for: ";
            cin >> target;
            results = MemoryScanner<int>(hProcess, BaseAddress, target);

            if (!results.empty()) {
                cout << "Found matching addresses:\n";
                for (uintptr_t addr : results) {
                    cout << "0x" << hex << addr << endl;
                }
                int resultsize = results.size();
                cout << "Found " << dec << resultsize << " matching addresses : \n";
            }
            else {
                cout << "No matching addresses found." << endl;
            }
            break;
        }
        case 2: {
            float target;
            cout << "Enter the float value to scan for: ";
            cin >> target;
            results = MemoryScanner<float>(hProcess, BaseAddress, target);

            if (!results.empty()) {
                cout << "Found matching addresses:\n";
                for (uintptr_t addr : results) {
                    cout << "0x" << hex << addr << endl;
                }
                int resultsize = results.size();
                cout << "Found " << dec << resultsize << " matching addresses : \n";
            }
            else {
                cout << "No matching addresses found." << endl;
            }
            break;
        }
        case 3: {
            double target;
            cout << "Enter the double value to scan for: ";
            cin >> target;
            results = MemoryScanner<double>(hProcess, BaseAddress, target);

            if (!results.empty()) {
                cout << "Found matching addresses:\n";
                for (uintptr_t addr : results) {
                    cout << "0x" << hex << addr << endl;
                }
                int resultsize = results.size();
                cout << "Found " << dec << resultsize << " matching addresses : \n";
            }
            else {
                cout << "No matching addresses found." << endl;
            }
            break;
        }
        case 4: {
            BYTE target;
            cout << "Enter the byte value to scan for (0-255): ";
            int temp;
            cin >> temp;
            target = static_cast<BYTE>(temp);
            results = MemoryScanner<BYTE>(hProcess, BaseAddress, target);

            if (!results.empty()) {
                cout << "Found matching addresses:\n";
                for (uintptr_t addr : results) {
                    cout << "0x" << hex << addr << endl;
                }
                int resultsize = results.size();
                cout << "Found " << dec << resultsize << " matching addresses : \n";
            }
            else {
                cout << "No matching addresses found." << endl;
            }
            break;
        }
        case 5: {
            string target;
            cout << "Enter the string value to scan for: ";
            cin >> target;
            results = MemoryScanner<string>(hProcess, BaseAddress, target);
            if (!results.empty()) {
                cout << "Found matching addresses:" << endl;
                for (uintptr_t addr : results) {
                    cout << "0x" << hex << addr << endl;
                }
                int resultsize = results.size();
                cout << "Found " << dec << resultsize << " matching addresses : \n";
            }
            else {
                cout << "No matching addresses found." << endl;
            }
            break;
        }
        default:
            cout << "Invalid choice." << endl;
            break;
        }
    }
    return results;
}
