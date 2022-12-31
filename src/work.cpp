#include "work.h"
#include "staticdata.h"
#include "keyboard.h"
#include <QThread>

WorkSuspendProcess::WorkSuspendProcess(QObject *parent, Gtav *g)
    : QObject(parent)
{
    gtav = g;
}

WorkSuspendProcess::~WorkSuspendProcess()
{
    gtav = nullptr;
}

void WorkSuspendProcess::slotWork()
{
    gtav->suspend_process();
}

WorkLuckyWheel::WorkLuckyWheel(QObject *parent, Gtav *g)
    : QObject(parent)
{
    gtav = g;
}

WorkLuckyWheel::~WorkLuckyWheel()
{
    gtav = nullptr;
}

void WorkLuckyWheel::slotWork()
{
    while (true)
    {
        gtav->set_luckyWheel();
        QThread::msleep(200);
    }
}

WorkStatHax::WorkStatHax(QObject *parent, Ui_App *u, Gtav *g)
    : QObject(parent)
{
    ui = u;
    gtav = g;
    defaultContent = ui->pushButton_statSubmit->text();
}

WorkStatHax::~WorkStatHax()
{
    ui = nullptr;
    gtav = nullptr;
}

void WorkStatHax::slotWork()
{
    if (ui->textEdit_statText->toPlainText().trimmed().isEmpty())
    {
        return;
    }
    emit signalStarted();
    QStringList rows = ui->textEdit_statText->toPlainText().trimmed().replace("INT32\n", "").split("\n");
    for (int i = 0; i < rows.count(); i++)
    {
        if (i % 2 == 0)
        {
            emit signalSet_statSubmit_text(QString::number(i / 2 + 1) + "/" + QString::number(rows.count() / 2));
        }
        else if (i % 2 == 1)
        {
            gtav->stat_write(
                rows[i - 1]
                    .replace("$", "")
                    .toLatin1()
                    .data(),
                rows[i].toInt());
        }
    }
    emit signalSet_statSubmit_text(defaultContent);
    emit signalFinished();
}

WorkCheckBoxWatch::WorkCheckBoxWatch(QObject *parent, Ui_App *u, QStandardItemModel *m, Gtav *g)
    : QObject(parent)
{
    ui = u;
    model = m;
    gtav = g;
}

WorkCheckBoxWatch::~WorkCheckBoxWatch()
{
    ui = nullptr;
    model = nullptr;
    gtav = nullptr;
}

void WorkCheckBoxWatch::slotWork()
{
    while (true)
    {
        gtav->set_godMode(ui->checkBox_godMode->isChecked());
        gtav->set_infiniteAmmo(ui->checkBox_infiniteAmmo->isChecked());
        gtav->set_seatbelt(ui->checkBox_seatbelt->isChecked());
        gtav->set_vehicleGodMode(ui->checkBox_vehicleGodMode->isChecked());
        if (ui->checkBox_antiAFK->isChecked())
        {
            gtav->antiAFK(true);
        }
        if (ui->checkBox_autoCure->isChecked())
        {
            gtav->set_health(gtav->get_maxHealth());
            gtav->set_armor(gtav->get_maxArmor());
        }
        if (ui->checkBox_continuousTeleportation->isChecked())
        {
            int row = ui->tableView_teleport->currentIndex().row();
            FLOAT position[3] = {0};
            position[0] = model->item(row, 1)->text().toFloat();
            position[1] = model->item(row, 2)->text().toFloat();
            position[2] = model->item(row, 3)->text().toFloat();
            gtav->teleport_enemy_npc(position);
        }
        QThread::msleep(300);
    }
}

WorkHotkey::WorkHotkey(QObject *parent, Ui_App *u, Gtav *g)
    : QObject(parent)
{
    ui = u;
    gtav = g;
}

WorkHotkey::~WorkHotkey()
{
    ui = nullptr;
    gtav = nullptr;
}

/**
 * @brief 按下按键
 *
 * @param scanKey 键盘扫描码 [可通过MapVirtualKeyA('a',MAPVK_VK_TO_VSC)转换获得: a->0x1E]
 */
void WorkHotkey::keydown(WORD scanKey)
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

/**
 * @brief 释放按键
 *
 * @param scanKey 键盘扫描码 [可通过MapVirtualKeyA('a',MAPVK_VK_TO_VSC)转换获得: a->0x1E]
 */
void WorkHotkey::keyup(WORD scanKey)
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

void WorkHotkey::slotWork()
{
    UINT code = 0;
    while (true)
    {
        code = keyboard::keyboard_watch(); // CTRL+1 物理方法卡末日1,2
        if (code == (VK_CONTROL << 8 | '1'))
        {
            emit signalDuu();
            unsigned int SPACE = 0x39; // MapVirtualKeyA(VK_SPACE, MAPVK_VK_TO_VSC);
            unsigned int D = 0x20;     // MapVirtualKeyA('D', MAPVK_VK_TO_VSC);
            keyboard::keydown(SPACE);
            QThread::msleep(30);
            keyboard::keydown(D);
            QThread::msleep(30);
            keyboard::keyup(SPACE);
            QThread::msleep(30);
            keyboard::keyup(D);
            QThread::msleep(150);
        }
        else if (code == (VK_CONTROL << 8 | '2')) // CTRL+2 物理方法卡末日3
        {
            emit signalDuu();
            unsigned int SPACE = 0x39; // MapVirtualKeyA(VK_SPACE, MAPVK_VK_TO_VSC);
            unsigned int S = 0x1F;     // MapVirtualKeyA('S', MAPVK_VK_TO_VSC);
            unsigned int D = 0x20;     // MapVirtualKeyA('D', MAPVK_VK_TO_VSC);
            keyboard::keydown(SPACE);
            QThread::msleep(100);
            keyboard::keydown(S);
            QThread::msleep(30);
            keyboard::keydown(D);
            QThread::msleep(30);
            keyboard::keyup(SPACE);
            QThread::msleep(30);
            keyboard::keyup(D);
            QThread::msleep(30);
            keyboard::keyup(S);
            QThread::msleep(150);
        }
        else if (code == (VK_CONTROL << 8 | 'R')) // CTRL+R
        {
            ui->pushButton_fillCurrentAmmo->click();
            QThread::msleep(150);
        }
        else if (code == (VK_CONTROL << 8 | VK_DELETE)) // CTRL+DEL
        {
            ui->pushButton_killYourself->click();
            QThread::msleep(150);
        }
        else if (code == VK_F1) // F1
        {
            if (ui->checkBox_killGTA->isChecked())
            {
                emit signalDuu();
                gtav->kill_process();
            }
            QThread::msleep(150);
        }
        else if (code == VK_F3) // F3
        {
            ui->pushButton_moveForward->click();
            QThread::msleep(150);
        }
        else if (code == VK_F4) // F4
        {
            ui->pushButton_teleportToWaypoint->click();
            QThread::msleep(150);
        }
        else if (code == VK_F5) // F5
        {
            ui->pushButton_teleportToObjective->click();
            QThread::msleep(150);
        }
        else if (code == VK_F6) // F6
        {
            emit signalDuu();
            gtav->set_vehicleAbility(StaticData::vehicleAbilities[ui->comboBox_vehicleAbilities->currentIndex()].value.toInt());
            QThread::msleep(150);
        }
        else if (code == VK_F7) // F7
        {
            ui->checkBox_godMode->setChecked(true);
            QThread::msleep(150);
        }
        else if (code == (VK_CONTROL << 8 | VK_F7)) // CTRL+F7
        {
            ui->checkBox_godMode->setChecked(false);
            QThread::msleep(150);
        }
        else if (code == VK_F8) // F8
        {
            ui->pushButton_noCops->click();
            QThread::msleep(150);
        }
        else if (code == (VK_CONTROL << 8 | VK_F9)) // CTRL+F9
        {
            ui->pushButton_vehicleRepairing->click();
            QThread::msleep(150);
        }
        else if (code == (VK_CONTROL << 8 | VK_F11)) // CTRL+F11
        {
            emit signalDuu();
            gtav->change_session(StaticData::session[ui->comboBox_session->currentIndex()].value.toInt());
            QThread::msleep(150);
        }
        else if (code == (VK_CONTROL << 8 | VK_OEM_3)) // CTRL+~
        {
            emit signalDuu();
            gtav->spawn_vehicle(Gtav::joaat(StaticData::vehicles[ui->comboBox_vehicles->currentIndex()].value.toByteArray().data()));
            QThread::msleep(150);
        }
        else if (code == VK_DIVIDE) // /
        {
            ui->pushButton_killEnemyNPC->click();
            QThread::msleep(150);
        }
        else if (code == VK_MULTIPLY) // *
        {
            ui->pushButton_destroyEnemyVehicles->click();
            QThread::msleep(150);
        }
        else if (code == (VK_CONTROL << 8 | 'H')) // CTRL+H
        {
            emit signalHack();
            QThread::msleep(150);
        }
        else
        {
            QThread::msleep(50);
        }
    }
}
