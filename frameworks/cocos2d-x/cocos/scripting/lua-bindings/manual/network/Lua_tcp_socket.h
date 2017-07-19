#ifndef __LUA_TCP_SOCKET_H__
#define __LUA_TCP_SOCKET_H__

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 || CC_TARGET_PLATFORM == CC_PLATFORM_MAC)

#ifdef __cplusplus
extern "C" {
#endif
#include "tolua++.h"
#ifdef __cplusplus
}
#endif

#include "network/TcpSocket.h"
class LuaTcpSocket: public cocos2d::extension::TcpSocket, public cocos2d::extension::TcpSocket::Delegate
{
public:
    enum TcpSocketScriptHandlerType
    {
        kTcpSocketScriptHandlerOpen    = 0,
        kTcpSocketScriptHandlerMessage,
        kTcpSocketScriptHandlerClose,
        kTcpSocketScriptHandlerError,
    };
public:
    virtual ~LuaTcpSocket();
    
    /**
     *  @brief Add Handler of DelegateEvent
     */
    void registerScriptHandler(int nFunID, TcpSocketScriptHandlerType scriptHandlerType);
    
    /**
     *  @brief Remove Handler of DelegateEvent
     */
    void unregisterScriptHandler(TcpSocketScriptHandlerType scriptHandlerType);
    
    /**
     *  @brief Get Handler By DelegateEvent Type
     */
    int  getScriptHandler(TcpSocketScriptHandlerType scriptHandlerType);
    
    
    void InitScriptHandleMap();
    virtual void onOpen(TcpSocket* ts);
    virtual void onMessage(TcpSocket* ts, const char* data, unsigned short size);
    virtual void onClose(TcpSocket* ts);
    virtual void onError(TcpSocket* ts, const TcpSocket::ErrorCode& error);
    
    /*
     * @brief  delegate event enum,for lua register handler
     */

    
private:
    std::map<int,int> m_mapScriptHandler;
};

TOLUA_API int tolua_tcp_socket_open(lua_State* tolua_S);
TOLUA_API int register_tcp_socket_manual(lua_State* tolua_S);

#endif //(CC_TARGET_PLATFORM == CC_PLATFORM_IOS ...

#endif //__LUA_WEB_SOCKET_H__
