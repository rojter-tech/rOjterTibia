function onUse(cid, item, frompos, item2, topos)


if item.uid ==7514 then
queststatus = getPlayerStorageValue(cid,7614)
if queststatus == -1 then
doPlayerSendTextMessage(cid,22,"You have found one piece of the broken amulet.")
doPlayerAddItem(cid,8265,1)
setPlayerStorageValue(cid,7614,1)
else
doPlayerSendTextMessage(cid,22,"It is empty.")
end
else
return 0
end

return 1
end