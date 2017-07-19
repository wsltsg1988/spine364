
local MainScene = class("MainScene", function()
    return display.newScene("MainScene")
end)

function MainScene:ctor()
    --[[cc.ui.UILabel.new({
            UILabelType = 2, text = "Hello, World", size = 64})
        :align(display.CENTER, display.cx, display.cy)
        :addTo(self)]]--

    --[[local addSpine = sp.SkeletonAnimation:create("common/coin-pro.json","common/coin.atlas",1.0)
    addSpine:setAnimation(888,"rotate",true)
    addSpine:setNormalizedPosition(cc.p(0.2,0.3)):addTo(self)    

    local addSpine1 = sp.SkeletonAnimation:create("common/goblins-pro.json","common/goblins.atlas",1.0)
    addSpine1:setAnimation(888,"walk",true)
    addSpine1:setSkin("goblin")
    addSpine1:setNormalizedPosition(cc.p(0.5,0.1)):addTo(self)  


    local addSpine2 = sp.SkeletonAnimation:create("common/raptor-pro.json","common/raptor.atlas",1.0)
    addSpine2:setAnimation(888,"walk",true)
    addSpine2:setAnimation(889,"empty",true)
    addSpine2:addAnimation(889,"gungrab",false, 2)
    addSpine2:setScale(0.5)
    --addSpine2:setTwoColorTint(true)
    addSpine2:setNormalizedPosition(cc.p(0.8,0.2)):addTo(self)


    local addSpine3 = sp.SkeletonAnimation:create("common/spineboy-ess.json","common/spineboy.atlas",1.0)
    addSpine3:setMix("walk", "jump", 0.4)
    addSpine3:setMix("jump", "run", 0.4)
    addSpine3:setAnimation(0, "walk", true)
    addSpine3:addAnimation(0, "jump", false, 1)
    addSpine3:addAnimation(0, "run", true)
    addSpine3:setScale(0.5)  
    addSpine3:setNormalizedPosition(cc.p(0.25,0.1)):addTo(self)]]


    --[[local addSpine4 = sp.SkeletonAnimation:create("common/tank-pro.json","common/tank.atlas",1.0)
    addSpine4:setAnimation(0, "drive", true)
    addSpine4:setScale(0.2)  
    addSpine4:setNormalizedPosition(cc.p(0.5,0.6)):addTo(self)]]--

    --local bg = cc.LayerColor:create(cc.c4b(121, 121, 121, 180)):addTo(self)
    local addSpine5 = sp.SkeletonAnimation:create("ceshi2/yeniu.json","ceshi2/yeniu.atlas",1.0)
    --addSpine5:setAnimation(0, "attack", true)
    --addSpine5:setScale(1.0)
    addSpine5:setNormalizedPosition(cc.p(0.5,0.5)):addTo(self,999)

end

function MainScene:onEnter()
end

function MainScene:onExit()
end

return MainScene
