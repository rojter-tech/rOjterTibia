local zabierz = 300
local kolor = 180

function onStepIn(cid, item, pos)

local zycie = getPlayerHealth(cid)
local topos = getPlayerPosition(cid)

if item.actionid == 9879 and isKnight(cid) == FALSE then
doPlayerAddHealth(cid, -zycie)
doSendAnimatedText(topos, zycie, kolor)
doSendMagicEffect(getPlayerPosition(cid), 15)

elseif item.actionid == 9879 and isKnight(cid) == TRUE then
doPlayerAddHealth(cid, -zabierz)
doSendAnimatedText(topos, zabierz, kolor)
doSendMagicEffect(getPlayerPosition(cid), 15)
end

return 1
end