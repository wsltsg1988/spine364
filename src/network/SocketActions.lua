local SocketActions = class("SocketActions")
local scheduler = require("framework.scheduler")

function SocketActions:ctor(app)
	self.app = app
end

function SocketActions:roleLoginResponse(event)
	local msg = pb.decode("RoleLoginResponse", event.data)
	if msg.result == "SUCCESS" then
		-- 表示已经登录
		self.app.role = require("datamodel.Role").new(msg.roleInfo)

		-- 时间戳数据
		for _, data in ipairs(msg.timestamps) do
			self.app.role[data.key] = data.value
		end

		self.app:setServerTime(msg.serverTime)

		-- 切换首页
		switchScene("home")

	elseif msg.result == "NOT_EXIST" then

		switchScene("login", { layer = "chooseHero" })

	elseif msg.result == "HAS_LOGIN" then

	elseif msg.result == "DB_ERROR" then

	end

	return "__REMOVE__"
end

return SocketActions