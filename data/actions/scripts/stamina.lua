function onUse(cid, item, frompos, item2, topos)
	 doPlayerAddStamina(cid, 2520) -- 2520 = 42 horas, se voc� colocar 1 o item ira curar 1 minuto da stamina.
	 doSendMagicEffect(frompos, 1) -- Efeito, para mudar basta alterar o n�mero 1 para o efeito que voc� quiser, /z 1 para ver o efeito no tibia.
	 doRemoveItem(item.uid, 1) -- Se quiser que o item fique infinito, basta alterar o n�mero 1 para 0
return 1
end