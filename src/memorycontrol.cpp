#include "memorycontrol.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include <tchar.h>
#include <tlhelp32.h>
using namespace std;
namespace memorycontrol
{
    DWORD pid;
    HANDLE hProcess;
    ADDR hModule;
    SIZE_T moduleSize;

    UINT get_pid(CHAR *processName)
    {
        WCHAR *wProcessName = new WCHAR[strlen(processName) + 1]{0};
        mbstowcs(wProcessName, processName, strlen(processName));
        // 创建进程快照
        HANDLE hProcessSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); // 进程快照
        if (hProcessSnapshot == INVALID_HANDLE_VALUE)
        {
            MessageBoxW(NULL, L"创建进程快照失败", L"错误", MB_ICONERROR | MB_OK);
            throw L"创建进程快照失败";
        }
        PROCESSENTRY32W process;                  // 进程结构体
        process.dwSize = sizeof(PROCESSENTRY32W); // 进程结构体长度
        if (!Process32FirstW(hProcessSnapshot, &process))
        {
            MessageBoxW(NULL, L"获取进程失败", L"错误", MB_ICONERROR | MB_OK);
            throw L"获取进程快照失败";
        }
        DWORD _pid = 0;
        do
        {
            if (wcsicmp(process.szExeFile, wProcessName) == 0) // 查找进程
            {
                _pid = process.th32ProcessID;
                break;
            }
        } while (Process32NextW(hProcessSnapshot, &process));
        if (_pid == 0)
        {
            WCHAR msg[MAX_PATH] = L"没有找到";
            wcsncat(msg, wProcessName, wcslen(wProcessName));
            wcscat(msg, L"进程。");
            MessageBoxW(NULL, msg, L"失败", MB_ICONERROR | MB_OK);
        }
        delete[] wProcessName;
        return _pid;
    }

    /**
     * @brief 获取hProcess
     *
     * @param pid
     * @return HANDLE
     */
    HANDLE get_hProcess(DWORD pid)
    {
        return OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    }

    /**
     * @brief Get the hModule object
     *
     * @param pid
     * @param moduleName
     * @param moduleSize
     * @return HMODULE
     */
    HMODULE get_hModule(DWORD pid, CHAR *moduleName, SIZE_T *moduleSize)
    {
        WCHAR *wModuleName = new WCHAR[strlen(moduleName) + 1]{0};
        mbstowcs(wModuleName, moduleName, strlen(moduleName));
        HANDLE hModuleSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
        if (hModuleSnapshot == INVALID_HANDLE_VALUE)
        {
            MessageBoxW(NULL, L"创建模块快照失败", L"错误", MB_ICONERROR | MB_OK);
            throw L"创建模块快照失败";
        }
        MODULEENTRY32W module;                  // 模块结构体
        module.dwSize = sizeof(MODULEENTRY32W); // 模块结构体长度
        if (!Module32FirstW(hModuleSnapshot, &module))
        {
            MessageBoxW(NULL, L"获取模块失败", L"错误", MB_ICONERROR | MB_OK);
            throw L"获取模块快照失败";
        }
        HMODULE hModule = 0;
        do
        {
            if (wcsicmp(module.szModule, wModuleName) == 0) // 查找模块
            {
                *moduleSize = module.modBaseSize;
                hModule = module.hModule;
                break;
            }
        } while (Module32NextW(hModuleSnapshot, &module));
        delete[] wModuleName;
        return hModule;
    }

    void close_hanle(HANDLE handle)
    {
        CloseHandle(handle);
    }

    template <typename T>
    inline T read_memory(ADDR address)
    {
        T buffer;
        ReadProcessMemory(hProcess, (LPCVOID)address, &buffer, sizeof(T), nullptr);
        return buffer;
    }

    // 实例化一个read_memory模板函数
    // template ULONGLONG read_memory<ULONGLONG>(ADDR address);

    template <typename T>
    inline void write_memory(ADDR address, T buffer)
    {
        WriteProcessMemory(hProcess, (LPVOID)address, &buffer, sizeof(T), nullptr);
    }

    CHAR read_char(ADDR address)
    {
        return read_memory<CHAR>(address);
    }

    UCHAR read_uchar(ADDR address)
    {
        return read_memory<UCHAR>(address);
    }

    SHORT read_short(ADDR address)
    {
        return read_memory<SHORT>(address);
    }

    USHORT read_ushort(ADDR address)
    {
        return read_memory<USHORT>(address);
    }

    INT read_int(ADDR address)
    {
        return read_memory<INT>(address);
    }

    UINT read_uint(ADDR address)
    {
        return read_memory<UINT>(address);
    }

    LONGLONG read_longlong(ADDR address)
    {
        return read_memory<LONGLONG>(address);
    }

    ULONGLONG read_ulonglong(ADDR address)
    {
        return read_memory<ULONGLONG>(address);
    }

    FLOAT read_float(ADDR address)
    {
        return read_memory<FLOAT>(address);
    }

    DOUBLE read_double(ADDR address)
    {
        return read_memory<DOUBLE>(address);
    }

    void write_char(ADDR address, CHAR value)
    {
        write_memory<CHAR>(address, value);
    }

    void write_uchar(ADDR address, UCHAR value)
    {
        write_memory<UCHAR>(address, value);
    }

    void write_short(ADDR address, SHORT value)
    {
        write_memory<SHORT>(address, value);
    }

    void write_ushort(ADDR address, USHORT value)
    {
        write_memory<USHORT>(address, value);
    }

    void write_int(ADDR address, INT value)
    {
        write_memory<INT>(address, value);
    }

    void write_uint(ADDR address, UINT value)
    {
        write_memory<UINT>(address, value);
    }

    void write_longlong(LONGLONG address, LONG value)
    {
        write_memory<LONGLONG>(address, value);
    }

    void write_ulonglong(ADDR address, ULONGLONG value)
    {
        write_memory<ULONGLONG>(address, value);
    }

    void write_float(ADDR address, FLOAT value)
    {
        write_memory<FLOAT>(address, value);
    }

    void write_double(ADDR address, DOUBLE value)
    {
        write_memory<DOUBLE>(address, value);
    }

    void read_bytes(ADDR address, void *buffer, SIZE_T size)
    {
        ReadProcessMemory(hProcess, (LPCVOID)address, buffer, (SIZE_T)size, nullptr);
    }

    void write_bytes(ADDR address, void *buffer, SIZE_T size)
    {
        WriteProcessMemory(hProcess, (LPVOID)address, buffer, (SIZE_T)size, nullptr);
    }

    template <typename T>
    inline T pointer_memory(ADDR *addrArray, INT length)
    {
        ADDR addr = addrArray[0];
        for (int i = 1; i < length; i++)
        {
            addr = read_memory<ADDR>(addr) + addrArray[i];
        }
        return read_memory<T>(addr);
    }

    template <typename T>
    inline void pointer_memory(ADDR *addrArray, INT length, T value)
    {
        ADDR addr = addrArray[0];
        for (int i = 1; i < length; i++)
        {
            addr = read_memory<ADDR>(addr) + addrArray[i];
        }
        write_memory<T>(addr, value);
    }

    CHAR pread_char(ADDR *addrArray, INT length)
    {
        return pointer_memory<CHAR>(addrArray, length);
    }

    UCHAR pread_uchar(ADDR *addrArray, INT length)
    {
        return pointer_memory<UCHAR>(addrArray, length);
    }

    SHORT pread_short(ADDR *addrArray, INT length)
    {
        return pointer_memory<SHORT>(addrArray, length);
    }

    USHORT pread_ushort(ADDR *addrArray, INT length)
    {
        return pointer_memory<USHORT>(addrArray, length);
    }

    INT pread_int(ADDR *addrArray, INT length)
    {
        return pointer_memory<INT>(addrArray, length);
    }

    UINT pread_uint(ADDR *addrArray, INT length)
    {
        return pointer_memory<UINT>(addrArray, length);
    }

    LONGLONG pread_longlong(ADDR *addrArray, INT length)
    {
        return pointer_memory<LONGLONG>(addrArray, length);
    }

    ULONGLONG pread_ulonglong(ADDR *addrArray, INT length)
    {
        return pointer_memory<ULONGLONG>(addrArray, length);
    }

    FLOAT pread_float(ADDR *addrArray, INT length)
    {
        return pointer_memory<FLOAT>(addrArray, length);
    }

    DOUBLE pread_double(ADDR *addrArray, INT length)
    {
        return pointer_memory<DOUBLE>(addrArray, length);
    }

    void pwrite_char(ADDR *addrArray, INT length, CHAR value)
    {
        pointer_memory<CHAR>(addrArray, length, value);
    }

    void pwrite_uchar(ADDR *addrArray, INT length, UCHAR value)
    {
        pointer_memory<UCHAR>(addrArray, length, value);
    }

    void pwrite_short(ADDR *addrArray, INT length, SHORT value)
    {
        pointer_memory<SHORT>(addrArray, length, value);
    }

    void pwrite_ushort(ADDR *addrArray, INT length, USHORT value)
    {
        pointer_memory<USHORT>(addrArray, length, value);
    }

    void pwrite_int(ADDR *addrArray, INT length, INT value)
    {
        pointer_memory<INT>(addrArray, length, value);
    }

    void pwrite_uint(ADDR *addrArray, INT length, UINT value)
    {
        pointer_memory<UINT>(addrArray, length, value);
    }

    void pwrite_longlong(ADDR *addrArray, INT length, LONGLONG value)
    {
        pointer_memory<LONGLONG>(addrArray, length, value);
    }

    void pwrite_ulonglong(ADDR *addrArray, INT length, ULONGLONG value)
    {
        pointer_memory<ULONGLONG>(addrArray, length, value);
    }

    void pwrite_float(ADDR *addrArray, INT length, FLOAT value)
    {
        pointer_memory<FLOAT>(addrArray, length, value);
    }

    void pwrite_double(ADDR *addrArray, INT length, DOUBLE value)
    {
        pointer_memory<DOUBLE>(addrArray, length, value);
    }

    template <typename T>
    inline T get_address(T *addrArray, INT length)
    {
        T addr = addrArray[0];
        for (int i = 1; i < length; i++)
        {
            addr = read_memory<T>(addr) + addrArray[i];
        }
        return addr;
    }

    void pread_bytes(ADDR *addrArray, INT length, void *buffer, SIZE_T size)
    {
        ReadProcessMemory(hProcess, (LPCVOID)get_address<ADDR>(addrArray, length), buffer, size, NULL);
    }

    void pwrite_bytes(ADDR *addrArray, INT length, void *buffer, SIZE_T size)
    {
        WriteProcessMemory(hProcess, (LPVOID)get_address<ADDR>(addrArray, length), buffer, size, NULL);
    }

    vector<char *> split(char *str, const char *s)
    {
        vector<char *> temp;
        char *res = strtok(str, s);
        while (res != NULL)
        {
            temp.push_back(res);
            res = strtok(NULL, s);
        }
        return temp;
    }

    vector<SHORT> bytecodearray2bytenumberarray(vector<char *> BYTEcodearray)
    {
        vector<SHORT> temp;
        for (unsigned int i = 0; i < BYTEcodearray.size(); i++)
        {
            if (stricmp(BYTEcodearray[i], "?") == 0 || stricmp(BYTEcodearray[i], "??") == 0)
            {
                temp.push_back(-1);
            }
            else
            {
                temp.push_back((SHORT)std::stoi(BYTEcodearray[i], 0, 16));
            }
        }
        return temp;
    }
}
