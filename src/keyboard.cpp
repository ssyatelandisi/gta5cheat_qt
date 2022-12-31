#include "keyboard.h"
#include "winuser.h"
namespace keyboard
{
    /**
     * @brief 监听按键
     *
     * @return UINT
     */
    UINT keyboard_watch()
    {
        {
            if (GetAsyncKeyState(VK_F1) & 0x8000)
            {
                return VK_F1;
            }
            else if (GetAsyncKeyState(VK_F3) & 0x8000)
            {
                return VK_F3;
            }
            else if (GetAsyncKeyState(VK_F4) & 0x8000)
            {
                return VK_F4;
            }
            else if (GetAsyncKeyState(VK_F5) & 0x8000)
            {
                return VK_F5;
            }
            else if (GetAsyncKeyState(VK_F6) & 0x8000)
            {
                return VK_F6;
            }
            else if (GetAsyncKeyState(VK_F7) & 0x8000 && !(GetAsyncKeyState(VK_CONTROL) & 0x8000))
            {
                return VK_F7;
            }
            else if (GetAsyncKeyState(VK_CONTROL) & 0x8000 && GetAsyncKeyState(VK_F7) & 0x8000)
            {
                return VK_CONTROL << 8 | VK_F7;
            }
            else if (GetAsyncKeyState(VK_F8) & 0x8000)
            {
                return VK_F8;
            }
            else if (GetAsyncKeyState(VK_CONTROL) & 0x8000 && GetAsyncKeyState(VK_F9) & 0x8000)
            {
                return VK_CONTROL << 8 | VK_F9;
            }
            else if (GetAsyncKeyState(VK_CONTROL) & 0x8000 && GetAsyncKeyState(VK_F11) & 0x8000)
            {
                return VK_CONTROL << 8 | VK_F11;
            }
            else if (GetAsyncKeyState(VK_CONTROL) & 0x8000 && GetAsyncKeyState(VK_OEM_3) & 0x8000) // LCTRL+~
            {
                return VK_CONTROL << 8 | VK_OEM_3;
            }
            else if (GetAsyncKeyState(VK_CONTROL) & 0x8000 && GetAsyncKeyState(VK_DELETE) & 0x8000)
            {
                return VK_CONTROL << 8 | VK_DELETE;
            }
            else if (GetAsyncKeyState('Q') & 0x8000)
            {
                return 'Q';
            }
            else if (GetAsyncKeyState('F') & 0x8000)
            {
                return 'F';
            }
            else if (GetAsyncKeyState(VK_CONTROL) & 0x8000 && GetAsyncKeyState('H') & 0x8000)
            {
                return VK_CONTROL << 8 | 'H';
            }
            else if (GetAsyncKeyState(VK_CONTROL) & 0x8000 && GetAsyncKeyState('R') & 0x8000)
            {
                return VK_CONTROL << 8 | 'R';
            }
            else if (GetAsyncKeyState(VK_CONTROL) & 0x8000 && GetAsyncKeyState('1') & 0x8000)
            {
                return VK_CONTROL << 8 | '1';
            }
            else if (GetAsyncKeyState(VK_CONTROL) & 0x8000 && GetAsyncKeyState('2') & 0x8000)
            {
                return VK_CONTROL << 8 | '2';
            }
            else if (GetAsyncKeyState(VK_DIVIDE) & 0x8000) // NUM /
            {
                return VK_DIVIDE;
            }
            else if (GetAsyncKeyState(VK_MULTIPLY) & 0x8000) // NUM *
            {
                return VK_MULTIPLY;
            }
            return 0;
        }
    }

    void keydown(DWORD scanKey)
    {
        INPUT in;
        in.type = INPUT_KEYBOARD;
        in.ki.wScan = scanKey;
        in.ki.dwFlags = 0;
        in.ki.time = 0;
        in.ki.dwExtraInfo = 0;
        in.ki.dwFlags = KEYEVENTF_SCANCODE; // 如果指定了，wScan会识别按键，而wVk会被忽略。
        SendInput(1, &in, sizeof(in));
    }
    
    void keyup(DWORD scanKey)
    {
        INPUT in;
        in.type = INPUT_KEYBOARD;
        in.ki.wScan = scanKey;
        in.ki.dwFlags = 0;
        in.ki.time = 0;
        in.ki.dwExtraInfo = 0;
        in.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP; // 如果指定了，wScan会识别按键，而wVk会被忽略。 | 如果指定了，则说明该键正在被释放。如果没有指定，则表示该键正在被按下。
        SendInput(1, &in, sizeof(in));
    }
}