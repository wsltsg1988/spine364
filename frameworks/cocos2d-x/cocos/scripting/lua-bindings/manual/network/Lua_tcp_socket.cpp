#include "tolua_fix.h"
#include "Lua_tcp_socket.h"
#include "cocos2d.h"
#include "network/TcpSocket.h"
#include "CCLuaStack.h"
#include "CCLuaValue.h"
#include "CCLuaEngine.h"

using namespace cocos2d;
using namespace cocos2d::extension;

LuaTcpSocket::~LuaTcpSocket()
{
    this->unregisterScriptHandler(kTcpSocketScriptHandlerOpen);
    this->unregisterScriptHandler(kTcpSocketScriptHandlerMessage);
    this->unregisterScriptHandler(kTcpSocketScriptHandlerClose);
    this->unregisterScriptHandler(kTcpSocketScriptHandlerError);
}


void LuaTcpSocket::registerScriptHandler(int nFunID, TcpSocketScriptHandlerType scriptHandlerType)
{
    this->unregisterScriptHandler(scriptHandlerType);
    m_mapScriptHandler[scriptHandlerType] = nFunID;
}

void LuaTcpSocket::unregisterScriptHandler(TcpSocketScriptHandlerType scriptHandlerType)
{
    std::map<int,int>::iterator Iter = m_mapScriptHandler.find(scriptHandlerType);
        
    if (m_mapScriptHandler.end() != Iter)
    {
        m_mapScriptHandler.erase(Iter);
    }
}

int LuaTcpSocket::getScriptHandler(TcpSocketScriptHandlerType scriptHandlerType)
{
    std::map<int,int>::iterator Iter = m_mapScriptHandler.find(scriptHandlerType);
        
    if (m_mapScriptHandler.end() != Iter)
        return Iter->second;
    
    return -1;
}
    
void LuaTcpSocket::InitScriptHandleMap()
{
    m_mapScriptHandler.clear();
}
    
void LuaTcpSocket::onOpen(TcpSocket* ts)
{
    LuaTcpSocket* luaWs = dynamic_cast<LuaTcpSocket*>(ts);
    if (NULL != luaWs) {
        int nHandler = luaWs->getScriptHandler(LuaTcpSocket::kTcpSocketScriptHandlerOpen);
        if (0 != nHandler)
        {
            CommonScriptData data(nHandler, "");
            ScriptEvent event(kCommonEvent, (void*)&data);
            ScriptEngineManager::getInstance()->getScriptEngine()->sendEvent(&event);
        }
    }
}
    
void LuaTcpSocket::onMessage(TcpSocket* ts, const char* data, unsigned short size)
{
    LuaTcpSocket* luaTs = dynamic_cast<LuaTcpSocket*>(ts);
    if (NULL != luaTs) {
        int nHandler = luaTs->getScriptHandler(LuaTcpSocket::kTcpSocketScriptHandlerMessage);
        if (0 != nHandler)
        {
            LuaStack* stack = LuaEngine::getInstance()->getLuaStack();
            stack->pushFunctionByHandler(nHandler);
            stack->pushString(data, size);
            stack->executeFunction(1);
        }
    }
}
    
void LuaTcpSocket::onClose(TcpSocket* ts)
{
    LuaTcpSocket* luaTs = dynamic_cast<LuaTcpSocket*>(ts);
    if (NULL != luaTs)
    {
        int nHandler = luaTs->getScriptHandler(LuaTcpSocket::kTcpSocketScriptHandlerClose);
        if (0 != nHandler) {
            CommonScriptData data(nHandler, "");
            ScriptEvent event(kCommonEvent, (void*)&data);
            ScriptEngineManager::getInstance()->getScriptEngine()->sendEvent(&event);
        }
    }
}
    
void LuaTcpSocket::onError(TcpSocket* ts, const TcpSocket::ErrorCode& error)
{
    LuaTcpSocket* luaTs = dynamic_cast<LuaTcpSocket*>(ts);
    if (NULL != luaTs)
    {
        int nHandler = luaTs->getScriptHandler(LuaTcpSocket::kTcpSocketScriptHandlerError);
        std::string errorMsg = "";
        if (error == kErrorTimeout)
        {
            errorMsg = "timeout";
        }
        else if (error == kErrorConnectionFailure)
        {
            errorMsg = "connection";
        }
        else
        {
            errorMsg = "unknown";
        }
        if (0 != nHandler) {
            CommonScriptData data(nHandler, errorMsg.c_str());
            ScriptEvent event(kCommonEvent, (void*)&data);
            ScriptEngineManager::getInstance()->getScriptEngine()->sendEvent(&event);
        }
    }
}

#ifdef __cplusplus
static int tolua_collect_TcpSocket (lua_State* tolua_S)
{
	LuaTcpSocket* self = (LuaTcpSocket*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}
#endif
/* function to release collected object via destructor */
static void tolua_reg_Tcp_Socket_type(lua_State* tolua_S)
{
	tolua_usertype(tolua_S, "TcpSocket");
}

/* method: create of class  TcpSocket */
#ifndef TOLUA_DISABLE_tolua_cocos2dx_TcpSocket_create00
static int tolua_cocos2dx_TcpSocket_create00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertable(tolua_S,1,"TcpSocket",0,&tolua_err) ||
		!tolua_isstring(tolua_S,2,0,&tolua_err) ||
		!tolua_isnumber(tolua_S,3,0,&tolua_err) ||
		!tolua_isnoobj(tolua_S,4,&tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		const char* url = ((const char*)  tolua_tostring(tolua_S,2,0));
		unsigned short port = ((unsigned short)  tolua_tonumber(tolua_S,3,0));
		LuaTcpSocket *tSocket = new LuaTcpSocket();
		tSocket->init(*tSocket, url, port);
		tolua_pushusertype(tolua_S,(void*)tSocket,"TcpSocket");
		tolua_register_gc(tolua_S, lua_gettop(tolua_S));
	}
	return 1;
#ifndef TOLUA_RELEASE
tolua_lerror:
	tolua_error(tolua_S,"#ferror in function 'create'.",&tolua_err);
	return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: close of class  TcpSocket */
#ifndef TOLUA_DISABLE_tolua_cocos2dx_TcpSocket_close00
static int tolua_cocos2dx_TcpSocket_close00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S,1,"TcpSocket",0,&tolua_err) ||
		!tolua_isnoobj(tolua_S,2,&tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		LuaTcpSocket* self = (LuaTcpSocket*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S,"invalid 'self' in function 'close'", NULL);
#endif
		{
			bool tolua_ret = (bool)  self->close();
			tolua_pushboolean(tolua_S,(bool)tolua_ret);
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
tolua_lerror:
	tolua_error(tolua_S,"#ferror in function 'close'.",&tolua_err);
	return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: reConnect of class  TcpSocket */
#ifndef TOLUA_DISABLE_tolua_cocos2dx_TcpSocket_reConnect00
static int tolua_cocos2dx_TcpSocket_reConnect00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S,1,"TcpSocket",0,&tolua_err) ||
		!tolua_isnoobj(tolua_S,2,&tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		LuaTcpSocket* self = (LuaTcpSocket*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S,"invalid 'self' in function 'reConnect'", NULL);
#endif
		{
			bool tolua_ret = (bool)  self->reConnect();
			tolua_pushboolean(tolua_S,(bool)tolua_ret);
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
tolua_lerror:
	tolua_error(tolua_S,"#ferror in function 'reConnect'.",&tolua_err);
	return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: sendUIMessage of class  TcpSocket */
#ifndef TOLUA_DISABLE_tolua_cocos2dx_TcpSocket_sendUIMessage00
static int tolua_cocos2dx_TcpSocket_sendUIMessage00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S,1,"TcpSocket",0,&tolua_err) ||
		!tolua_isstring(tolua_S,2,0,&tolua_err) ||
		!tolua_isnumber(tolua_S,3,0,&tolua_err) ||
		!tolua_isnoobj(tolua_S,4,&tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		LuaTcpSocket* self = (LuaTcpSocket*)  tolua_tousertype(tolua_S,1,0);
		const char* data = ((const char*)  tolua_tostring(tolua_S,2,0));
		int dataLength = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S,"invalid 'self' in function 'sendUIMessage'", NULL);
#endif
		{
			bool tolua_ret = (bool)  self->sendUIMessage(data,dataLength);
			tolua_pushboolean(tolua_S,(bool)tolua_ret);
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
tolua_lerror:
	tolua_error(tolua_S,"#ferror in function 'sendUIMessage'.",&tolua_err);
	return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getReadyState of class  TcpSocket */
#ifndef TOLUA_DISABLE_tolua_cocos2dx_TcpSocket_getReadyState00
static int tolua_cocos2dx_TcpSocket_getReadyState00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S,1,"TcpSocket",0,&tolua_err) ||
		!tolua_isnoobj(tolua_S,2,&tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		LuaTcpSocket* self = (LuaTcpSocket*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getReadyState'", NULL);
#endif
		{
			int tolua_ret = (int)  self->getReadyState();
			tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
tolua_lerror:
	tolua_error(tolua_S,"#ferror in function 'getReadyState'.",&tolua_err);
	return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: registerScriptHandler of class  TcpSocket */
#ifndef TOLUA_DISABLE_tolua_cocos2dx_TcpSocket_registerScriptHandler00
static int tolua_cocos2dx_TcpSocket_registerScriptHandler00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S,1,"TcpSocket",0,&tolua_err) ||
		(tolua_isvaluenil(tolua_S,2,&tolua_err) || !toluafix_isfunction(tolua_S,2,"LUA_FUNCTION",0,&tolua_err)) ||
		!tolua_isnumber(tolua_S,3,0,&tolua_err) ||
		!tolua_isnoobj(tolua_S,4,&tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		LuaTcpSocket* self = (LuaTcpSocket*)  tolua_tousertype(tolua_S,1,0);
		LUA_FUNCTION nFunID = (  toluafix_ref_function(tolua_S,2,0));
		int scriptHandlerType = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S,"invalid 'self' in function 'registerScriptHandler'", NULL);
#endif
		{
			LuaTcpSocket::TcpSocketScriptHandlerType handlerType = (LuaTcpSocket::TcpSocketScriptHandlerType)scriptHandlerType;
			self->registerScriptHandler(nFunID,handlerType);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
tolua_lerror:
	tolua_error(tolua_S,"#ferror in function 'registerScriptHandler'.",&tolua_err);
	return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: unregisterScriptHandler of class  TcpSocket */
#ifndef TOLUA_DISABLE_tolua_cocos2dx_TcpSocket_unregisterScriptHandler00
static int tolua_cocos2dx_TcpSocket_unregisterScriptHandler00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S,1,"TcpSocket",0,&tolua_err) ||
		!tolua_isnumber(tolua_S,2,0,&tolua_err) ||
		!tolua_isnoobj(tolua_S,3,&tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		LuaTcpSocket* self = (LuaTcpSocket*)  tolua_tousertype(tolua_S,1,0);
		int scriptHandlerType = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S,"invalid 'self' in function 'unregisterScriptHandler'", NULL);
#endif
		{
			LuaTcpSocket::TcpSocketScriptHandlerType handlerType = (LuaTcpSocket::TcpSocketScriptHandlerType)scriptHandlerType;
			self->unregisterScriptHandler(handlerType);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
tolua_lerror:
	tolua_error(tolua_S,"#ferror in function 'unregisterScriptHandler'.",&tolua_err);
	return 0;
#endif
}
#endif

TOLUA_API int tolua_tcp_socket_open(lua_State* tolua_S){
	tolua_open(tolua_S);
	tolua_reg_Tcp_Socket_type(tolua_S);
	tolua_module(tolua_S,NULL,0);
	tolua_beginmodule(tolua_S,NULL);
	tolua_constant(tolua_S,"kTcpStateConnecting",TcpSocket::kTcpStateConnecting);
	tolua_constant(tolua_S,"kTcpStateOpen",TcpSocket::kTcpStateOpen);
	tolua_constant(tolua_S,"kTcpStateClosing",TcpSocket::kTcpStateClosing);
	tolua_constant(tolua_S,"kTcpStateClosed",TcpSocket::kTcpStateClosed);
	tolua_constant(tolua_S,"kTcpSocketScriptHandlerOpen",LuaTcpSocket::kTcpSocketScriptHandlerOpen);
	tolua_constant(tolua_S,"kTcpSocketScriptHandlerMessage",LuaTcpSocket::kTcpSocketScriptHandlerMessage);
	tolua_constant(tolua_S,"kTcpSocketScriptHandlerClose",LuaTcpSocket::kTcpSocketScriptHandlerClose);
	tolua_constant(tolua_S,"kTcpSocketScriptHandlerError",LuaTcpSocket::kTcpSocketScriptHandlerError);
#ifdef __cplusplus
	tolua_cclass(tolua_S,"TcpSocket","TcpSocket","",tolua_collect_TcpSocket);
#else
	tolua_cclass(tolua_S,"TcpSocket","TcpSocket","",NULL);
#endif
	tolua_beginmodule(tolua_S,"TcpSocket");
	tolua_function(tolua_S, "create", tolua_cocos2dx_TcpSocket_create00);
	tolua_function(tolua_S, "getReadyState", tolua_cocos2dx_TcpSocket_getReadyState00);
	tolua_function(tolua_S, "sendUIMessage", tolua_cocos2dx_TcpSocket_sendUIMessage00);
	tolua_function(tolua_S, "reConnect", tolua_cocos2dx_TcpSocket_reConnect00);
	tolua_function(tolua_S, "close", tolua_cocos2dx_TcpSocket_close00);
	tolua_function(tolua_S, "registerScriptHandler", tolua_cocos2dx_TcpSocket_registerScriptHandler00);
	tolua_function(tolua_S, "unregisterScriptHandler", tolua_cocos2dx_TcpSocket_unregisterScriptHandler00);
	tolua_endmodule(tolua_S);
	tolua_endmodule(tolua_S);
	return 1;
}



TOLUA_API int register_tcp_socket_manual(lua_State* tolua_S)
{
    if (nullptr == tolua_S)
        return 0 ;
    
    lua_pushstring(tolua_S,"cc.TcpSocket");
    lua_rawget(tolua_S,LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S,-1))
    {
        lua_pushstring(tolua_S,"registerScriptHandler");
        lua_pushcfunction(tolua_S,tolua_cocos2dx_TcpSocket_registerScriptHandler00);
        lua_rawset(tolua_S,-3);
        lua_pushstring(tolua_S,"unregisterScriptHandler");
        lua_pushcfunction(tolua_S,tolua_cocos2dx_TcpSocket_unregisterScriptHandler00);
        lua_rawset(tolua_S,-3);
    }
    lua_pop(tolua_S, 1);
    
    return 1;
}

