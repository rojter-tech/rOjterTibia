local combat = createCombatObject()
setCombatParam(combat, COMBAT_PARAM_TYPE, COMBAT_PHYSICALDAMAGE)
setCombatParam(combat, COMBAT_PARAM_EFFECT, CONST_ME_EXPLOSIONAREA)
setCombatParam(combat, COMBAT_PARAM_DISTANCEEFFECT, CONST_ANI_EXPLOSION)
setAttackFormula(combat, COMBAT_FORMULA_LEVELMAGIC, -1, -9, -1, -19, 5, 5, 1.6, 3.19)

function onGetFormulaValues(cid, level, maglevel)
	local min = -((level/5)+(maglevel*1.6)+9)
	local max = -((level/5)+(maglevel*3.19)+19)
	return min, max
end

setCombatCallback(combat, CALLBACK_PARAM_LEVELMAGICVALUE, "onGetFormulaValues")

local area = createCombatArea(AREA_CROSS1X1)
setCombatArea(combat, area)

function onCastSpell(cid, var)
	return doCombat(cid, combat, var)
end
