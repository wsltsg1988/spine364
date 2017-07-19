local struct = require "struct"
local TcpSockets = class("TcpSockets")

TcpSockets.OPEN_EVENT    = "open"
TcpSockets.MESSAGE_EVENT = "message"
TcpSockets.CLOSE_EVENT   = "close"
TcpSockets.ERROR_EVENT   = "error"

function TcpSockets:ctor(host, port)
    cc(self):addComponent("components.behavior.EventProtocol"):exportMethods()
    self.socket = TcpSocket:create(host, port)

    if self.socket then
        self.socket:registerScriptHandler(handler(self, self.onOpen_), kTcpSocketScriptHandlerOpen)
        self.socket:registerScriptHandler(handler(self, self.onMessage_), kTcpSocketScriptHandlerMessage)
        self.socket:registerScriptHandler(handler(self, self.onClose_), kTcpSocketScriptHandlerClose)
        self.socket:registerScriptHandler(handler(self, self.onError_), kTcpSocketScriptHandlerError)
    end
end

function TcpSockets:isReady()
    return self.socket and self.socket:getReadyState() == kTcpStateOpen
end

function TcpSockets:reConnect()
    return self.socket:reConnect()
end

function TcpSockets:send(actionCode, data)
    if not self:isReady() then
        echoError("TcpSockets:send() - socket isn't ready")
        return false
    end

    local sendData = struct.pack("H", actionCode)
    sendData = sendData .. data
    return self.socket:sendUIMessage(sendData, #sendData)
end

function TcpSockets:close()
    if self.socket then
        self.socket = nil
    end
    self:removeAllEventListeners()
end

function TcpSockets:onOpen_()
    self:dispatchEvent({name = TcpSockets.OPEN_EVENT})
end

function TcpSockets:onMessage_(message)
    local cmd = struct.unpack("H", string.sub(message, 1, 2))
    local body = string.sub(message, 3)
    
    self:dispatchEvent({
        name = TcpSockets.MESSAGE_EVENT,
        cmd = cmd,
        message = body,
    })
end

function TcpSockets:onClose_()
    self:dispatchEvent({name = TcpSockets.CLOSE_EVENT})
end

function TcpSockets:onError_(error)
    self:dispatchEvent({name = TcpSockets.ERROR_EVENT, error = error})
end

return TcpSockets
