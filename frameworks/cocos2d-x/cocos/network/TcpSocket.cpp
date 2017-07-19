#include "TcpSocket.h"
#include <thread>
#include <mutex>
#include <queue>
#include <list>
#include <signal.h>
#include <errno.h>

#ifndef WIN32
#include <netinet/tcp.h>
#endif

NS_CC_EXT_BEGIN
	
class TsMessage

{
public:
    TsMessage() : what(0), obj(nullptr){}
    unsigned int what; // message type
    unsigned int param;// message parameter
    const char* obj;
    unsigned short size;
};

/**
 *  @brief Tcpsocket thread helper, it's used for sending message between UI thread and tcpsocket thread.
 */
class TsThreadHelper : public Ref
{
public:
    TsThreadHelper();
    ~TsThreadHelper();
        
    // Creates a new thread
    bool createThread(const TcpSocket& ws);
    // Quits sub-thread (TcpMessage thread).
    void quitSubThread();
    
    // Schedule callback function
    virtual void update(float dt);
    
    // Sends message to UI thread. It's needed to be invoked in sub-thread.
    void sendMessageToUIThread(TsMessage *msg);
    
    // Waits the sub-thread (tcpsocket thread) to exit,
    void joinSubThread();
    
protected:
    void tsThreadEntryFunc();
    
private:
    std::list<TsMessage*>* _UITsMessageQueue;
    std::mutex _UITsMessageQueueMutex;

	unsigned int _updatecount;
    std::thread* _subThreadInstance;

    TcpSocket* _ts;
    bool _needQuit;
    friend class TcpSocket;
};





TsThreadHelper::TsThreadHelper()
: _updatecount(0)
, _needQuit(false)
{
    _UITsMessageQueue = new std::list<TsMessage*>();


    Director::getInstance()->getScheduler()->scheduleUpdate(this, 0, false);
}

TsThreadHelper::~TsThreadHelper()
{
    Director::getInstance()->getScheduler()->unscheduleAllForTarget(this);
    joinSubThread();
    CC_SAFE_DELETE(_subThreadInstance);
    delete _UITsMessageQueue;
}

bool TsThreadHelper::createThread(const TcpSocket& ts)
{
    _ts = const_cast<TcpSocket*>(&ts);
    
    // Creates websocket thread
    _subThreadInstance = new std::thread(&TsThreadHelper::tsThreadEntryFunc, this);
    return true;
}

void TsThreadHelper::quitSubThread()
{
    _needQuit = true;
}

void TsThreadHelper::tsThreadEntryFunc()
{
    _ts->onSubThreadStarted();
    
    while (!_needQuit)
    {
        if (_ts->onSubThreadLoop())
        {
            break;
        }
    }
    
	_ts->onSubThreadEnded();

}

void TsThreadHelper::sendMessageToUIThread(TsMessage *msg)
{
	_UITsMessageQueueMutex.lock();
	_UITsMessageQueue->push_back(msg);
	_UITsMessageQueueMutex.unlock();
}

//void WsThreadHelper::sendMessageToSubThread(WsMessage *msg)
//{
//}


void TsThreadHelper::joinSubThread()
{
    if (_subThreadInstance->joinable())
    {
        _subThreadInstance->join();
    }
}

void TsThreadHelper::update(float dt)
{
	if(!((++_updatecount) % 2) && _ts)
		_ts->updateQueuedData();

	TsMessage *msg = nullptr;

	// Returns quickly if no message
	_UITsMessageQueueMutex.lock();
	if (0 == _UITsMessageQueue->size())
	{
		_UITsMessageQueueMutex.unlock();
		return;
	}

	// Gets message
	msg = *(_UITsMessageQueue->begin());
	_UITsMessageQueue->pop_front();
	_UITsMessageQueueMutex.unlock();

	if (_ts)
	{
		_ts->onUIThreadReceiveMessage(msg);
	}

	CC_SAFE_DELETE(msg);
}

enum TS_MSG {
	TCP_MSG_TO_UITHREAD_OPEN,
	TCP_MSG_TO_UITHREAD_MESSAGE,
	TCP_MSG_TO_UITHREAD_ERROR,
	TCP_MSG_TO_UITHREAD_CLOSE
};

TcpSocket::TcpSocket()
: _remaining(0)
, _messageCmd(0)
, _messageSize(0)
, _threadHelper(nullptr)
, _delegate(nullptr)
, _readyState(kTcpStateClosed)
{
#ifndef WIN32
	_socket = -1;
#else
	WSADATA wsaData;
	WORD version = MAKEWORD(2,2);
    if (WSAStartup(version,&wsaData))
	{
         return;
    }

	_socket = INVALID_SOCKET;
#endif
	_readBuffer.Allocate(RECV_BUFFER_SIZE);
	_writeBuffer.Allocate(SEND_BUFFER_SIZE);
}

TcpSocket::~TcpSocket()
{
	close();

	CC_SAFE_DELETE(_threadHelper);

    _writeMuxtex.lock();
	while(!_dataQueue.empty())
	{
		Data queuedData = _dataQueue.front();
		CC_SAFE_DELETE(queuedData.data);
		_dataQueue.pop_front();
	}
    _writeMuxtex.unlock();

#ifdef WIN32
    WSACleanup();
#endif
}

bool TcpSocket::init(const Delegate& delegate, const std::string& host, unsigned short port)
{
	_host = host;
	_port = port;

	_socket = createTCPFileDescriptor();

	struct hostent* ci = gethostbyname(_host.c_str());
	if(ci == 0) return false;

	_client.sin_family = ci->h_addrtype;
	_client.sin_port = htons(_port);
	memcpy(&_client.sin_addr.s_addr, ci->h_addr_list[0], ci->h_length);

	setBlocking(_socket);
	if(connect(_socket, (const sockaddr*)&_client, sizeof(_client)) == -1)
	{
		return false;
	}
	setNonblocking(_socket);

	_readyState = kTcpStateOpen;

	_delegate = const_cast<Delegate*>(&delegate);

	// TcpSocket thread needs to be invoked at the end of this method.
	_threadHelper = new TsThreadHelper();
	return _threadHelper->createThread(*this);
}

bool TcpSocket::reConnect()
{
	setBlocking(_socket);
	if(connect(_socket, (const sockaddr*)&_client, sizeof(_client)) == -1)
	{
		return false;
	}
	setNonblocking(_socket);

	_readyState = kTcpStateOpen;

	CC_SAFE_DELETE(_threadHelper);
	_threadHelper = new TsThreadHelper();
	return _threadHelper->createThread(*this);
}

bool TcpSocket::close()
{
	Director::getInstance()->getScheduler()->unscheduleAllForTarget(_threadHelper);

	CCLOG("tcpsocket (%p) connection closed by client", this);

	_readyState = kTcpStateClosed;

	_threadHelper->quitSubThread();
	_threadHelper->joinSubThread();

	CC_SAFE_DELETE(_threadHelper);

	// onClose callback needs to be invoked at the end of this method
	// since tcpsocket instance may be deleted in 'onClose'.
	// m_delegate->onClose(this);
#ifndef WIN32
	::close(_socket);
#else
	closesocket(_socket);
#endif

	return true;
}

bool TcpSocket::sendUIMessage(const char* data, int dataLength)
{
	if (this->getReadyState() != kTcpStateOpen)
	{
		return false;
	}

	size_t dataLen = dataLength + sizeof(unsigned short) ;
	char *buffer = new char[dataLen];
	memset(buffer, 0, dataLen);
    
    buffer[0] = (dataLength >> 8) & 0xff;
	buffer[1] = dataLength & 0xff;
	memcpy(buffer + sizeof(unsigned short), data, dataLength);

    _writeMuxtex.lock();
	if (_writeBuffer.GetSpace() < dataLen)
	{
        Data queueData;
        queueData.data = buffer;
        queueData.size = dataLen;
		_dataQueue.push_back(queueData);
        
        _writeMuxtex.unlock();
		return true;
	} 

	_writeBuffer.Write((void*)buffer, dataLen);
    _writeMuxtex.unlock();

	CC_SAFE_DELETE_ARRAY(buffer);	// copy to write buffer

	return true;
}

bool TcpSocket::onSubThreadStarted()
{
	if (_readyState == kTcpStateOpen)
	{
		TsMessage* msg = new TsMessage();
		msg->what = TCP_MSG_TO_UITHREAD_OPEN;
		_threadHelper->sendMessageToUIThread(msg);
	}

	return true;
}

int TcpSocket::onSubThreadLoop()
{
	if (_readyState == State::kTcpStateClosed || _readyState == State::kTcpStateClosing)
	{
		// return 1 to exit the loop.
		return 1;
	}

	readInData();
	writeOutData();

	// Sleep 50 ms
	std::this_thread::sleep_for(std::chrono::microseconds(50));

	// return 0 to continue the loop.
	return 0;
}

void TcpSocket::onSubThreadEnded()
{
	_readyState = kTcpStateClosed;
#ifndef WIN32
	::close(_socket);
#else
	closesocket(_socket);
#endif
}

void TcpSocket::onUIThreadReceiveMessage(TsMessage* msg)
{
	switch (msg->what) 
	{
	case TCP_MSG_TO_UITHREAD_OPEN:
		{
			_delegate->onOpen(this);
		}
		break;
	case TCP_MSG_TO_UITHREAD_MESSAGE:
		{
			_delegate->onMessage(this, msg->obj, msg->size);

			CC_SAFE_DELETE_ARRAY(msg->obj);
		}
		break;
	case TCP_MSG_TO_UITHREAD_CLOSE:
		{
			_delegate->onClose(this);
		}
		break;
	case TCP_MSG_TO_UITHREAD_ERROR:
		{
			// FIXME: The exact error needs to be checked.
			_delegate->onError(this, (TcpSocket::ErrorCode)msg->param);
		}
		break;
	default:
		break;
	}
}

bool TcpSocket::readInData()
{
	if (_readyState == kTcpStateClosed || _readyState == kTcpStateClosing)
	{
		// return 1 to exit the loop.
		return false;
	}

	size_t space = _readBuffer.GetSpace();
	int bytes = recv(_socket, (char*)_readBuffer.GetBuffer(), space, 0);
	if(bytes == SOCKET_ERROR)  
	{
		if (fatalError())
		{
			onErrorClose();
		}
		return false;
	}
	else if (bytes == 0)
	{
#ifndef WIN32
		CCLOG("socket recv occurs error %d", errno);
#else
		CCLOG("socket recv occurs error %d", WSAGetLastError());
#endif
		onErrorClose();
		return false;
	}
	else
	{
		_readBuffer.IncrementWritten(bytes);
		onRead();
	}

	return true;
}

void TcpSocket::writeOutData()
{
	if (_readyState == kTcpStateClosed || _readyState == kTcpStateClosing)
	{
		return;
	}

    _writeMuxtex.lock();
	int bytesWritten = send(_socket, (char*)_writeBuffer.GetBufferStart(), _writeBuffer.GetContiguiousBytes(), 0);
	if(bytesWritten == SOCKET_ERROR)
	{
		if (fatalError())
		{
			onErrorClose();
		}

        _writeMuxtex.unlock();
		return;
	}

	_writeBuffer.Remove(bytesWritten);

    _writeMuxtex.unlock();
}

void TcpSocket::updateQueuedData()
{
    _writeMuxtex.lock();

	while (!_dataQueue.empty()) 
	{
		Data queuedData = _dataQueue.front();
		if (_writeBuffer.GetSpace() < queuedData.size)
		{
			// size not enough, then return;
            _writeMuxtex.unlock();
			return;
		}
		else
		{
			_writeBuffer.Write(queuedData.data, queuedData.size);
			_dataQueue.pop_front();

			CC_SAFE_DELETE_ARRAY(queuedData.data);
		}
	}

    _writeMuxtex.unlock();
}

SOCKET TcpSocket::createTCPFileDescriptor()
{
#ifndef WIN32
	return socket(AF_INET, SOCK_STREAM, 0);
#else
	return socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
#endif
}

bool TcpSocket::setNonblocking(SOCKET fd)
{
#ifndef WIN32
    int flags = fcntl(fd, F_GETFL, 0);
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#else
	u_long arg = 1;
	return (::ioctlsocket(fd, FIONBIO, &arg) == 0);
#endif
}

bool TcpSocket::setBlocking(SOCKET fd)
{
#ifndef WIN32
    int flags = fcntl(fd, F_GETFL, 0);
    return fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);
#else
	u_long arg = 0;
	return (ioctlsocket(fd, FIONBIO, &arg) == 0);
#endif
}

void TcpSocket::onErrorClose()
{
	CCLOG("tcpsocket (%p) connection closed by server", this);

	_threadHelper->quitSubThread();
	_readyState = kTcpStateClosed;

	TsMessage* msg = new TsMessage();
	msg->what = TCP_MSG_TO_UITHREAD_CLOSE;
	_threadHelper->sendMessageToUIThread(msg);
}

bool TcpSocket::fatalError()
{
#ifdef WIN32
	int err = WSAGetLastError();
	if(err != WSAEWOULDBLOCK)
	{
#else
	int err = errno;
	if(err != EINPROGRESS && err != EAGAIN)
	{
#endif
		return true;
	}

	return false;
}

/*
 *	Read ui message from read buffer
 */
void TcpSocket::onRead()
{
	for(;;)
	{
		// Check for the header if we don't have any bytes to wait for.
		if(_remaining == 0)
		{
			if(_readBuffer.GetSize() < sizeof(unsigned short))
			{
				// No header in the packet, let's wait.
				return;
			}

			// Copy from packet buffer into header local var
            char sizeBuf[2] = { 0, 0 };
			_readBuffer.Read((uint8*)&sizeBuf, sizeof(unsigned short));
            
            unsigned short size = (unsigned short)(sizeBuf[0] << 8) | (unsigned short)(sizeBuf[1] & 0xff);
            _remaining = _messageSize = size;
        }

		if(_remaining > 0)
		{
			if(_readBuffer.GetSize() < _remaining)
			{
				// We have a fragmented packet. Wait for the complete one before proceeding.
				return;
			}
		}

		char *buffer = new char[_messageSize];
		memset(buffer, 0, _messageSize);

		if(_remaining > 0)
		{
			// Copy from packet buffer into our actual buffer.
			///Read(mRemaining, (uint8*)Packet->contents());
			_readBuffer.Read(buffer, _remaining);
		}

		TsMessage* msg = new TsMessage();
		msg->what = TCP_MSG_TO_UITHREAD_MESSAGE;
		msg->obj = buffer;
        msg->size = _messageSize;
		_threadHelper->sendMessageToUIThread(msg);

		_remaining = _messageSize = _messageCmd = 0;
	}
}

NS_CC_EXT_END

