local config = {
level = 10,
days = 1,
storageId = 15558,
msg = "Parab�ns, por jogar Offensive Global, voc� recebeu 1 dia de VIP gratis, desloga-se e loga-se para mudar a sua voca��o!!!",
msgtype = MESSAGE_EVENT_ADVANCE
}

function onAdvance(cid, oldLevel, newLevel)
if (getPlayerLevel(cid) >= config.level and getGlobalStorageValue(getPlayerAccountId(cid)+450) <= 0) then
setGlobalStorageValue(getPlayerAccountId(cid)+450, 1)
vip.addVipByAccount(getPlayerAccount(cid) ,vip.getDays(config.days))
doPlayerSendTextMessage(cid, config.msgtype, config.msg)

end

return TRUE
end