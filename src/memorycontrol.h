#ifndef MEMORYCONTROL_H
#define MEMORYCONTROL_H
#include <windows.h>
#include <tlhelp32.h>
#include <vector>
#define len(T) (sizeof(T) / sizeof(T[0]))
typedef unsigned long long ADDR;
using std::vector;
namespace memorycontrol
{
    extern DWORD pid;         // 进程id
    extern HANDLE hProcess;   // 进程句柄
    extern ADDR hModule;      // 模块地址(句柄)
    extern SIZE_T moduleSize; // 模块大小
    UINT get_pid(CHAR *processName);
    HANDLE get_hProcess(DWORD pid);
    HMODULE get_hModule(DWORD pid, CHAR *moduleName, SIZE_T *moduleSize);
    void close_hanle(HANDLE handle);
    template <typename T>
    inline T read_memory(ADDR address);
    template <typename T>
    inline void write_memory(ADDR address, T buffer);
    CHAR read_char(ADDR address);
    UCHAR read_uchar(ADDR address);
    SHORT read_short(ADDR address);
    USHORT read_ushort(ADDR address);
    INT read_int(ADDR address);
    UINT read_uint(ADDR address);
    LONGLONG read_longlong(ADDR address);
    ULONGLONG read_ulonglong(ADDR address);
    FLOAT read_float(ADDR address);
    DOUBLE read_double(ADDR address);
    void write_char(ADDR address, CHAR value);
    void write_uchar(ADDR address, UCHAR value);
    void write_short(ADDR address, SHORT value);
    void write_ushort(ADDR address, USHORT value);
    void write_int(ADDR address, INT value);
    void write_uint(ADDR address, UINT value);
    void write_longlong(ADDR address, LONGLONG value);
    void write_ulonglong(ADDR address, ULONGLONG value);
    void write_float(ADDR address, FLOAT value);
    void write_double(ADDR address, DOUBLE value);
    void read_bytes(ADDR address, void *buffer, SIZE_T size);
    void write_bytes(ADDR address, void *buffer, SIZE_T size);
    template <typename T>
    inline T pointer_memory(ADDR *addrArray, INT length);
    template <typename T>
    inline void pointer_memory(ADDR *addrArray, INT length, T value);
    CHAR pread_char(ADDR *addrArray, INT length);
    UCHAR pread_uchar(ADDR *addrArray, INT length);
    SHORT pread_short(ADDR *addrArray, INT length);
    USHORT pread_ushort(ADDR *addrArray, INT length);
    INT pread_int(ADDR *addrArray, INT length);
    UINT pread_uint(ADDR *addrArray, INT length);
    LONGLONG pread_longlong(ADDR *addrArray, INT length);
    ULONGLONG pread_ulonglong(ADDR *addrArray, INT length);
    FLOAT pread_float(ADDR *addrArray, INT length);
    DOUBLE pread_double(ADDR *addrArray, INT length);
    void pwrite_char(ADDR *addrArray, INT length, CHAR value);
    void pwrite_uchar(ADDR *addrArray, INT length, UCHAR value);
    void pwrite_short(ADDR *addrArray, INT length, SHORT value);
    void pwrite_ushort(ADDR *addrArray, INT length, USHORT value);
    void pwrite_int(ADDR *addrArray, INT length, INT value);
    void pwrite_uint(ADDR *addrArray, INT length, UINT value);
    void pwrite_longlong(ADDR *addrArray, INT length, LONGLONG value);
    void pwrite_ulonglong(ADDR *addrArray, INT length, ULONGLONG value);
    void pwrite_float(ADDR *addrArray, INT length, FLOAT value);
    void pwrite_double(ADDR *addrArray, INT length, DOUBLE value);
    template <typename T>
    T get_address(T *addrArray, INT length);
    void pread_bytes(ADDR *addrArray, INT length, void *buffer, SIZE_T size);
    void pwrite_bytes(ADDR *addrArray, INT length, void *buffer, SIZE_T size);
    vector<char *> split(char *str, const char *s);
    vector<SHORT> bytecodearray2bytenumberarray(vector<char *> BYTEcodearray);
}
#endif