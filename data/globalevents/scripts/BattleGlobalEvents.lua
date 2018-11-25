function onThink(interval, lastExecution) 
local _Lib_Battle_Days = {
        ["Monday"] = {
            ["21:32"] = {players = 10}    
        },
        ["Tuesday"] = {
            ["18:02"] = {players = 10}    
        },
        ["Saturday"] = {
            ["16:02"] = {players = 10}    
        },
        ["Thursday"] = {
            ["17:02"] = {players = 10}
        }
}
if _Lib_Battle_Days[os.date("%A")] then
local hours = tostring(os.date("%X")):sub(1, 5)       
local tb = _Lib_Battle_Days[os.date("%A")][hours] 
if tb and (tb.players % 2 == 0) then 
local tp = doCreateItem(1387, 1, _Lib_Battle_Info.tpPos)
doItemSetAttribute(tp, "aid", 45000)
CheckEvent(_Lib_Battle_Info.limit_Time)
doBroadcastMessage("The event BattleField was opened and We are waiting "..tb.players.." Players! Team divided into "..((tb.players)/2).." VS "..((tb.players)/2))
setGlobalStorageValue(_Lib_Battle_Info.storage_count, tb.players)
return true
end
end
return true
end