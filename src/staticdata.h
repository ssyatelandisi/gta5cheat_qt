#ifndef STATICDATA_H
#define STATICDATA_H
#include <QList>
#include <QString>
#include <QVariant>
#include <windows.h>

struct StrVar
{
    QString name;
    QVariant value;
};

class StaticData
{
public:
    StaticData();
    ~StaticData();
    static QList<StrVar> session;
    static QList<StrVar> weather;
    static QList<StrVar> weaponAmmoType;
    static QList<StrVar> vehicles;
    static QList<StrVar> vehicleAbilities;
};

#endif // STATICDATA_H
