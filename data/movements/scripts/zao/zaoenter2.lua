function onStepIn(cid, item, position, fromPosition)

local teleport = {x=33120, y=31197, z=7} -- Posi��o do teleport.
local effect = 34 -- Efeito do teleport.

if isPremium(cid) == TRUE then
doTeleportThing(cid, teleport)
doSendMagicEffect(getPlayerPosition(cid), effect)
else
doPlayerSendText(cid, MESSAGE_INFO_DESCR, "Sorry, you need a premium.")
end
return TRUE
end