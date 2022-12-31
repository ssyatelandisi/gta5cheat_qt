#include "app.h"

#include <QApplication>
int main(int argc, char *argv[])
{
    if (FindWindowW(L"Qt632QWindowIcon", L"GTAOL辅助") != 0)
    {
        MessageBoxW(NULL, L"已有一个GTAOL辅助处于开启状态。", L"失败", MB_ICONERROR | MB_OK);
        return 0;
    }
    QApplication a(argc, argv);
    App w;
    w.show();
    return a.exec();
}
