local config = {
level = 500, -- level que ir� ganhar
storage = 61860, -- storage
msg = "Voc� ganhou 105 Premium Points por avan�ar ao level 500.", -- mensagem ao ganhar
msgtp = MESSAGE_EVENT_ADVANCE, -- tipo da mensagem
}

function onAdvance(cid, oldLevel, newLevel)
if getPlayerStorageValue(cid, config.storage) < 1 and getPlayerLevel(cid) >= config.level then
addPremiumPoints(cid,105)
setPlayerStorageValue(cid, config.storage, 1)
doPlayerSendTextMessage(cid, config.msgtp, config.msg)
end
return TRUE
end