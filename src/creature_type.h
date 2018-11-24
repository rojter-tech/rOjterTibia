//////////////////////////////////////////////////////////////////////
// OpenTibia - an opensource roleplaying game
//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//////////////////////////////////////////////////////////////////////

#ifndef __OTSERV_CREATURE_TYPE_H__
#define __OTSERV_CREATURE_TYPE_H__

#include "classes.h"
#include "outfit.h"
#include "condition.h"
#include "condition_attributes.h"

#define MAX_LOOTCHANCE 100000
#define MAX_STATICWALK 100

struct LootBlock{
  unsigned short id;
  unsigned short countmax;
  uint32_t chance;

  //optional
  int subType;
  int actionId;
  std::string text;

  typedef std::list<LootBlock> LootItems;
  LootItems childLoot;
  bool dropEmpty;

  LootBlock(){
    id = 0;
    countmax = 0;
    chance = 0;

    subType = -1;
    actionId = -1;
    dropEmpty = false;
  }
};

struct SummonBlock{
  std::string name;
  uint32_t chance;
  uint32_t speed;
};

struct SpellBlock{
  SpellBlock()
  {
    chance = 100;
    speed = 2000;
    damageType = COMBAT_NONE;
    range = 0;
    min = 0;
    max = 0;
    blockedByShield = false;
    blockedByArmor = false;
    needTarget = false;
    aggressive = true;
    configureSpell = true;
    spread = 0;
    length = 0;
    radius = 0;
    field = 0;

    shootEffect = SHOOT_EFFECT_NONE;
    areaEffect = MAGIC_EFFECT_NONE;

    condition.type = CONDITION_NONE;
    condition.interval = 0;
    condition.duration = 0;
  }

  uint32_t chance;
  uint32_t speed;

  std::string name;
  CombatType damageType;
  uint32_t range;
  int32_t min;
  int32_t max;
  bool blockedByShield;
  bool blockedByArmor;
  bool needTarget;
  bool aggressive;
  bool configureSpell;
  uint32_t spread;
  uint32_t length;
  uint32_t radius;
  uint32_t field;

  ShootEffect shootEffect;
  MagicEffect areaEffect;

  struct{
    ConditionId type;
    int32_t interval;
    int32_t duration;
    ConditionEffect effect;
  } condition;
};

struct VoiceBlock{
  std::string text;
  bool yellText;
};

typedef std::list<LootBlock> LootItems;
typedef std::list<SummonBlock> SummonList;
typedef std::vector<VoiceBlock> VoiceVector;
typedef std::list<SpellBlock> SpellList;
typedef std::map<CombatType, int32_t> ElementMap;

class CreatureType{
  CreatureType& operator=(const CreatureType& ct);
public:
  CreatureType();
  CreatureType(const CreatureType& ct);
  ~CreatureType();

  static uint32_t getLootChance();
  void createLoot(Container* corpse) const;

  #define DECLARE_PROPERTY(proptype, propname) \
    proptype& propname(); \
    const proptype& propname() const; \
    void propname(const proptype& v);

  DECLARE_PROPERTY(std::string, name)
  DECLARE_PROPERTY(std::string, nameDescription)
  DECLARE_PROPERTY(std::string, fileLoaded)
  
  DECLARE_PROPERTY(uint64_t, experience)
  DECLARE_PROPERTY(int32_t, defense)
  DECLARE_PROPERTY(int32_t, armor)
  DECLARE_PROPERTY(bool, canPushItems)
  DECLARE_PROPERTY(bool, canPushCreatures)
  DECLARE_PROPERTY(uint32_t, staticAttackChance)
  DECLARE_PROPERTY(int32_t, maxSummons)
  DECLARE_PROPERTY(int32_t, targetDistance)
  DECLARE_PROPERTY(int32_t, fleeHealth)
  DECLARE_PROPERTY(bool, pushable)
  DECLARE_PROPERTY(int32_t, base_speed)
  DECLARE_PROPERTY(int32_t, health)
  DECLARE_PROPERTY(int32_t, health_max)
  
  DECLARE_PROPERTY(OutfitType, outfit)
  DECLARE_PROPERTY(int32_t, corpseId)
  DECLARE_PROPERTY(MechanicType, mechanicImmunities)
  DECLARE_PROPERTY(CombatType, damageImmunities)
  DECLARE_PROPERTY(RaceType, race)
  
  DECLARE_PROPERTY(bool, isSummonable)
  DECLARE_PROPERTY(bool, isIllusionable)
  DECLARE_PROPERTY(bool, isConvinceable)
  DECLARE_PROPERTY(bool, isAttackable)
  DECLARE_PROPERTY(bool, isHostile)
  DECLARE_PROPERTY(bool, isLureable)
  DECLARE_PROPERTY(int32_t, lightLevel)
  DECLARE_PROPERTY(int32_t, lightColor)
  DECLARE_PROPERTY(uint32_t, manaCost)

  DECLARE_PROPERTY(SummonList, summonList)
  DECLARE_PROPERTY(LootItems, lootItems)
  DECLARE_PROPERTY(ElementMap, elementMap)

  DECLARE_PROPERTY(SpellList, spellAttackList)
  DECLARE_PROPERTY(SpellList, spellDefenseList)

  DECLARE_PROPERTY(uint32_t, yellChance)
  DECLARE_PROPERTY(uint32_t, yellSpeedTicks)
  DECLARE_PROPERTY(VoiceVector, voiceVector)
  DECLARE_PROPERTY(int32_t, changeTargetSpeed)
  DECLARE_PROPERTY(int32_t, changeTargetChance)

  #undef DECLARE_PROPERTY
private:
  void self_copy();
  InternalCreatureType* impl;
};

#endif
