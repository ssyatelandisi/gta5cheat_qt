#include "gtav.h"
#include "memorycontrol.h"
#include "offset.h"
#include <stdio.h>
#include <tchar.h>
using namespace std;
using namespace memorycontrol;

Gtav::Gtav()
{
    pid = get_pid(processName);
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid); // 进程句柄
    hModule = (ADDR)get_hModule(pid, processName, &moduleSize);
    if (moduleSize != 0)
    {
        BYTE *memory = new BYTE[moduleSize];
        SIZE_T NumberOfBytesRead = 0;
        ReadProcessMemory(hProcess, (LPCVOID)hModule, memory, moduleSize, &NumberOfBytesRead);
        if (NumberOfBytesRead != 0)
        {
            check_version(memory, moduleSize, Version);
            pattern_init(memory, moduleSize, masks, len(masks));
        }
        delete[] memory;
    }
}

Gtav::~Gtav()
{
    pid = 0;
    hProcess = 0;
    hModule = 0;
    moduleSize = 0;
}

/**
 * @brief joaat运算
 *
 * @param input 输入字符
 * @return UINT
 */
UINT Gtav::joaat(char *input)
{
    UINT num1 = 0U;
    int i = 0;
    while (input[i] != '\0')
    {
        UINT num2 = num1 + (UINT)tolower(input[i]);
        UINT num3 = num2 + (num2 << 10);
        num1 = num3 ^ num3 >> 6;
        i++;
    }
    UINT num4 = num1 + (num1 << 3);
    UINT num5 = num4 ^ num4 >> 11;
    return num5 + (num5 << 15);
}

/**
 * @brief local线程算法
 *
 * @param threadName 线程名
 * @param offset 偏移
 * @return ULONGLONG
 */
ULONGLONG Gtav::localAddress(CHAR *threadName, INT offset)
{
    ULONGLONG p = read_ulonglong(LocalScriptsPTR);
    for (SIZE_T i = 0; i < 54; i++)
    {
        ULONGLONG pointer = read_ulonglong(p + i * 0x8);
        char buffer[MAX_PATH] = {0};
        read_bytes(pointer + 0xD4, buffer, MAX_PATH);
        if (stricmp(threadName, buffer) == 0)
        {
            return read_ulonglong(pointer + 0xB0) + offset * 8;
        }
    }
    return 0;
}

/**
 * @brief global偏移地址运算
 *
 * @param offset 偏移
 * @return ULONGLONG
 */
ULONGLONG Gtav::globalAddress(ULONGLONG offset)
{
    if (GlobalPTR == 0)
    {
        return 0;
    }
    else
    {
        return read_ulonglong(GlobalPTR + 8 * ((offset >> 0x12) & 0x3F)) + 8 * (offset & 0x3FFFF);
    }
}

INT Gtav::read_gaInt(ADDR offset)
{
    return read_int(globalAddress(offset));
}

UINT Gtav::read_gaUint(ADDR offset)
{
    return read_uint(globalAddress(offset));
}

SHORT Gtav::read_gaShort(ADDR offset)
{
    return read_short(globalAddress(offset));
}

USHORT Gtav::read_gaUshort(ADDR offset)
{
    return read_ushort(globalAddress(offset));
}

FLOAT Gtav::read_gaFloat(ADDR offset)
{
    return read_float(globalAddress(offset));
}

void Gtav::write_gaInt(ADDR offset, INT value)
{
    write_int(globalAddress(offset), value);
}

void Gtav::write_gaUint(ADDR offset, UINT value)
{
    write_uint(globalAddress(offset), value);
}

void Gtav::write_gaShort(ADDR offset, SHORT value)
{
    write_short(globalAddress(offset), value);
}

void Gtav::write_gaUshort(ADDR offset, USHORT value)
{
    write_ushort(globalAddress(offset), value);
}

void Gtav::write_gaFloat(ADDR offset, FLOAT value)
{
    write_float(globalAddress(offset), value);
}

void Gtav::write_gaUchar(ADDR offset, UCHAR value)
{
    write_uchar(globalAddress(offset), value);
}

/**
 * @brief 特征码定位查询
 *
 * @param memory 内存字节
 * @param length 内存字节长度
 * @param maskCode 特征码字节
 * @param size 特征码字节长度
 * @return ULONGLONG
 */
ULONGLONG Gtav::memoryMaskCode_search(BYTE *memory, SIZE_T length, SHORT *maskCode, SIZE_T size)
{
    SIZE_T posStart = 0;
    SIZE_T posEnd = posStart + length - size;
    SIZE_T curIndex = 0;
    while (posStart <= posEnd)
    {
        if (memory[posStart] == maskCode[curIndex] || maskCode[curIndex] < 0)
        {
            curIndex += 1;
            if (curIndex >= size)
            {
                posStart = posStart - size + 1;
                return posStart;
            }
        }
        else if (memory[posStart] == maskCode[0] || maskCode[0] < 0)
        {
            curIndex = 1;
        }
        else
        {
            curIndex = 0;
        }
        posStart++;
    }
    return 0;
}

/**
 * @brief 版本核对
 *
 * @param memory 内存字节
 * @param moduleSize 内存字节长度
 * @param version 版本字符串
 * @return bool
 */
bool Gtav::check_version(BYTE *memory, SIZE_T moduleSize, char *version)
{
    SIZE_T size = strlen(version);
    SHORT *maskCode = new SHORT[size]{0};
    for (int i = 0; i < size; i++)
    {
        maskCode[i] = version[i];
    }
    BOOLEAN result;
    if (memoryMaskCode_search(memory, moduleSize, maskCode, size) == 0)
    {
        WCHAR msg[MAX_PATH] = L"当前游戏版本与辅助支持的";
        WCHAR *msg0 = new WCHAR[strlen(version) + 1]{0};
        mbstowcs(msg0, version, strlen(version) + 1);
        wcscat(msg, msg0);
        wcscat(msg, L"版本不匹配，谨慎使用。");
        MessageBoxW(NULL, msg, L"警告", MB_ICONWARNING | MB_OK);
        delete[] msg0;
        result = false;
    }
    else
    {
        result = true;
    }
    delete[] maskCode;
    return result;
}

/**
 * @brief 特征码初始化
 *
 * @param memory 内存字节
 * @param length 内存字节长度
 * @param masks 特征码字符串
 * @param size 特征码长度
 */
void Gtav::pattern_init(BYTE *memory, SIZE_T length, MASK *masks, SIZE_T size)
{
    for (int i = 0; i < size; i++)
    {
        vector<char *> BYTECodeArray = split(masks[i].mask, " ");
        vector<SHORT> BYTENumberArray = bytecodearray2bytenumberarray(BYTECodeArray);
        SHORT *maskCode = new SHORT[BYTENumberArray.size()];
        for (int j = 0; j < BYTENumberArray.size(); j++)
        {
            maskCode[j] = BYTENumberArray[j];
        }
        ULONGLONG pos = memoryMaskCode_search(memory, moduleSize, maskCode, BYTENumberArray.size());
        UINT *match;
        ULONGLONG ptr = 0;
        if (wcsicmp(masks[i].name, L"WorldPTR") == 0)
        {
            match = (UINT *)&memory[pos + 3];
            ptr = hModule + pos + *match + 3 + 4;
            WorldPTR = ptr;
        }
        else if (wcsicmp(masks[i].name, L"BlipPTR") == 0)
        {
            match = (UINT *)&memory[pos + 3];
            ptr = hModule + pos + *match + 3 + 4;
            BlipPTR = ptr;
        }
        else if (wcsicmp(masks[i].name, L"ReplayInterfacePTR") == 0)
        {
            match = (UINT *)&memory[pos + 3];
            ptr = hModule + pos + *match + 3 + 4;
            ReplayInterfacePTR = ptr;
        }
        else if (wcsicmp(masks[i].name, L"LocalScriptsPTR") == 0)
        {
            match = (UINT *)&memory[pos + 3];
            ptr = hModule + pos + *match + 3 + 4;
            LocalScriptsPTR = ptr;
        }
        else if (wcsicmp(masks[i].name, L"GlobalPTR") == 0)
        {
            match = (UINT *)&memory[pos + 3];
            ptr = hModule + pos + *match + 3 + 4;
            GlobalPTR = ptr;
        }
        else if (wcsicmp(masks[i].name, L"PlayerCountPTR") == 0)
        {
            match = (UINT *)&memory[pos + 3];
            ptr = hModule + pos + *match + 3 + 4;
            PlayerCountPTR = ptr;
        }
        else if (wcsicmp(masks[i].name, L"PickupDataPTR") == 0)
        {
            match = (UINT *)&memory[pos + 3];
            ptr = hModule + pos + *match + 3 + 4;
            PickupDataPTR = ptr;
        }
        else if (wcsicmp(masks[i].name, L"WeatherPTR") == 0)
        {
            match = (UINT *)&memory[pos + 6];
            ptr = hModule + pos + *match + 6 + 4;
            WeatherPTR = ptr;
        }
        wprintf(L"%-18S: %s + 0x%08llX\n", masks[i].name, processName, ptr - hModule);
        delete[] maskCode;
        // WorldPTR          : gta5.exe + 0x02561678
        // BlipPTR           : gta5.exe + 0x01FD5990
        // ReplayInterfacePTR: gta5.exe + 0x01F6FA20
        // LocalScriptsPTR   : gta5.exe + 0x02E770B0
        // GlobalPTR         : gta5.exe + 0x02E771E0
        // PlayerCountPTR    : gta5.exe + 0x01E195E0
        // PickupDataPTR     : gta5.exe + 0x02560BC0
        // WeatherPTR        : gta5.exe + 0x0254A1BC
    }
}

/**
 * @brief 检查地址有效性
 *
 * @param address 地址
 * @return bool
 */
bool Gtav::check_valid(ADDR address)
{
    if (0x00400000 <= address && address <= hModule + moduleSize)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * @brief 检查玩家是否在载具上
 *
 * @return bool
 */
bool Gtav::check_inVehicle()
{
    ADDR pedInVehicleArray[] = {WorldPTR, oPed, oPedInVehicle};
    return pread_int(pedInVehicleArray, len(pedInVehicleArray)) == 0 ? false : true;
}

/**
 * @brief 获取当前定位
 *
 * @param position
 */
void Gtav::get_position(FLOAT *position)
{
    ADDR navigationXArray[] = {WorldPTR, oPed, oPedNavigation, oPedNavigationPositionX};
    pread_bytes(navigationXArray, len(navigationXArray), position, 12);
}

/**
 * @brief 传送到坐标点
 *
 * @param x x坐标值
 * @param y y坐标值
 * @param z z坐标值
 */
void Gtav::teleport(FLOAT *position)
{
    if(position[0]>6399.F){
        return;
    }
    ADDR pedVisualXArray[] = {WorldPTR, oPed, oPedVisualX};
    ADDR pedNavigationPositionXArray[] = {WorldPTR, oPed, oPedNavigation, oPedNavigationPositionX};
    ADDR pedVehicleVisualXArray[] = {WorldPTR, oPed, oPedVehicle, oPedVehicleVisualX};
    ADDR pedVehicleNavigationPositionXArray[] = {WorldPTR, oPed, oPedVehicle, oPedVehicleNavigation, oPedVehicleNavigationPositionX};
    if (check_inVehicle())
    {
        pwrite_bytes(pedVehicleVisualXArray, len(pedVehicleVisualXArray), position, 12);
        pwrite_bytes(pedVehicleNavigationPositionXArray, len(pedVehicleNavigationPositionXArray), position, 12);
    }
    else
    {
        pwrite_bytes(pedVisualXArray, len(pedVisualXArray), position, 12);
        pwrite_bytes(pedNavigationPositionXArray, len(pedNavigationPositionXArray), position, 12);
    }
}

/**
 * @brief 传送到导航点
 *
 */
void Gtav::teleport_to_waypoint()
{
    for (int i = 2000; i > 0; i--)
    {
        ADDR addr = read_ulonglong(BlipPTR + i * 8);
        if (!check_valid(addr))
        {
            continue;
        }
        if (read_int(addr + 0x40) == 0x08 && read_int(addr + 0x48) == 0x54)
        {
            FLOAT position[3] = {0};
            read_bytes(addr + 0x10, position, 8);
            position[2] = (abs(read_float(addr + 0x18) - 20.F) < 0.01F) ? -200.F : read_float(addr + 0x18) + 1.F;
            teleport(position);
        }
    }
}

/**
 * @brief 传送到目标点
 *
 */
void Gtav::teleport_to_objective()
{
    FLOAT position[3] = {6400.F};
    for (int i = 1; i <= 2000; i++)
    {
        ADDR addr = read_ulonglong(BlipPTR + i * 8);
        if (!check_valid(addr))
        {
            continue;
        }
        ADDR buf0 = read_int(addr + 0x40);
        ADDR buf1 = read_int(addr + 0x48);
        if (buf0 == 1 && (buf1 == 5 || buf1 == 60 || buf1 == 66))
        {
            read_bytes(addr + 0x10, position, 12);
            position[2] += 1.F;
            teleport(position);
            return;
        }
        else if ((buf0 == 1 || buf0 == 225 || buf0 == 427 || buf0 == 478 || buf0 == 501 || buf0 == 523 || buf0 == 556) && (buf1 == 1 || buf1 == 2 || buf1 == 3 || buf1 == 54 || buf1 == 78))
        {
            read_bytes(addr + 0x10, position, 12);
            position[2] += 1.F;
            teleport(position);
            return;
        }
        else if ((buf0 == 432 || buf0 == 443) && (buf1 == 59))
        {
            read_bytes(addr + 0x10, position, 12);
            position[2] += 1.F;
            teleport(position);
            return;
        }
    }
}

/**
 * @brief 向前移动
 *
 */
void Gtav::move_forward()
{
    if (check_inVehicle())
    {
        return;
    }
    ADDR headingArray0[] = {WorldPTR, oPed, oPedNavigation, 0x20};
    ADDR headingArray1[] = {WorldPTR, oPed, oPedNavigation, 0x24};
    FLOAT heading0 = pread_float(headingArray0, len(headingArray0));
    FLOAT heading1 = pread_float(headingArray1, len(headingArray1));
    FLOAT position[3] = {0};
    get_position(position);
    position[0] -= heading1 * 2.5;
    position[1] += heading0 * 2.5;
    teleport(position);
}

/**
 * @brief 设置天气
 *
 * @param value
 */
void Gtav::set_weather(INT weather)
{
    if (weather == -1)
    {
        write_int(WeatherPTR + 0x24, -1);
    }
    else if (weather == 13)
    {
        write_int(WeatherPTR + 0x24, 13);
    }
    write_int(WeatherPTR + 0x104, weather);
}

/**
 * @brief 补满弹药
 *
 */
void Gtav::fill_current_ammo()
{
    ADDR pedWeaponAmmoArray[] = {WorldPTR, oPed, oPedWeaponManager, oPedWeapon, oPedWeaponAmmo};
    ADDR pedWeaponAmmoAddress = pread_ulonglong(pedWeaponAmmoArray, len(pedWeaponAmmoArray));
    if (!check_valid(pedWeaponAmmoAddress))
    {
        return;
    }
    INT maxAmmo = read_int(pedWeaponAmmoAddress + 0x28);
    ADDR offset0 = pedWeaponAmmoAddress;
    ADDR offset1 = 0;
    UCHAR ammoType = 0;
    while (ammoType == 0)
    {
        offset0 = read_ulonglong(offset0 + 0x8);
        offset1 = read_ulonglong(offset0 + 0x0);
        if (offset0 == 0 || offset1 == 0)
        {
            return;
        }
        ammoType = read_uchar(offset1 + 0xC);
    }
    write_ushort(offset1 + 0x18, maxAmmo);
}

/**
 * @brief 设置通缉等级
 *
 * @param value 通缉等级
 */
void Gtav::set_copLevel(INT copLevel)
{
    ADDR pedCopLevelArray[] = {WorldPTR, oPed, oPedPlayerInfo, oPedPlayerInfoCopLevel};
    ADDR pedCopLevelExtentArray[] = {WorldPTR, oPed, oPedPlayerInfo, 0x7F0}; // 通缉范围0-5700 焱芔提供
    pwrite_int(pedCopLevelArray, len(pedCopLevelArray), copLevel);
    if (copLevel == 0)
    {
        pwrite_int(pedCopLevelExtentArray, len(pedCopLevelExtentArray), 0);
    }
}

/**
 * @brief 设置血量值
 *
 */
void Gtav::set_health(FLOAT health)
{
    ADDR pedHealthArray[] = {WorldPTR, oPed, oPedHealth};
    pwrite_float(pedHealthArray, len(pedHealthArray), health);
}

/**
 * @brief 设置护甲数值
 *
 * @param armor
 */
void Gtav::set_armor(FLOAT armor)
{
    ADDR pedArmorArray[] = {WorldPTR, oPed, oPedArmor};
    pwrite_float(pedArmorArray, len(pedArmorArray), armor);
}

/**
 * @brief 设置无敌状态
 *
 * @param status
 */
void Gtav::set_godMode(bool status)
{
    ADDR pedGodModeArray[] = {WorldPTR, oPed, oPedGodMode};
    pwrite_uchar(pedGodModeArray, len(pedGodModeArray), status ? 1 : 0);
}

/**
 * @brief 设置安全带状态
 *
 * @param status
 */
void Gtav::set_seatbelt(bool status)
{
    ADDR pedSeatbeltArray[] = {WorldPTR, oPed, oPedSeatbelt};
    pwrite_uchar(pedSeatbeltArray, len(pedSeatbeltArray), status ? 0xC9 : 0xC8);
}

/**
 * @brief 获取最大血量值
 *
 * @return FLOAT
 */
FLOAT Gtav::get_maxHealth()
{
    ADDR pedMaxHealthArray[] = {WorldPTR, oPed, oPedMaxHealth};
    return pread_float(pedMaxHealthArray, len(pedMaxHealthArray));
}

/**
 * @brief 获取最大护甲值
 *
 * @return FLOAT
 */
FLOAT Gtav::get_maxArmor()
{
    ADDR pedMaxArmorArray[] = {WorldPTR, oPed, oPedMaxArmor};
    return pread_float(pedMaxArmorArray, len(pedMaxArmorArray));
}

/**
 * @brief 设置无限弹药状态
 *
 * @param status
 */
void Gtav::set_infiniteAmmo(bool status)
{
    ADDR pedInfiniteAmmoArray[] = {WorldPTR, oPed, oPedInventory, oPedInventoryInfiniteAmmo};
    pwrite_uchar(pedInfiniteAmmoArray, len(pedInfiniteAmmoArray), status ? 10 : 0);
}

/**
 * @brief 设置载具无敌状态
 *
 * @param status
 */
void Gtav::set_vehicleGodMode(bool status)
{
    ADDR pedVehicleGodModeArray[] = {WorldPTR, oPed, oPedVehicle, oPedVehicleGodMode};
    pwrite_uchar(pedVehicleGodModeArray, len(pedVehicleGodModeArray), status ? 1 : 0);
}

/**
 * @brief 修复载具
 *
 */
void Gtav::repairing_vehicle()
{
    if (read_gaInt(2672524 + 3690) != 0)
    {
        write_gaInt(2672524 + 3690, -1); // 消除牛鲨睾酮效果
        return;
    }
    if (!check_inVehicle()) // 不在载具上
    {
        return;
    }
    // ADDR bstTrigger[] = {GlobalPTR + 0x8 * 0xA, 0x17BE28};
    // pwrite_int(bstTrigger, len(bstTrigger), 1); // 空投牛鲨睾酮
    write_gaInt(2794162 + 899, 1);
    Sleep(1700);
    ADDR vehicleHealthArray[] = {WorldPTR, oPed, oPedVehicle, oPedVehicleHealth};
    pwrite_float(vehicleHealthArray, len(vehicleHealthArray), 999.F); // 由于载具满血无法接到牛鲨睾酮
    ADDR fixVehValueArray[] = {PickupDataPTR, 0x230};
    INT fixVehValue = pread_int(fixVehValueArray, len(fixVehValueArray));
    ADDR bstValueArray[] = {PickupDataPTR, 0x160};
    INT bstValue = pread_int(bstValueArray, len(bstValueArray));
    ADDR pickupInterfaceArray[] = {ReplayInterfacePTR, oReplayInterfacePickupInterface};
    ADDR pickupInterface = pread_ulonglong(pickupInterfaceArray, len(pickupInterfaceArray));
    INT pickupCount = read_int(pickupInterface + 0x110); // 拾取物数量
    ADDR pickupList = read_ulonglong(pickupInterface + 0x100);
    ADDR pedVehicleNavigationPositionXArray[] = {WorldPTR, oPed, oPedVehicle, oPedVehicleNavigation, oPedVehicleNavigationPositionX};
    for (int i = 0; i < pickupCount; i++)
    {
        ADDR pickup = read_ulonglong(pickupList + i * 0x10);
        INT pickupValue = read_int(pickup + 0x470);
        if (pickupValue == bstValue || pickupValue == fixVehValue)
        {
            write_uint(pickup + 0x470, fixVehValue);
            FLOAT position[3] = {0};
            pread_bytes(pedVehicleNavigationPositionXArray, len(pedVehicleNavigationPositionXArray), position, 12);
            write_bytes(pickup + 0x90, position, 12); // 修改拾取物坐标
            ADDR vehicleCleanlinessArray[] = {WorldPTR, oPed, oPedVehicle, 0x9D8};
            pwrite_float(vehicleCleanlinessArray, len(vehicleCleanlinessArray), 0.F); // 清洁载具外表
        }
    }
    Sleep(1000);
    if (read_gaInt(2672524 + 3689) != 0)
    {
        write_gaInt(2672524 + 3689, -1); // 消除牛鲨睾酮效果
    }
}

/**
 * @brief 切换战局
 *
 */
void Gtav::change_session(INT value)
{
    if (value == -1)
    {
        write_gaInt(oSessionCache, value); // valueAddress
        write_gaInt(oSessionState, 1);     // triggerAddress
    }
    else
    {
        write_gaInt(oSessionType, value);
        write_gaInt(oSessionState, 1);
    }
    Sleep(200);
    write_gaInt(oSessionState, 0);
}

/**
 * @brief 设置载具能力
 *
 * @param value
 */
void Gtav::set_vehicleAbility(INT value)
{
    ADDR vehicleAbilityArray[] = {WorldPTR, oPed, oPedVehicle, oPedVehicleModelInfo, oPedVehicleModelInfoExtras};
    ADDR vehicleAbilityRecoveryArray[] = {WorldPTR, oPed, oPedVehicle, oPedVehicleAbilityRecovery};
    pwrite_int(vehicleAbilityArray, len(vehicleAbilityArray), value);
    if (value == 0x40) // 火箭助推器
    {
        pwrite_float(vehicleAbilityRecoveryArray, len(vehicleAbilityRecoveryArray), 5.F);
    }
}

/**
 * @brief 设置武器弹药类型
 *
 * @param value
 */
void Gtav::set_weaponAmmoType(INT value)
{
    ADDR ammoTypeArray[] = {WorldPTR, oPed, oPedWeaponManager, oPedWeaponManagerWeaponInfo, oPedWeaponManagerWeaponInfoImpactExplosion};
    ADDR ammoImpactExplosion[] = {WorldPTR, oPed, oPedWeaponManager, oPedWeaponManagerWeaponInfo, oPedWeaponManagerWeaponInfoImpactType};
    pwrite_short(ammoImpactExplosion, len(ammoImpactExplosion), (value == -1) ? 3 : 5); // 影响类型
    pwrite_int(ammoTypeArray, len(ammoTypeArray), value);
}

/**
 * @brief 刷载具
 *
 * @param hash
 */
void Gtav::spawn_vehicle(UINT hash, FLOAT d)
{
    ADDR heading0Array[] = {WorldPTR, oPed, oPedNavigation, 0x20};
    ADDR heading1Array[] = {WorldPTR, oPed, oPedNavigation, 0x30};
    float heading0 = pread_float(heading0Array, len(heading0Array));
    float heading1 = pread_float(heading1Array, len(heading1Array));
    ADDR playerPositionAddary[] = {WorldPTR, oPed, oPedNavigation, oPedNavigationPositionX};
    float playerPosition[3];
    pread_bytes(playerPositionAddary, len(playerPositionAddary), playerPosition, 12);
    playerPosition[0] += heading1 * d;
    playerPosition[1] += heading0 * d;
    playerPosition[2] += 0.5;
    write_gaFloat(oVehicleCreate + 7 + 0, playerPosition[0]);
    write_gaFloat(oVehicleCreate + 7 + 1, playerPosition[1]);
    write_gaFloat(oVehicleCreate + 7 + 2, playerPosition[2]);
    write_gaUint(oVehicleCreate + 27 + 66, hash);
    write_gaInt(oVehicleCreate + 3, 0);
    write_gaInt(oVehicleCreate + 5, 1);
    write_gaInt(oVehicleCreate + 2, 1);
    write_gaInt(oVehicleCreate + 27 + 5, -1);
    write_gaInt(oVehicleCreate + 27 + 6, -1);
    write_gaInt(oVehicleCreate + 27 + 7, -1);
    write_gaInt(oVehicleCreate + 27 + 8, -1);
    write_gaInt(oVehicleCreate + 27 + 15, 1);
    write_gaInt(oVehicleCreate + 27 + 19, -1);
    write_gaInt(oVehicleCreate + 27 + 20, 2);
    write_gaInt(oVehicleCreate + 27 + 21, 3);
    write_gaInt(oVehicleCreate + 27 + 22, 6);
    write_gaInt(oVehicleCreate + 27 + 23, 9);
    write_gaInt(oVehicleCreate + 27 + 24, 0);
    write_gaInt(oVehicleCreate + 27 + 25, 14);
    write_gaInt(oVehicleCreate + 27 + 26, 19);
    write_gaInt(oVehicleCreate + 27 + 27, 1);
    write_gaInt(oVehicleCreate + 27 + 28, 1);
    write_gaInt(oVehicleCreate + 27 + 30, 1);
    write_gaInt(oVehicleCreate + 27 + 32, 0);
    write_gaInt(oVehicleCreate + 27 + 33, -1);
    write_gaInt(oVehicleCreate + 27 + 60, 1);
    write_gaInt(oVehicleCreate + 27 + 65, 0);
    write_gaInt(oVehicleCreate + 27 + 67, 1);
    write_gaInt(oVehicleCreate + 27 + 69, -1);
    write_gaUint(oVehicleCreate + 27 + 77, 4030726305);
    write_gaUchar(oVehicleCreate + 27 + 77 + 1, 2);
    write_gaInt(oVehicleCreate + 27 + 95, 14);
    write_gaInt(oVehicleCreate + 27 + 94, 2);
}

/**
 * @brief 挂起进程卡单
 *
 */
void Gtav::suspend_process()
{
    typedef void (*fun)(HANDLE);
    fun NtSuspendProcess = (fun)GetProcAddress(GetModuleHandleW(L"ntdll"), "NtSuspendProcess");
    fun NtResumeProcess = (fun)GetProcAddress(GetModuleHandleW(L"ntdll"), "NtResumeProcess");
    NtSuspendProcess(hProcess);
    Sleep(10000);
    NtResumeProcess(hProcess);
}

/**
 * @brief 传送敌人到坐标点
 *
 * @param position
 */
void Gtav::teleport_enemy_npc(FLOAT *position)
{
    ULONGLONG addrArray[] = {ReplayInterfacePTR, oReplayInterfaceInterface, oReplayInterfaceInterfacePedList};
    ULONGLONG pedList = pread_ulonglong(addrArray, len(addrArray));
    for (int i = 0; i < 256; i++)
    {
        ULONGLONG ped = read_ulonglong(pedList + i * 0x10);
        if (!check_valid(ped))
        {
            continue;
        }
        ULONGLONG playerAddr = read_ulonglong(ped + oPedPlayerInfo);
        if (check_valid(playerAddr)) // 跳过自己
        {
            continue;
        }
        ULONGLONG navigation = read_ulonglong(ped + oPedNavigation);
        if (!check_valid(navigation))
        {
            continue;
        }
        UCHAR enemyFlags = read_uchar(ped + 0x18C);
        if (enemyFlags > 1)
        {
            if (read_float(ped + oPedHealth) < 100.F) // 跳过已死亡
            {
                continue;
            }
            write_bytes(navigation + oPedNavigationPositionX, position, 12);
            write_bytes(ped + oPedVisualX, position, 12);
        }
    }
}

/**
 * @brief 杀死敌对NPC
 *
 */
void Gtav::kill_enemy_npc()
{
    ULONGLONG addrArray[] = {ReplayInterfacePTR, oReplayInterfaceInterface, oReplayInterfaceInterfacePedList};
    ULONGLONG pedList = pread_ulonglong(addrArray, len(addrArray));
    for (int i = 0; i < 256; i++)
    {
        ULONGLONG ped = read_ulonglong(pedList + i * 0x10);
        if (!check_valid(ped))
        {
            continue;
        }
        ULONGLONG playerAddr = read_ulonglong(ped + oPedPlayerInfo);
        if (check_valid(playerAddr)) // 跳过自己
        {
            continue;
        }
        UCHAR enemyFlags = read_uchar(ped + 0x18C);
        if (enemyFlags > 1)
        {
            write_float(ped + oPedHealth, -1.F);
        }
    }
}

/**
 * @brief 摧毁敌对NPC载具
 *
 */
void Gtav::destroy_enemy_vehicles()
{
    ULONGLONG addrArray[] = {ReplayInterfacePTR, oReplayInterfaceInterface, oReplayInterfaceInterfacePedList};
    ULONGLONG pedList = pread_ulonglong(addrArray, len(addrArray));
    for (int i = 0; i < 256; i++)
    {
        ULONGLONG ped = read_ulonglong(pedList + i * 0x10);
        if (!check_valid(ped))
        {
            continue;
        }
        ULONGLONG playerAddr = read_ulonglong(ped + oPedPlayerInfo);
        if (check_valid(playerAddr)) // 跳过自己
        {
            continue;
        }
        UCHAR enemyFlags = read_uchar(ped + 0x18C);
        ULONGLONG vehicleAddr = read_ulonglong(ped + oPedVehicle);
        if (enemyFlags > 1)
        {
            ULONGLONG vehicleAddr = read_ulonglong(ped + oPedVehicle);
            write_float(vehicleAddr + oPedVehicleHealth, -1.F);
            write_float(vehicleAddr + 0x820, -1.F); // 车体
            write_float(vehicleAddr + 0x824, -1.F); // 油箱
            write_float(vehicleAddr + 0x8E8, -1.F); // 引擎
        }
    }
}

/**
 * @brief 设置幸运转盘结果
 *
 * @param value
 */
void Gtav::set_luckyWheel(INT value)
{
    ULONGLONG addr = localAddress((CHAR *)"casino_lucky_wheel", 276 + 14);
    if (check_valid(addr))
    {
        write_int(addr, value);
    }
}

/**
 * @brief 终止进程
 *
 */
void Gtav::kill_process()
{
    if (hProcess != 0)
    {
        TerminateProcess(hProcess, 0);
        pid = 0;
        hProcess = 0;
        hModule = 0;
        moduleSize = 0;
    }
}

/**
 * @brief stat写入
 *
 * @param stat
 * @param value
 */
void Gtav::stat_write(CHAR *stat, INT value)
{
    CHAR *s = new CHAR[strlen(stat) + 1]{0};
    int i = 0;
    while (stat[i] != '\0')
    {
        if (stat[i] != '$')
        {
            s[i] = tolower(stat[i]);
        }
        i++;
    }
    if (memcmp(s, (CHAR *)"mpx_", 4) == 0)
    {
        INT playerNum = read_gaInt(1574918); // 获取玩家序号
        if (playerNum == 0)
        {
            s[2] = '0';
        }
        else if (playerNum == 1)
        {
            s[2] = '1';
        }
        else
        {
            return;
        }
    }
    UINT stat_ResotreHash = read_gaUint(1665476 + 1 + 3);
    INT stat_ResotreValue = read_gaInt(980531 + 5525);
    write_gaUint(1665476 + 1 + 3, joaat(s));
    write_gaInt(980531 + 5525, value);
    write_gaInt(1654054 + 1139, -1);
    Sleep(1000);
    write_gaUint(1665476 + 1 + 3, stat_ResotreHash);
    write_gaInt(980531 + 5525, stat_ResotreValue);
    delete[] s;
}

/**
 * @brief dll注入
 *
 * @param dllPath
 */
void Gtav::dll_inject(CHAR *dllPath)
{
    HANDLE hProcess = get_hProcess(pid); // 进程句柄
    if (hProcess == 0)
    {
        return;
    }
    CHAR dllPathCopy[MAX_PATH];
    strcpy(dllPathCopy, dllPath);
    vector<char *> dllPathSplit = split(dllPathCopy, "\\");
    WCHAR *wDllName = new WCHAR[strlen(dllPathSplit[dllPathSplit.size() - 1]) + 1]{0};
    mbstowcs(wDllName, dllPathSplit[dllPathSplit.size() - 1], strlen(dllPathSplit[dllPathSplit.size() - 1]) + 1);
    if (get_hModule(pid, dllPathSplit[dllPathSplit.size() - 1], nullptr) != 0)
    {
        WCHAR msg[128];
        wcscpy(msg, wDllName);
        wcscat(msg, L"已经存在，终止注入\n");
        MessageBoxW(NULL, msg, L"失败", MB_ICONERROR | MB_OK);
    }
    else
    {
        SIZE_T length = strlen(dllPath);
        LPVOID startAddress = VirtualAllocEx(hProcess, NULL, length + 1, MEM_COMMIT, PAGE_READWRITE);
        if (WriteProcessMemory(hProcess, startAddress, dllPath, length, NULL))
        {
            PTHREAD_START_ROUTINE LoadLibrary = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleW(L"Kernel32.dll"), "LoadLibraryA");
            HANDLE hThread = CreateRemoteThreadEx(hProcess, NULL, 0, LoadLibrary, (LPVOID)startAddress, 0, NULL, NULL);
            WaitForSingleObject(hThread, INFINITE); // 等待hThread线程执行完
            CloseHandle(hThread);
            CloseHandle(hProcess);
        }
    }
    delete[] wDllName;
}

/**
 * @brief localAddress写入INT类型数据
 *
 * @param threadName
 * @param offset
 * @param value
 */
void Gtav::localAddress(CHAR *threadName, ADDR offset, INT value)
{
    write_int(localAddress(threadName, offset), value);
}

/**
 * @brief localAddress写入UINT类型数据
 *
 * @param threadName
 * @param offset
 * @param value
 */
void Gtav::localAddress(CHAR *threadName, ADDR offset, UINT value)
{
    write_uint(localAddress(threadName, offset), value);
}

/**
 * @brief localAddress写入FLOAT类型数据
 *
 * @param threadName
 * @param offset
 * @param value
 */
void Gtav::localAddress(CHAR *threadName, ADDR offset, FLOAT value)
{
    write_float(localAddress(threadName, offset), value);
}

INT Gtav::read_localAddressInt(CHAR *threadName, ADDR offset)
{
    return read_int(localAddress(threadName, offset));
}
UINT Gtav::read_localAddressUint(CHAR *threadName, ADDR offset)
{
    return read_uint(localAddress(threadName, offset));
}
FLOAT Gtav::read_localAddressFloat(CHAR *threadName, ADDR offset)
{
    return read_float(localAddress(threadName, offset));
}

/**
 * @brief 读取分红
 *
 * @param data INT数组
 */
void Gtav::read_cut(INT *data)
{

    data[0] = read_gaInt(1938365 + 3008 + 1); // 公寓豪劫
    data[1] = read_gaInt(1938365 + 3008 + 2);
    data[2] = read_gaInt(1938365 + 3008 + 3);
    data[3] = read_gaInt(1938365 + 3008 + 4);

    data[4] = read_gaInt(1967630 + 812 + 50 + 1); // 末日豪劫
    data[5] = read_gaInt(1967630 + 812 + 50 + 2);
    data[6] = read_gaInt(1967630 + 812 + 50 + 3);
    data[7] = read_gaInt(1967630 + 812 + 50 + 4);

    data[8] = read_gaInt(1971696 + 1497 + 736 + 92 + 1); // 赌场豪动
    data[9] = read_gaInt(1971696 + 1497 + 736 + 92 + 2);
    data[10] = read_gaInt(1971696 + 1497 + 736 + 92 + 3);
    data[11] = read_gaInt(1971696 + 1497 + 736 + 92 + 4);

    data[12] = read_gaInt(1978495 + 825 + 56 + 1); // 佩里科岛豪劫
    data[13] = read_gaInt(1978495 + 825 + 56 + 2);
    data[14] = read_gaInt(1978495 + 825 + 56 + 3);
    data[15] = read_gaInt(1978495 + 825 + 56 + 4);
}

/**
 * @brief 写入分红
 *
 * @param data INT数组
 */
void Gtav::write_cut(INT *data)
{
    write_gaInt(1938365 + 3008 + 1, data[0]); // 公寓豪劫
    write_gaInt(1938365 + 3008 + 2, data[1]); // 公寓豪劫
    write_gaInt(1938365 + 3008 + 3, data[2]); // 公寓豪劫
    write_gaInt(1938365 + 3008 + 4, data[3]); // 公寓豪劫

    write_gaInt(1967630 + 812 + 50 + 1, data[4]); // 末日豪劫
    write_gaInt(1967630 + 812 + 50 + 2, data[5]); // 末日豪劫
    write_gaInt(1967630 + 812 + 50 + 3, data[6]); // 末日豪劫
    write_gaInt(1967630 + 812 + 50 + 4, data[7]); // 末日豪劫

    write_gaInt(1971696 + 1497 + 736 + 92 + 1, data[8]);  // 赌场豪动
    write_gaInt(1971696 + 1497 + 736 + 92 + 2, data[9]);  // 赌场豪动
    write_gaInt(1971696 + 1497 + 736 + 92 + 3, data[10]); // 赌场豪动
    write_gaInt(1971696 + 1497 + 736 + 92 + 4, data[11]); // 赌场豪动

    write_gaInt(1978495 + 825 + 56 + 1, data[12]); // 佩里科岛豪劫
    write_gaInt(1978495 + 825 + 56 + 2, data[13]); // 佩里科岛豪劫
    write_gaInt(1978495 + 825 + 56 + 3, data[14]); // 佩里科岛豪劫
    write_gaInt(1978495 + 825 + 56 + 4, data[15]); // 佩里科岛豪劫
}

/**
 * @brief 挂机防踢
 *
 * @param status
 */
void Gtav::antiAFK(bool status)
{
    write_gaInt(262145 + 87, status ? 99999999 : 120000);
    write_gaInt(262145 + 88, status ? 99999999 : 300000);
    write_gaInt(262145 + 89, status ? 99999999 : 600000);
    write_gaInt(262145 + 90, status ? 99999999 : 900000);
    // 742014
    write_gaInt(262145 + 8420, status ? 2000000000 : 30000);
    write_gaInt(262145 + 8421, status ? 2000000000 : 60000);
    write_gaInt(262145 + 8422, status ? 2000000000 : 90000);
    write_gaInt(262145 + 8423, status ? 2000000000 : 120000);
}

/**
 * @brief 调用外部的GTAHaX程序
 *
 */
void using_GTAHaX()
{
    FILE *fp;
    if ((fp = fopen("GTAHaXUI.exe", "rb")) == NULL)
    {
        return;
    }
    STARTUPINFOA si{0};
    PROCESS_INFORMATION pi{0};
    si.cb = sizeof(si);
    if (!CreateProcessA(
            NULL,
            (LPSTR) "GTAHaXUI.exe",
            NULL,
            NULL,
            FALSE,
            CREATE_NEW_CONSOLE,
            NULL,
            NULL,
            &si,
            &pi))
    {
        printf("打开GTAHaXUI.exe失败 (%u).\n", (unsigned int)GetLastError());
        return;
    }
    else
    {
        Sleep(1000);
        HWND hW = FindWindowA((LPCSTR) "#32770", (LPCSTR) "man why can't life always be this easy");
        HWND btn = FindWindowExA(hW, NULL, (LPCSTR) "Button", (LPCSTR) "Import from file");
        SendMessageA(btn, WM_LBUTTONDOWN, 0, 0);
        Sleep(20);
        SendMessageA(btn, WM_LBUTTONUP, 0, 0);
        return;
    }
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}
