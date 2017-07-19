#ifndef __TCP_SOCKET_H_
#define __TCP_SOCKET_H_

#ifndef WIN32
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <netdb.h>

#define SOCKET int
#define SOCKET_ERROR -1
#else
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
#endif

#include "CircularBuffer.h"
#include "extensions/ExtensionMacros.h"
#include <mutex>

NS_CC_EXT_BEGIN

class TsMessage;
class TsThreadHelper;

class CC_DLL TcpSocket
{
public:
	TcpSocket();
	virtual ~TcpSocket();

	/**
     *  @brief Data structure for message
     */
	struct Data
	{
        char* data;
        unsigned short size;
	};

    /**
     *  @brief Errors in tcpsocket
     */
    enum ErrorCode
    {
        kErrorTimeout = 0,
        kErrorConnectionFailure,
        kErrorUnknown
    };

	/**
     *  @brief The delegate class to process tcpsocket events.
     *  @js NA
     *  @lua NA
     */
    class Delegate
    {
    public:
        virtual ~Delegate() {}
        virtual void onOpen(TcpSocket* ts) = 0;
        virtual void onMessage(TcpSocket* ts, const char* data, unsigned short size) = 0;
        virtual void onClose(TcpSocket* ts) = 0;
        virtual void onError(TcpSocket* ts, const ErrorCode& error) = 0;
    };

	/**
     *  @brief  The initialized method for tcpsocket.
     *          It needs to be invoked right after tcpsocket instance is allocated.
     *  @param  delegate The delegate which want to receive event from tcpsocket.
     *  @param  url      The URL of tcpsocket server.
     *  @return true: Success, false: Failure
     */
    bool init(const Delegate& delegate, const std::string& url, unsigned short port);

	bool close();
	bool reConnect();
	bool sendUIMessage(const char* data, int dataLength);

	/**
     *  Tcpsocket state
     */
    enum State
    {
        kTcpStateConnecting = 0,
        kTcpStateOpen,
        kTcpStateClosing,
        kTcpStateClosed
    };

	/**
     *  @brief Gets current state of connection.
     */
	State getReadyState()
	{
		return _readyState;
	}
private:
	virtual bool onSubThreadStarted();
	virtual int onSubThreadLoop();
	virtual void onSubThreadEnded();
	virtual void onUIThreadReceiveMessage(TsMessage* msg);

	virtual void onRead();
	virtual void onErrorClose();

	bool readInData();
	void writeOutData();
	void updateQueuedData();

	// Create file descriptor for socket i/o operations.
	SOCKET createTCPFileDescriptor();
	// Disable blocking send/recv calls.
	bool setNonblocking(SOCKET fd);

	// Enable blocking send/recv calls.
	bool setBlocking(SOCKET fd);

	bool fatalError();

	const static int RECV_BUFFER_SIZE = 64 * 1024;
	const static int SEND_BUFFER_SIZE = 64 * 1024;

	State        _readyState;
	std::string  _host;
	int          _port;

#ifndef WIN32
	int _socket;
#else
	SOCKET _socket;
#endif
	sockaddr_in _client;
	CircularBuffer _readBuffer;

    std::mutex _writeMuxtex;

	std::list<Data> _dataQueue;
	CircularBuffer _writeBuffer;

	unsigned short _remaining;
	unsigned short _messageSize;
	unsigned short _messageCmd;

	friend class TsThreadHelper;
	TsThreadHelper* _threadHelper;

	Delegate* _delegate;
};

NS_CC_EXT_END

#endif
