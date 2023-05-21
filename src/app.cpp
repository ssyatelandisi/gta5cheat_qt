// QT多个按钮信号绑定一个槽函数 https://zxfdog.blog.csdn.net/article/details/113696479
// 热键教程 http://cppdebug.com/archives/471
#include "app.h"
#include "gtav.h"
#include "staticdata.h"
#include "ui_app.h"
#include <windows.h>
// #include <QDebug>
#include <QDir>
#include <QFile>
#include <QSoundEffect>
#include <QStandardItemModel>

App::App(QWidget *parent)
    : QWidget(parent), ui(new Ui::App)
{
    ui->setupUi(this);
    model = new QStandardItemModel(0, 4, this);
    setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
    comboxBox_init();
    teleportTable_init();
    for (QPushButton *component : findChildren<QPushButton *>())
    {
        connect(component, &QPushButton::clicked, this, [=]
                { slotPushButton_clicked(component); });
    }
    for (QCheckBox *component : findChildren<QCheckBox *>())
    {
        connect(component, &QCheckBox::stateChanged, this, [=]()
                { slotCheckBox_stateChanged(component); });
    }
    for (QComboBox *component : findChildren<QComboBox *>())
    {
        connect(component, &QComboBox::currentIndexChanged, this, [=]()
                { slotComboBox_changed(component); });
    }
    connect(ui->lineEdit_vehicleCode, &QLineEdit::textChanged, this, &App::slotVehicleCode_textChanged);
    connect(this, &App::signalUpdate_cutTab, this, &App::slotUpdate_cutTab);
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &App::slotTabWidgetChanged);
    connect(this, &App::signalDuu, this, &App::slotDuu);
    QFile file("YimMenu.dll");
    if (!file.open(QIODevice::ReadOnly))
    {
        ui->pushButton_yimInject->deleteLater();
    }
    soundEffect = new QSoundEffect;
    gtav = new Gtav;
    workSuspend = new WorkSuspendProcess(nullptr, gtav);
    workLuckyWheel = new WorkLuckyWheel(nullptr, gtav);
    workStatHax = new WorkStatHax(nullptr, ui, gtav);
    workCheckBoxWatch = new WorkCheckBoxWatch(nullptr, ui, model, gtav);
    workHotkey = new WorkHotkey(nullptr, ui, gtav);
    threadSuspend = new QThread;
    threadLuckyWheel = new QThread;
    threadStatHax = new QThread;
    threadCheckBoxWatch = new QThread;
    threadHotkey = new QThread;
    workSuspend->moveToThread(threadSuspend);
    workLuckyWheel->moveToThread(threadLuckyWheel);
    workStatHax->moveToThread(threadStatHax);
    workCheckBoxWatch->moveToThread(threadCheckBoxWatch);
    workHotkey->moveToThread(threadHotkey);
    connect(workStatHax, &WorkStatHax::signalStarted, this, &App::slotSet_statSubmit_disable);
    connect(workStatHax, &WorkStatHax::signalFinished, this, &App::slotSet_statSubmit_enable);
    connect(workStatHax, &WorkStatHax::signalSet_statSubmit_text, this, &App::slotSet_statSubmit_text);
    connect(workHotkey, &WorkHotkey::signalHack, this, &App::slotHack);
    connect(workHotkey, &WorkHotkey::signalDuu, this, &App::slotDuu);
    connect(this, &App::signalWorkSuspend, workSuspend, &WorkSuspendProcess::slotWork);
    connect(this, &App::signalWorkLuckyWheel, workLuckyWheel, &WorkLuckyWheel::slotWork);
    connect(this, &App::signalWorkStatHax, workStatHax, &WorkStatHax::slotWork);
    connect(this, &App::signalWorkCheckBoxWatch, workCheckBoxWatch, &WorkCheckBoxWatch::slotWork);
    connect(this, &App::signalWorkHotkey, workHotkey, &WorkHotkey::slotWork);
    threadSuspend->start();
    threadStatHax->start();
    threadCheckBoxWatch->start();
    threadHotkey->start();
    emit signalWorkCheckBoxWatch();
    emit signalWorkHotkey();
}

App::~App()
{
    threadSuspend->terminate();
    threadLuckyWheel->terminate();
    threadStatHax->terminate();
    threadCheckBoxWatch->terminate();
    threadHotkey->terminate();
    delete threadSuspend;
    delete threadLuckyWheel;
    delete threadStatHax;
    delete threadCheckBoxWatch;
    delete threadHotkey;
    delete workSuspend;
    delete workLuckyWheel;
    delete workStatHax;
    delete workCheckBoxWatch;
    delete workHotkey;
    delete ui;
    delete gtav;
}

/**
 * @brief 按钮点击槽函数
 *
 * @param obj 按钮
 */
void App::slotPushButton_clicked(QWidget *object)
{
    if (object->objectName() == "pushButton_delSelect")
    {
        model->removeRow(ui->tableView_teleport->currentIndex().row());
    }
    else if (object->objectName() == "pushButton_saveTable")
    {
        QFile file("teleports.txt");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            return;
        }
        QTextStream stream(&file);
        for (int x = 0; x < model->rowCount(); x++)
        {
            stream << model->item(x, 0)->text();
            stream << ",";
            stream << model->item(x, 1)->text();
            stream << ",";
            stream << model->item(x, 2)->text();
            stream << ",";
            stream << model->item(x, 3)->text();
            stream << "\n";
        }
        file.close();
    }
    else if (object->objectName() == "pushButton_addLocalPosition")
    {
        FLOAT position[3] = {0};
        gtav->get_position(position);
        QList<QStandardItem *> rows = {
            new QStandardItem(ui->lineEdit_nameInput->text()),
            new QStandardItem(QString::asprintf("%.3f", position[0])),
            new QStandardItem(QString::asprintf("%.3f", position[1])),
            new QStandardItem(QString::asprintf("%.3f", position[2]))};
        model->appendRow(rows);
        ui->lineEdit_nameInput->setText("");
    }
    else if (object->objectName() == "pushButton_teleportToSelectedPosition")
    {
        threadCheatThread = QThread::create(
            [=]
            {
                int row = ui->tableView_teleport->currentIndex().row();
                FLOAT position[3] = {
                    model->item(row, 1)->text().toFloat(),
                    model->item(row, 2)->text().toFloat(),
                    model->item(row, 3)->text().toFloat(),
                };
                gtav->teleport((FLOAT *)position);
            });
        threadCheatThread->start();
    }
    else if (object->objectName() == "pushButton_statSubmit")
    {
        emit signalWorkStatHax();
    }
    else if (object->objectName() == "pushButton_teleportToWaypoint")
    {
        threadCheatThread = QThread::create(
            [=]
            {
                gtav->teleport_to_waypoint();
            });
        threadCheatThread->start();
    }
    else if (object->objectName() == "pushButton_fillCurrentAmmo")
    {
        threadCheatThread = QThread::create(
            [=]
            {
                gtav->fill_current_ammo();
            });
        threadCheatThread->start();
    }
    else if (object->objectName() == "pushButton_moveForward")
    {
        threadCheatThread = QThread::create(
            [=]
            {
                gtav->move_forward();
            });
        threadCheatThread->start();
    }
    else if (object->objectName() == "pushButton_noCops")
    {
        threadCheatThread = QThread::create(
            [=]
            {
                gtav->set_copLevel(0);
            });
        threadCheatThread->start();
    }
    else if (object->objectName() == "pushButton_killYourself")
    {
        threadCheatThread = QThread::create(
            [=]
            {
                gtav->set_health(-1.F);
                gtav->set_armor(-1.F);
            });
        threadCheatThread->start();
    }
    else if (object->objectName() == "pushButton_teleportToObjective")
    {
        threadCheatThread = QThread::create(
            [=]
            {
                gtav->teleport_to_objective();
            });
        threadCheatThread->start();
    }
    else if (object->objectName() == "pushButton_vehicleRepairing")
    {
        threadCheatThread = QThread::create(
            [=]
            {
                gtav->repairing_vehicle();
            });
        threadCheatThread->start();
    }
    else if (object->objectName() == "pushButton_solo")
    {
        emit signalWorkSuspend();
    }
    else if (object->objectName() == "pushButton_teleportEnemyNPC")
    {
        threadCheatThread = QThread::create(
            [=]
            {
                int row = ui->tableView_teleport->currentIndex().row();
                FLOAT position[3] = {
                    model->item(row, 1)->text().toFloat(),
                    model->item(row, 2)->text().toFloat(),
                    model->item(row, 3)->text().toFloat(),
                };
                gtav->teleport_enemy_npc((FLOAT *)position);
            });
        threadCheatThread->start();
    }
    else if (object->objectName() == "pushButton_readCut")
    {
        threadCheatThread = QThread::create(
            [=]
            {
                gtav->read_cut(cutData);
                emit signalUpdate_cutTab();
            });
        threadCheatThread->start();
    }
    else if (object->objectName() == "pushButton_writeCut")
    {
        threadCheatThread = QThread::create(
            [=]
            {
                INT data[] = {
                    ui->spinBox_playerCut1_apartmentHeist->value(),
                    ui->spinBox_playerCut2_apartmentHeist->value(),
                    ui->spinBox_playerCut3_apartmentHeist->value(),
                    ui->spinBox_playerCut4_apartmentHeist->value(),
                    ui->spinBox_playerCut1_doomsdayHeist->value(),
                    ui->spinBox_playerCut2_doomsdayHeist->value(),
                    ui->spinBox_playerCut3_doomsdayHeist->value(),
                    ui->spinBox_playerCut4_doomsdayHeist->value(),
                    ui->spinBox_playerCut1_casinoHeist->value(),
                    ui->spinBox_playerCut2_casinoHeist->value(),
                    ui->spinBox_playerCut3_casinoHeist->value(),
                    ui->spinBox_playerCut4_casinoHeist->value(),
                    ui->spinBox_playerCut1_pericoHeist->value(),
                    ui->spinBox_playerCut2_pericoHeist->value(),
                    ui->spinBox_playerCut3_pericoHeist->value(),
                    ui->spinBox_playerCut4_pericoHeist->value()};
                gtav->write_cut(data);
            });
        threadCheatThread->start();
    }
    else if (object->objectName() == "pushButton_skipApartmentHeist")
    {
        threadCheatThread = QThread::create(
            [=]
            {
                // 跳过准备前置 4人开始进入动画后触发生效
                gtav->stat_write((CHAR *)"MPx_HEIST_PLANNING_STAGE", -1);
            });
        threadCheatThread->start();
    }
    else if (object->objectName() == "pushButton_circuitBreaker")
    {
        threadCheatThread = QThread::create(
            [=]
            {
                // 全福银行钻开保险柜
                gtav->localAddress((CHAR *)"fm_mission_controller", 11731 + 24, 7);
            });
        threadCheatThread->start();
    }
    else if (object->objectName() == "pushButton_skipDoomsdayHeist")
    {
        threadCheatThread = QThread::create(
            [=]
            {
                // 跳过前置，M-设施管理-关闭后开启抢劫策划大屏
                gtav->stat_write((CHAR *)"MPx_GANGOPS_FLOW_MISSION_PROG", -1);
            });
        threadCheatThread->start();
    }
    else if (object->objectName() == "pushButton_doomsday1Hack")
    {
        threadCheatThread = QThread::create(
            [=]
            {
                // 末日1服务器群组
                gtav->localAddress((CHAR *)"fm_mission_controller", 1539, 2);
                gtav->localAddress((CHAR *)"fm_mission_controller", 1508, 3);
            });
        threadCheatThread->start();
    }
    else if (object->objectName() == "pushButton_doomsday3Hack")
    {
        threadCheatThread = QThread::create(
            [=]
            {
                // 末日3服务器
                gtav->localAddress((CHAR *)"fm_mission_controller", 1398, 3);
            });
        threadCheatThread->start();
    }
    else if (object->objectName() == "pushButton_casinoTargetChange")
    {
        threadCheatThread = QThread::create(
            [=]
            {
                // 钻石
                gtav->stat_write((CHAR *)"MPx_H3OPT_TARGET", 3);
            });
        threadCheatThread->start();
    }
    else if (object->objectName() == "pushButton_casinoFingerprintHack")
    {
        threadCheatThread = QThread::create(
            [=]
            {
                // 赌场指纹破解
                gtav->localAddress((CHAR *)"fm_mission_controller", 52929, 5);
            });
        threadCheatThread->start();
    }
    else if (object->objectName() == "pushButton_casinoDoorHack")
    {
        threadCheatThread = QThread::create(
            [=]
            {
                // 门禁点阵
                gtav->localAddress((CHAR *)"fm_mission_controller", 54726, 1);
                QThread::msleep(100);
                gtav->localAddress((CHAR *)"fm_mission_controller", 54747, 5);
                QThread::msleep(100);
                gtav->localAddress((CHAR *)"fm_mission_controller", 54726 + 17, 10);
            });
        threadCheatThread->start();
    }
    else if (object->objectName() == "pushButton_pericoTargetChange")
    {
        threadCheatThread = QThread::create(
            [=]
            {
                // 钻石猎豹
                gtav->stat_write((CHAR *)"MPx_H4CNF_TARGET", 5);
            });
        threadCheatThread->start();
    }
    else if (object->objectName() == "pushButton_pericoHack1") // 机场塔楼电压柜
    {
        threadCheatThread = QThread::create(
            [=]
            {
                gtav->localAddress(
                    (CHAR *)"fm_mission_controller_2020",
                    1715,
                    gtav->read_localAddressInt(
                        (CHAR *)"fm_mission_controller_2020",
                        1716));
            });
        threadCheatThread->start();
    }
    else if (object->objectName() == "pushButton_pericoHack2") // 下水道栅栏
    {
        threadCheatThread = QThread::create(
            [=]
            {
                gtav->localAddress((CHAR *)"fm_mission_controller_2020", 27500, 6);
            });
        threadCheatThread->start();
    }
    else if (object->objectName() == "pushButton_pericoHack3") // 指纹破解
    {
        threadCheatThread = QThread::create(
            [=]
            {
                gtav->localAddress((CHAR *)"fm_mission_controller_2020", 23385, 5);
            });
        threadCheatThread->start();
    }
    else if (object->objectName() == "pushButton_pericoHack4") // 主目标玻璃切割
    {
        threadCheatThread = QThread::create(
            [=]
            {
                gtav->localAddress((CHAR *)"fm_mission_controller_2020", 28736 + 3, 100.F);
            });
        threadCheatThread->start();
    }
    else if (object->objectName() == "pushButton_killEnemyNPC")
    {
        threadCheatThread = QThread::create(
            [=]
            {
                gtav->kill_enemy_npc();
            });
        threadCheatThread->start();
    }
    else if (object->objectName() == "pushButton_destroyEnemyVehicles")
    {
        threadCheatThread = QThread::create(
            [=]
            {
                gtav->destroy_enemy_vehicles();
            });
        threadCheatThread->start();
    }
    else if (object->objectName() == "pushButton_yimInject")
    {
        QString dllPath = QDir::currentPath() + "/" + "YimMenu.dll";
        threadCheatThread = QThread::create(
            [=]
            {
                gtav->dll_inject(dllPath.toLatin1().data());
            });
        threadCheatThread->start();
    }
    else if (object->objectName() == "pushButton_indoorSpawn")
    {
        if (!ui->lineEdit_vehicleHash->text().isEmpty())
        {
            threadCheatThread = QThread::create(
                [=]
                {
                    gtav->spawn_vehicle(ui->lineEdit_vehicleHash->text().toUInt(nullptr, 16), 2.5F);
                });
            threadCheatThread->start();
        }
    }
    else if (object->objectName() == "pushButton_outdoorSpawn")
    {
        if (!ui->lineEdit_vehicleHash->text().isEmpty())
        {
            threadCheatThread = QThread::create(
                [=]
                {
                    gtav->spawn_vehicle(ui->lineEdit_vehicleHash->text().toUInt(nullptr, 16));
                });
            threadCheatThread->start();
        }
    }
    emit signalDuu();
}

/**
 * @brief 复选框槽函数
 *
 * @param object 复选框
 */
void App::slotCheckBox_stateChanged(QCheckBox *object)
{
    if (object->objectName() == "checkBox_antiAFK")
    {
        threadCheatThread = QThread::create(
            [=]
            {
                gtav->antiAFK(ui->checkBox_antiAFK->isChecked());
            });
        threadCheatThread->start();
    }
    else if (object->objectName() == "checkBox_godMode")
    {
        threadCheatThread = QThread::create(
            [=]
            {
                gtav->set_godMode(ui->checkBox_godMode->isChecked());
            });
        threadCheatThread->start();
    }
    else if (object->objectName() == "checkBox_seatbelt")
    {
        threadCheatThread = QThread::create(
            [=]
            {
                gtav->set_seatbelt(ui->checkBox_seatbelt->isChecked());
            });
        threadCheatThread->start();
    }
    else if (object->objectName() == "checkBox_autoCure")
    {
        if (ui->checkBox_autoCure->isChecked())
        {
            threadCheatThread = QThread::create(
                [=]
                {
                    gtav->set_health(gtav->get_maxHealth());
                    gtav->set_armor(gtav->get_maxArmor());
                });
            threadCheatThread->start();
        }
    }
    else if (object->objectName() == "checkBox_infiniteAmmo")
    {
        threadCheatThread = QThread::create(
            [=]
            {
                gtav->set_infiniteAmmo(ui->checkBox_infiniteAmmo->isChecked());
            });
        threadCheatThread->start();
    }
    else if (object->objectName() == "checkBox_vehicleGodMode")
    {
        threadCheatThread = QThread::create(
            [=]
            {
                gtav->set_vehicleGodMode(ui->checkBox_vehicleGodMode->isChecked());
            });
        threadCheatThread->start();
    }
    else if (object->objectName() == "checkBox_luckyWheel")
    {
        if (ui->checkBox_luckyWheel->isChecked())
        {
            if (!threadLuckyWheel->isRunning())
            {
                threadLuckyWheel->start();
                emit signalWorkLuckyWheel();
            }
        }
        else
        {
            if (threadLuckyWheel->isRunning())
            {
                threadLuckyWheel->terminate();
            }
        }
    }
    if (object->isChecked())
    {
        emit signalDuu();
    }
    else
    {
        emit signalDuu(1);
    }
}

/**
 * @brief 组合框初始化
 *
 */
void App::comboxBox_init()
{
    ui->comboBox_session->clear();
    ui->comboBox_weather->clear();
    ui->comboBox_vehicles->clear();
    ui->comboBox_vehicleAbilities->clear();
    ui->comboBox_weaponAmmoType->clear();
    QList<StrVar>::const_iterator i;
    for (i = StaticData::session.constBegin();
         i != StaticData::session.constEnd(); i++)
    {
        ui->comboBox_session->addItem(i->name);
    }
    for (i = StaticData::weather.constBegin();
         i != StaticData::weather.constEnd(); i++)
    {
        ui->comboBox_weather->addItem(i->name);
    }
    for (i = StaticData::vehicles.constBegin();
         i != StaticData::vehicles.constEnd(); i++)
    {
        ui->comboBox_vehicles->addItem(i->name);
    }
    for (i = StaticData::vehicleAbilities.constBegin();
         i != StaticData::vehicleAbilities.constEnd(); i++)
    {
        ui->comboBox_vehicleAbilities->addItem(i->name);
    }
    for (i = StaticData::weaponAmmoType.constBegin();
         i != StaticData::weaponAmmoType.constEnd(); i++)
    {
        ui->comboBox_weaponAmmoType->addItem(i->name);
    }
}

/**
 * @brief 载具代码文本变化槽函数
 *
 */
void App::slotVehicleCode_textChanged()
{
    if (ui->lineEdit_vehicleCode->text().isEmpty() || ui->lineEdit_vehicleCode->text().isNull())
    {
        ui->lineEdit_vehicleHash->clear();
    }
    else
    {
        QByteArray vehicleCode = ui->lineEdit_vehicleCode->text().trimmed().toLatin1();
        UINT hash = Gtav::joaat(vehicleCode.data());
        QString hashText = "0x" + QString::asprintf("%08X", hash);
        ui->lineEdit_vehicleHash->setText(hashText);
    }
}

/**
 * @brief 传送表格初始化
 *
 */
void App::teleportTable_init()
{
    ui->tableView_teleport->setAlternatingRowColors(true);
    ui->tableView_teleport->setSortingEnabled(false);
    ui->tableView_teleport->horizontalHeader()->setCascadingSectionResizes(true);
    model->setHorizontalHeaderLabels(QStringList{"名称", "x", "y", "z"});
    ui->tableView_teleport->setModel(model);
    QFile file("teleports.txt");
    QTextStream stream(&file);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }
    while (!stream.atEnd())
    {
        QString line = stream.readLine();
        if (line.trimmed().isEmpty())
        {
            continue;
        }
        QStringList items = line.split(",", Qt::KeepEmptyParts);
        QList<QStandardItem *> rows = {
            new QStandardItem(items.value(0)),
            new QStandardItem(items.value(1)),
            new QStandardItem(items.value(2)),
            new QStandardItem(items.value(3))};
        model->appendRow(rows);
    }
    if (model->rowCount() > 0)
    {
        ui->tableView_teleport->selectRow(0);
    }
    file.close();
}

/**
 * @brief 组合框槽函数
 *
 * @param object 组合框
 */
void App::slotComboBox_changed(QWidget *object)
{
    if (object->objectName() == "comboBox_weather")
    {
        emit signalDuu();
        threadCheatThread = QThread::create(
            [=]
            {
                gtav->set_weather(StaticData::weather[ui->comboBox_weather->currentIndex()].value.toInt());
            });
        threadCheatThread->start();
    }
    else if (object->objectName() == "comboBox_weaponAmmoType")
    {
        emit signalDuu();
        threadCheatThread = QThread::create(
            [=]
            {
                gtav->set_weaponAmmoType(StaticData::weaponAmmoType[ui->comboBox_weaponAmmoType->currentIndex()].value.toInt());
            });
        threadCheatThread->start();
    }
}

/**
 * @brief 播放声音槽函数
 *
 */
void App::slotDuu(INT value)
{
    switch (value)
    {
    case 0:
        soundEffect->setSource(QUrl::fromLocalFile(":/sound/sound0.wav"));
        break;
    case 1:
        soundEffect->setSource(QUrl::fromLocalFile(":/sound/sound1.wav"));
        break;
    default:
        break;
    }
    soundEffect->play();
}

/**
 * @brief pushButton_statSubmit可点击槽函数
 *
 */
void App::slotSet_statSubmit_enable()
{
    ui->pushButton_statSubmit->setFlat(false);
    ui->pushButton_statSubmit->setDisabled(false);
}

/**
 * @brief pushButton_statSubmit不可点击槽函数
 *
 */
void App::slotSet_statSubmit_disable()
{
    ui->pushButton_statSubmit->setFlat(true);
    ui->pushButton_statSubmit->setDisabled(true);
}

/**
 * @brief pushButton_statSubmit设置文本槽函数
 *
 */
void App::slotSet_statSubmit_text(QString content)
{
    ui->pushButton_statSubmit->setText(content);
}

/**
 * @brief 更新分红tab页面数据配槽函数
 *
 * @param data
 */
void App::slotUpdate_cutTab()
{
    ui->spinBox_playerCut1_apartmentHeist->setValue(cutData[0]);
    ui->spinBox_playerCut2_apartmentHeist->setValue(cutData[1]);
    ui->spinBox_playerCut3_apartmentHeist->setValue(cutData[2]);
    ui->spinBox_playerCut4_apartmentHeist->setValue(cutData[3]);
    ui->spinBox_playerCut1_doomsdayHeist->setValue(cutData[4]);
    ui->spinBox_playerCut2_doomsdayHeist->setValue(cutData[5]);
    ui->spinBox_playerCut3_doomsdayHeist->setValue(cutData[6]);
    ui->spinBox_playerCut4_doomsdayHeist->setValue(cutData[7]);
    ui->spinBox_playerCut1_casinoHeist->setValue(cutData[8]);
    ui->spinBox_playerCut2_casinoHeist->setValue(cutData[9]);
    ui->spinBox_playerCut3_casinoHeist->setValue(cutData[10]);
    ui->spinBox_playerCut4_casinoHeist->setValue(cutData[11]);
    ui->spinBox_playerCut1_pericoHeist->setValue(cutData[12]);
    ui->spinBox_playerCut2_pericoHeist->setValue(cutData[13]);
    ui->spinBox_playerCut3_pericoHeist->setValue(cutData[14]);
    ui->spinBox_playerCut4_pericoHeist->setValue(cutData[15]);
}

/**
 * @brief TabWidget切换槽函数
 *
 */
void App::slotTabWidgetChanged()
{
    if (ui->tabWidget->currentIndex() == 2)
    {
        threadCheatThread = QThread::create(
            [=]
            {
                gtav->read_cut(cutData);
                emit signalUpdate_cutTab();
            });
        threadCheatThread->start();
    }
}

/**
 * @brief 破解槽函数
 *
 */
void App::slotHack()
{
    threadCheatThread = QThread::create(
        [=]
        {
            emit signalDuu();
            gtav->localAddress((CHAR *)"fm_mission_controller", 11731 + 24, 7);
            gtav->localAddress((CHAR *)"fm_mission_controller", 1537, 2);
            gtav->localAddress((CHAR *)"fm_mission_controller", 1398, 3);
            gtav->localAddress((CHAR *)"fm_mission_controller", 52929, 5);
            gtav->localAddress((CHAR *)"fm_mission_controller_2020",
                               1715,
                               gtav->read_localAddressInt(
                                   (CHAR *)"fm_mission_controller_2020",
                                   1716));
            gtav->localAddress((CHAR *)"fm_mission_controller_2020", 27500, 6);
            gtav->localAddress((CHAR *)"fm_mission_controller_2020", 23385, 5);
            gtav->localAddress((CHAR *)"fm_mission_controller_2020", 28736 + 3, 100.F);
            gtav->localAddress((CHAR *)"fm_mission_controller", 54726, 1);
            QThread::msleep(100);
            gtav->localAddress((CHAR *)"fm_mission_controller", 54747, 5);
            QThread::msleep(100);
            gtav->localAddress((CHAR *)"fm_mission_controller", 54726 + 17, 10);
        });
    threadCheatThread->start();
}