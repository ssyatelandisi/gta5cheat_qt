#ifndef APP_H
#define APP_H

#include "gtav.h"
#include "work.h"
#include <QPushButton>
#include <QSoundEffect>
#include <QStandardItemModel>
#include <QString>
#include <QThread>
#include <QWidget>
#include <windows.h>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class App;
}
QT_END_NAMESPACE

class App : public QWidget
{
    Q_OBJECT

public:
    App(QWidget *parent = nullptr);
    ~App();
    void comboxBox_init();
    void teleportTable_init();

private slots:
    void slotVehicleCode_textChanged();
    void slotCheckBox_stateChanged(QCheckBox *object);
    void slotPushButton_clicked(QWidget *object);
    void slotComboBox_changed(QWidget *object);
    void slotDuu(INT value = 0);
    void slotSet_statSubmit_enable();
    void slotSet_statSubmit_disable();
    void slotSet_statSubmit_text(QString content);
    void slotUpdate_cutTab();
    void slotTabWidgetChanged();
    void slotHack();

signals:
    void signalDuu(INT value = 0); // 播放声音信号
    void signalUpdate_cutTab();
    void signalWorkSuspend();
    void signalWorkLuckyWheel();
    void signalWorkStatHax();
    void signalWorkCheckBoxWatch();
    void signalWorkHotkey();

private:
    Ui::App *ui;
    QStandardItemModel *model;
    INT cutData[16] = {0};
    Gtav *gtav;
    QSoundEffect *soundEffect;
    WorkSuspendProcess *workSuspend;      // 挂起进程工作线程
    WorkLuckyWheel *workLuckyWheel;       // 幸运转盘工作线程
    WorkStatHax *workStatHax;             // StatHax工作线程
    WorkCheckBoxWatch *workCheckBoxWatch; // 复选框监听工作线程
    WorkHotkey *workHotkey;               // 热键监听工作线程
    QThread *threadSuspend;               // 挂起进程子线程
    QThread *threadLuckyWheel;            // 幸运转盘子线程
    QThread *threadStatHax;               // StatHax子线程
    QThread *threadCheckBoxWatch;         // 复选框监听子线程
    QThread *threadHotkey;                // 热键监听子线程
    QThread *threadCheatThread;           // 运行其他作弊功能子线程
};

#endif // APP_H
