function onUse(cid, item, frompos, item2, topos)

if getPlayerStorageValue(cid, 7190) < 1 then
doPlayerSendTextMessage(cid,22,"Voce acabou de conseguir um boots of haste, Parabens!!!")
local bag = doPlayerAddItem(cid, 2195, 1)
setPlayerStorageValue(cid, 7190, 1)
else
doPlayerSendTextMessage(cid, MESSAGE_INFO_DESCR, "It is empty.")
end

return TRUE
end