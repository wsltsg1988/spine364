/* 战斗逻辑实现需要server和client根据相同的随机种子得到完全相同的结果,由于
 * server端可能会同时验算多场战斗,所以需要同时维护多个随机序列,故实现此文件
 * 保证server和client的随机过程完全一致
 *
 * @wangyue 2014－12-17
 */
#include <stdint.h>
#include <lauxlib.h>
#include <lua.h>
//算法提取自 POSIX.1-2001 rand()实现
static int32_t nextRandom(lua_State *L)
{
    uint32_t seed = lua_tointeger(L, lua_upvalueindex(1));
    int32_t ret;
	seed = seed * (int32_t)1103515245 + (int32_t)12345;
    ret = (int32_t)((uint32_t)seed / (int32_t)65536) % (int32_t)32768;
    lua_pushinteger(L, ret);
    lua_pushinteger(L, seed);
    lua_replace(L, lua_upvalueindex(1));
    return 1;
}

//给lua返回一个closure用来生成随机序列
int32_t generator(lua_State *L)
{
    int32_t seed = (int32_t)lua_tonumber(L, 1);
    lua_pushinteger(L, seed);
    lua_pushcclosure(L, nextRandom, 1);
    return 1;
}

int32_t luaopen_myrand(lua_State *L)
{
    luaL_Reg reg[] = {
        {"generator", generator},
        {NULL, NULL}
    };
    luaL_register(L, "myrand", reg);
    return 1;
}
