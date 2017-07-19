
#include "lua_extensions.h"

#if __cplusplus
extern "C" {
#endif
// socket
#include "luasocket/luasocket.h"
#include "luasocket/luasocket_scripts.h"
#include "luasocket/mime.h"
#include "sproto/lsproto.h"
#include "lpeg/lptypes.h"
#include "lpeg/lpcap.h"
#include "lpeg/lpcode.h"
#include "lpeg/lpprint.h"
#include "lpeg/lptree.h"
#include "lpeg/lpvm.h"
#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)
#include "bitop/bit.h"
#endif
#include "struct/struct.h"

static luaL_Reg luax_exts[] = {
    {"socket.core", luaopen_socket_core},
    {"mime.core", luaopen_mime_core},
#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)
    {"bit", luaopen_bit},
#endif
    {"lpeg", luaopen_lpeg},
    {"sproto.core", luaopen_sproto_core},
    {"struct",luaopen_struct},
    {NULL, NULL}
};

void luaopen_lua_extensions(lua_State *L)
{
    // load extensions
    luaL_Reg* lib = luax_exts;
    lua_getglobal(L, "package");
    lua_getfield(L, -1, "preload");
    for (; lib->func; lib++)
    {
        lua_pushcfunction(L, lib->func);
        lua_setfield(L, -2, lib->name);
    }
    lua_pop(L, 2);

    luaopen_luasocket_scripts(L);
}

#if __cplusplus
} // extern "C"
#endif
