#ifndef WORK_H
#define WORK_H

#include "gtav.h"
#include "ui_app.h"
#include <windows.h>
#include <QStandardItemModel>
#include <QString>
#include <QObject>

// 挂起进程子线程
class WorkSuspendProcess : public QObject
{
    Q_OBJECT

public:
    explicit WorkSuspendProcess(QObject *parent, Gtav *g);
    ~WorkSuspendProcess();

public slots:
    void slotWork();

private:
    Gtav *gtav;
};

// 幸运转盘子线程
class WorkLuckyWheel : public QObject
{
    Q_OBJECT

public:
    explicit WorkLuckyWheel(QObject *parent, Gtav *g);
    ~WorkLuckyWheel();

public slots:
    void slotWork();

private:
    Gtav *gtav;
};

// StatHax子线程
class WorkStatHax : public QObject
{
    Q_OBJECT

public:
    explicit WorkStatHax(QObject *parent, Ui_App *u, Gtav *g);
    ~WorkStatHax();

public slots:
    void slotWork();

signals:
    void signalSet_statSubmit_text(QString content);
    void signalStarted();
    void signalFinished();

private:
    Ui_App *ui;
    Gtav *gtav;
    QString defaultContent;
};

// 复选框监听子线程
class WorkCheckBoxWatch : public QObject
{
    Q_OBJECT

public:
    explicit WorkCheckBoxWatch(QObject *parent, Ui_App *ui, QStandardItemModel *model, Gtav *g);
    ~WorkCheckBoxWatch();

public slots:
    void slotWork();

private:
    Gtav *gtav;
    Ui_App *ui;
    QStandardItemModel *model;
};

// 热键监听子线程
class WorkHotkey : public QObject
{
    Q_OBJECT

public:
    explicit WorkHotkey(QObject *parent, Ui_App *u, Gtav *g);
    ~WorkHotkey();
    void keyup(WORD scanKey);
    void keydown(WORD scanKey);

public slots:
    void slotWork();

signals:
    void signalDuu(INT value = 0);
    void signalHack();

private:
    Gtav *gtav;
    Ui_App *ui;
};

#endif // WORK_H
