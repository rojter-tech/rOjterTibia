    function onUse(cid, item, toPosition)

    rock1pos = {x=1879, y=306, z=10, stackpos=1} -- Posi��o da Pedra
    getrock1 = getThingfromPos(rock1pos)
    UniID = 12121 -- UniqueID que vai ser adicionado na alavanca
    rockID = 1304 -- ID da pedra

    if item.uid == UniID and item.itemid == 1945 and getrock1.itemid == rockID then
    doRemoveItem(getrock1.uid, 1)
    doTransformItem(item.uid, item.itemid+1)
    doSendMagicEffect(rock1pos, 2)
    elseif item.uid == UniID and item.itemid == 1946 then
    doCreateItem(rockID, 1, rock1pos)
    doTransformItem(item.uid,item.itemid-1)
    doSendMagicEffect(rock1pos, 13)
    end
    return TRUE
    end