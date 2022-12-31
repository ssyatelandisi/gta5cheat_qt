#ifndef KEYBOARD_H
#define KEYBOARD_H
#include <windows.h>

namespace keyboard
{
    UINT keyboard_watch();
    void keydown(DWORD scanKey);
    void keyup(DWORD scanKey);
} // namespace name

#endif