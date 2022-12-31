#include "staticdata.h"

StaticData::StaticData() {}

StaticData::~StaticData() {}

QList<StrVar> StaticData::session = {
    {"离开线上模式", -1},
    {"公共战局", 0},
    {"创建公共战局", 1},
    {"私人帮会战局", 2},
    {"帮会战局", 3},
    {"加入好友", 9},
    {"私人好友战局", 6},
    {"单人战局", 10},
    {"仅限邀请战局", 11},
    {"加入帮会伙伴", 12}};

QList<StrVar> StaticData::weather = {
    {"默认", -1},
    {"格外晴朗", 0},
    {"晴朗", 1},
    {"多云", 2},
    {"阴霾", 3},
    {"大雾", 4},
    {"阴天", 5},
    {"下雨", 6},
    {"雷雨", 7},
    {"雨转晴", 8},
    {"阴雨", 9},
    {"下雪", 10},
    {"暴雪", 11},
    {"小雪", 12},
    {"圣诞", 13},
    {"万圣节", 14}};

QList<StrVar> StaticData::weaponAmmoType = {
    {"默认", -1},
    {"MKⅡ爆炸子弹", 18},
    {"信号弹", 22},
    {"燃油泵", 34},
    {"原子能枪", 70}};

QList<StrVar> StaticData::vehicles = {
    {"801巴提", "Bati"},
    {"骷髅马", "Kuruma2"},
    {"阿库拉", "Akula"},
    {"长崎小艇", "Dinghy2"},
    {"图拉尔多", "Toreador"},
    {"狂焰", "Pyro"},
    {"P-996 天煞", "Lazer"},
    {"暴君MK2", "Oppressor2"}};

QList<StrVar> StaticData::vehicleAbilities = {
    {"默认载具能力", 0x00},
    {"载具跳跃", 0x20},
    {"火箭推进", 0x40}};