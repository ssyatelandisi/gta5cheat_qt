#ifndef GTAV_H
#define GTAV_H

#include "memorycontrol.h"
#include <windows.h>

struct MASK
{
    WCHAR name[32];
    CHAR mask[128];
};

class Gtav
{
private:
    ULONGLONG BlipPTR = 0;
    ULONGLONG GlobalPTR = 0;
    ULONGLONG LocalScriptsPTR = 0;
    ULONGLONG PickupDataPTR = 0;
    ULONGLONG PlayerCountPTR = 0;
    ULONGLONG ReplayInterfacePTR = 0;
    ULONGLONG WorldPTR = 0;
    ULONGLONG WeatherPTR = 0;
    CHAR processName[9] = "gta5.exe";
    CHAR Version[5] = "1.67";
    MASK masks[8] = {
        {L"WorldPTR", "48 8B 05 ? ? ? ? 45 ? ? ? ? 48 8B 48 08 48 85 C9 74 07"},
        {L"BlipPTR", "4C 8D 05 ? ? ? ? 0F B7 C1"},
        {L"ReplayInterfacePTR", "48 8D 0D ? ? ? ? 48 8B D7 E8 ? ? ? ? 48 8D 0D ? ? ? ? 8A D8 E8"},
        {L"LocalScriptsPTR", "48 8B 05 ? ? ? ? 8B CF 48 8B 0C C8 39 59 68"},
        {L"GlobalPTR", "4C 8D 05 ? ? ? ? 4D 8B 08 4D 85 C9 74 11"},
        {L"PlayerCountPTR", "48 8B 0D ? ? ? ? E8 ? ? ? ? 48 8B C8 E8 ? ? ? ? 48 8B CF"},
        {L"PickupDataPTR", "48 8B 05 ? ? ? ? 48 8B 1C F8 8B"},
        {L"WeatherPTR", "48 83 EC ? 8B 05 ? ? ? ? 8B 3D ? ? ? ? 49"}};

public:
    Gtav();
    ~Gtav();
    static UINT joaat(char *input);
    ULONGLONG globalAddress(ULONGLONG offset);
    ULONGLONG localAddress(CHAR *threadName, INT offset);
    void localAddress(CHAR *threadName, ADDR offset, INT value);
    void localAddress(CHAR *threadName, ADDR offset, UINT value);
    void localAddress(CHAR *threadName, ADDR offset, FLOAT value);
    INT read_localAddressInt(CHAR *threadName, ADDR offset);
    UINT read_localAddressUint(CHAR *threadName, ADDR offset);
    FLOAT read_localAddressFloat(CHAR *threadName, ADDR offset);
    ULONGLONG memoryMaskCode_search(BYTE *memory, SIZE_T length, SHORT *maskCode, SIZE_T size);
    bool check_version(BYTE *memory, SIZE_T length, char *version);
    void pattern_init(BYTE *memory, SIZE_T length, MASK *maskCode, SIZE_T size);
    bool check_valid(ADDR address);
    bool check_inVehicle();
    void get_position(FLOAT *position);
    void teleport(FLOAT *position);
    void teleport_to_waypoint();
    void teleport_to_objective();
    void set_weather(INT weather);
    void fill_current_ammo();
    void move_forward();
    void set_copLevel(INT copLevel);
    void set_health(FLOAT health);
    FLOAT get_maxHealth();
    void set_armor(FLOAT armor);
    FLOAT get_maxArmor();
    void set_godMode(bool value);
    void set_seatbelt(bool value);
    void set_infiniteAmmo(bool value);
    void set_vehicleGodMode(bool value);
    void repairing_vehicle();
    void change_session(INT value);
    void set_vehicleAbility(INT value);
    void set_weaponAmmoType(INT value);
    INT read_gaInt(ADDR offset);
    UINT read_gaUint(ADDR offset);
    SHORT read_gaShort(ADDR offset);
    USHORT read_gaUshort(ADDR offset);
    FLOAT read_gaFloat(ADDR offset);
    void write_gaInt(ADDR offset, INT value);
    void write_gaUint(ADDR offset, UINT value);
    void write_gaShort(ADDR offset, SHORT value);
    void write_gaUshort(ADDR offset, USHORT value);
    void write_gaFloat(ADDR offset, FLOAT value);
    void write_gaUchar(ADDR offset, UCHAR value);
    void spawn_vehicle(UINT hash, FLOAT d = 5.F);
    void suspend_process();
    void teleport_enemy_npc(FLOAT *position);
    void kill_enemy_npc();
    void destroy_enemy_vehicles();
    void set_luckyWheel(INT value = 18); // 18: 展台载具
    void kill_process();
    void stat_write(CHAR *stat, INT value);
    void dll_inject(CHAR *dllPath);
    void read_cut(INT *data);
    void write_cut(INT *data);
    void antiAFK(bool value);
};

#endif // GTAV_H
