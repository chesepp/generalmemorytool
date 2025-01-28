#include <iostream>
#include <windows.h>
#include <limits>
#include <string>
#include <sstream>
#include <vector>
using namespace std;
enum class DataType
{
    Byte,
    Integer,
    Float,
    Double,
    String,
    Boolean,
    ArrayOfBytes,
    Unknown
};
DataType datatypearray[7] = {
    DataType::Byte, 
    DataType::ArrayOfBytes, 
    DataType::Integer, 
    DataType::Double, 
    DataType::Float, 
    DataType::String, 
    DataType::Boolean
};

// Check if the value looks like a valid pointer (non-zero and within reasonable address ranges).

DataType IdentifyDataType(HANDLE hProcess, DWORD_PTR address, int datatype)
{
    SIZE_T bytesRead;
    BYTE byteValue;
    SHORT shortValue;
    int intValue;
    float floatValue;
    double doubleValue;
    bool boolValue;
    char stringValue[256] = { 0 }; // For string detection
    BYTE byteArray[10] = { 0 };    // For byte array detection (default: 10 bytes)
    DWORD dwordValue;

    
    if (datatype == 1)
    {
        ReadProcessMemory(hProcess, LPCVOID(address), &byteValue, sizeof(BYTE), &bytesRead);
        cout << "byte value: " << static_cast<int>(byteValue) << endl;
        return DataType::Byte;
    }
    else if (datatype == 2)
    {
        ReadProcessMemory(hProcess, (LPCVOID)(address), &byteArray, sizeof(byteArray), &bytesRead);

        cout << "array of bytes: " << endl;
        for (size_t i = 0; i < bytesRead; ++i)
        {
            cout << hex << uppercase << static_cast<int>(byteArray[i]) << " ";
        }
        cout << dec << endl;
        return DataType::ArrayOfBytes;
    }
    else if (datatype == 3)
    {
        ReadProcessMemory(hProcess, (LPCVOID)(address), &intValue, sizeof(int), &bytesRead);
        cout << "integer value: " << intValue << endl;
        return DataType::Integer;
    }
    else if (datatype == 4)
    {
        ReadProcessMemory(hProcess, (LPCVOID)(address), &doubleValue, sizeof(double), &bytesRead);
        cout << "double value: " << doubleValue << endl;
        return DataType::Double;
    }
    else if (datatype == 5)
    {
        ReadProcessMemory(hProcess, (LPCVOID)(address), &floatValue, sizeof(float), &bytesRead);
        cout << "float value: " << floatValue << endl;
        return DataType::Float;
    }
    else if (datatype == 6)
    {
        ReadProcessMemory(hProcess, (LPCVOID)(address), &stringValue, sizeof(stringValue), &bytesRead);
        cout << "string value: " << stringValue << endl;
        return DataType::String;
    }
    else if (datatype == 7)
    {
        ReadProcessMemory(hProcess, (LPCVOID)(address), &boolValue, sizeof(bool), &bytesRead);
        cout << "boolean value: " << (boolValue ? "True" : "False") << endl;
        return DataType::Boolean;
    }
    else if (datatype == 8)
    {
        ReadProcessMemory(hProcess, (LPCVOID)(address), &dwordValue, sizeof(dwordValue), &bytesRead);
        cout << "DWORD address: " << hex << dwordValue << dec << endl;
    }
    else
    {
        cout << "unknown data type" << endl;
        return DataType::Unknown;
    }
}
void WriteToMemory(DWORD address, HANDLE pHandle, int datatype, string value)
{   
    SIZE_T bytesWritten;
    BYTE byteValue;
    SHORT shortValue;
    int intValue;
    float floatValue;
    double doubleValue;
    bool boolValue;
    BYTE byteArray[10] = { 0 };
    
    DataType data;
    if (datatype > 0 && datatype <= 7) {
        data = datatypearray[datatype - 1];
        if (data == DataType::Byte)
        {
            byteValue = static_cast<BYTE>(stoi(value));
            WriteProcessMemory(pHandle, LPVOID(address), &byteValue, sizeof(byteValue), &bytesWritten);
        }
        else if (data == DataType::ArrayOfBytes)
        {
            istringstream stream(value);
            string byte;
            vector<string> strbytevector;
            while (stream >> byte)
            {
                strbytevector.push_back(byte);
            }
            for (int i = 0; i < strbytevector.size(); i++)
            {
                byteArray[i] = static_cast<BYTE>(stoi(strbytevector[i]));
            }
            WriteProcessMemory(pHandle, LPVOID(address), &byteArray, sizeof(byteArray), &bytesWritten);
        }
        else if (data == DataType::Integer)
        {
            intValue = stoi(value);
            WriteProcessMemory(pHandle, LPVOID(address), &intValue, sizeof(intValue), &bytesWritten);
        }
    }
    else
    {
        cout << "choose a valid data type" << endl;
    }
}