
// OpenTibia - an opensource roleplaying game
//////////////////////////////////////////////////////////////////////
// base class for every creature
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

#ifndef __OTSERV_CREATURE_H__
#define __OTSERV_CREATURE_H__

#include <list>
#include <map>
#include "templates.h"
#include "thing.h"
#include "condition_attributes.h"
#include "const.h"
#include "protocolconst.h"
#include "outfit.h"
#include "condition.h"

typedef std::list<Condition*> ConditionList;
typedef std::map<std::string, std::string> StorageMap;

struct FindPathParams{
  bool fullPathSearch;
  bool clearSight;
  bool allowDiagonal;
  bool keepDistance;
  int32_t maxSearchDist;
  int32_t minTargetDist;
  int32_t maxTargetDist;

  FindPathParams()
  {
    clearSight = true;
    fullPathSearch = true;
    allowDiagonal = true;
    keepDistance = false;
    maxSearchDist = -1;
    minTargetDist = -1;
    maxTargetDist = -1;
  }
};

// Used for death entries
struct DeathLessThan;
struct DeathEntry{
  // Death can be either a name (for fields) or a creature (for anything substantial)
  // Fields are only counted if they are the final hit killer
  DeathEntry(std::string name, int32_t dmg) : data(name), damage(dmg), isUnjust(false) {}
  DeathEntry(Creature* killer, int32_t dmg, bool unjust) : data(killer), damage(dmg), isUnjust(unjust) {}

  bool isCreatureKill() const {return data.type() == typeid(Creature*);}
  bool isNameKill() const {return !isCreatureKill();}
  bool isUnjustKill() const {return isUnjust;}

  Creature* getKillerCreature() const {return boost::any_cast<Creature*>(data);}
  std::string getKillerName() const {return boost::any_cast<std::string>(data);}

protected:
  boost::any data;
  int32_t damage;
  bool isUnjust;

  friend struct DeathLessThan;
};

struct DeathLessThan{
  bool operator()(const DeathEntry& d1, const DeathEntry& d2) const
  {
    // Sort descending
    return d1.damage > d2.damage;
  }
};

typedef std::vector<DeathEntry> DeathList;

namespace Script {
  class Listener;
  typedef boost::shared_ptr<Listener> Listener_ptr;
  typedef boost::weak_ptr<Listener> Listener_wptr;
  typedef std::vector<Listener_ptr> ListenerList;
}

class FrozenPathingConditionCall {
public:
  FrozenPathingConditionCall(const Position& _targetPos);
  virtual ~FrozenPathingConditionCall() {}

  virtual bool operator()(const Position& startPos, const Position& testPos,
    const FindPathParams& fpp, int32_t& bestMatchDist) const;

  bool isInRange(const Position& startPos, const Position& testPos,
    const FindPathParams& fpp) const;

protected:
  Position targetPos;
};

//////////////////////////////////////////////////////////////////////
// Defines the Base class for all creatures and base functions which
// every creature has

class Creature : public AutoID, virtual public Thing
{
protected:
  Creature();
public:
  virtual ~Creature();

  virtual Creature* getCreature();
  virtual const Creature* getCreature() const;
  virtual Player* getPlayer();
  virtual const Player* getPlayer() const;
  virtual Actor* getActor();
  virtual const Actor* getActor() const;

  void getPathToFollowCreature();

  virtual const std::string& getName() const = 0;
  virtual const std::string& getNameDescription() const = 0;
  virtual std::string getDescription(int32_t lookDistance) const;

  void setID();
  void setRemoved();

  virtual uint32_t idRange() = 0;
  uint32_t getID() const;
  virtual void removeList() = 0;
  virtual void addList() = 0;
  virtual void onRemoved();

  virtual bool canSee(const Position& pos) const;
  virtual bool canSeeCreature(const Creature* creature) const;
  virtual bool canWalkthrough(const Creature* creature) const;

  virtual RaceType getRace() const;
  Direction getDirection() const;
  void setDirection(Direction dir);

  const Position& getMasterPos() const;
  virtual void setMasterPos(const Position& pos, uint32_t radius = 1);

  virtual int getThrowRange() const;
  virtual bool isPushable() const;
  virtual bool isRemoved() const;
  virtual bool canSeeInvisibility() const;

  int32_t getWalkDelay(Direction dir) const;
  int32_t getWalkDelay() const;
  int64_t getTimeSinceLastMove() const;

  int64_t getEventStepTicks() const;
  int32_t getStepDuration(Direction dir) const;
  int32_t getStepDuration() const;
  virtual int32_t getStepSpeed() const;
  int32_t getSpeed() const;
  void setSpeed(int32_t varSpeedDelta);

  void setBaseSpeed(uint32_t newBaseSpeed);
  int getBaseSpeed() const;

  virtual int32_t getHealth() const;
  virtual int32_t getMaxHealth() const;
  virtual int32_t getMana() const;
  virtual int32_t getMaxMana() const;

  const OutfitType getCurrentOutfit() const;
  void setCurrentOutfit(OutfitType outfit);
  const OutfitType getDefaultOutfit() const;
  bool isInvisible() const;
  ZoneType getZone() const;

  //walk functions
  bool startAutoWalk(std::list<Direction>& listDir);
  void addEventWalk();
  void stopEventWalk();

  //walk events
  virtual void onWalk(Direction& dir);
  virtual void onWalkAborted();
  virtual void onWalkComplete();

  //follow functions
  virtual Creature* getFollowCreature() const;
  virtual bool setFollowCreature(Creature* creature, bool fullPathSearch = false);

  //follow events
  virtual void onFollowCreature(const Creature* creature);
  virtual void onFollowCreatureComplete(const Creature* creature);

  //combat functions
  Creature* getAttackedCreature();
  virtual bool setAttackedCreature(Creature* creature);
  virtual BlockType blockHit(CombatType combatType, const CombatSource& combatSource, int32_t& damage,
    bool checkDefense = false, bool checkArmor = false);

  void setMaster(Creature* creature);
  Creature* getMaster();
  bool isSummon() const;
  bool isPlayerSummon() const;
  Player* getPlayerMaster() const;
  const Creature* getMaster() const;

  virtual void addSummon(Creature* creature);
  virtual void removeSummon(const Creature* creature);
  void destroySummons();
  const std::list<Creature*>& getSummons() const;

  virtual int32_t getArmor() const;
  virtual int32_t getDefense() const;
  virtual double getAttackFactor() const;
  virtual double getDefenseFactor() const;

  bool addCondition(Condition* condition);
  void removeCondition(Condition* condition);
  void removeCondition(const std::string& name);
  void removeCondition(ConditionId id);
  void removeCondition(const std::string& name, uint32_t sourceId);
  void removeCondition(const std::string& name, const CombatSource& combatSource);
  void removeCondition(CombatType type);
  void removeCondition(MechanicType type);

  Condition* getCondition(const std::string& name) const;
  Condition* getCondition(ConditionId id) const;
  Condition* getCondition(const std::string& name, uint32_t sourceId) const;

  bool hasCondition(const std::string& name) const;
  bool hasCondition(ConditionId id) const;
  bool hasCondition(CombatType type) const;
  bool hasCondition(MechanicType type) const;

  void executeConditions(uint32_t interval);

  virtual bool isImmune(const Condition* condition) const;
  virtual bool isImmune(MechanicType type) const;
  virtual bool isImmune(CombatType type) const;
  virtual bool isCured(Condition* condition) const;
  virtual MechanicType getMechanicImmunities() const;
  virtual CombatType getDamageImmunities() const;
  virtual bool isAttackable() const;
  virtual void changeHealth(int32_t healthChange);
  virtual void changeMana(int32_t manaChange);

  virtual void gainHealth(const CombatSource& combatSource, int32_t healthGain);
  virtual void drainHealth(CombatType combatType, const CombatSource& combatSource, int32_t damage, bool showtext);
  virtual void drainMana(const CombatSource& combatSource, int32_t manaLoss, bool showtext);

  virtual bool challengeCreature(Creature* creature);
  virtual bool convinceCreature(Creature* creature);

  virtual void onDie();
  virtual void die();

  virtual uint64_t getGainedExperience(Creature* attacker) const;
  void addDamagePoints(Creature* attacker, int32_t damagePoints);
  void addHealPoints(Creature* caster, int32_t healthPoints);
  bool hasBeenAttacked(uint32_t attackerId) const;

  //combat event functions
  virtual void onAddCondition(const Condition* condition, bool preAdd = true);
  virtual void onEndCondition(const Condition* condition, bool preEnd = true);
  virtual void onCombatRemoveCondition(const CombatSource& combatSource, Condition* condition);
  virtual void onAttackedCreature(Creature* target);
  virtual void onSummonAttackedCreature(Creature* summon, Creature* target);
  virtual void onAttacked();
  virtual void onAttackedCreatureDrainHealth(Creature* target, int32_t points);
  virtual void onSummonAttackedCreatureDrainHealth(Creature* summon, Creature* target, int32_t points);
  virtual void onAttackedCreatureDrainMana(Creature* target, int32_t points);
  virtual void onSummonAttackedCreatureDrainMana(Creature* summon, Creature* target, int32_t points);
  virtual void onTargetCreatureGainHealth(Creature* target, int32_t points);
  virtual void onAttackedCreatureKilled(Creature* target);
  virtual void onKilledCreature(Creature* target);
  virtual void onGainExperience(uint64_t gainExp, bool fromMonster);
  virtual void onGainSharedExperience(uint64_t gainExp, bool fromMonster);
  virtual void onAttackedCreatureBlockHit(Creature* target, BlockType blockType);
  virtual void onBlockHit(BlockType blockType);
  virtual void onChangeZone(ZoneType zone);
  virtual void onAttackedCreatureChangeZone(ZoneType zone);
  virtual void onIdleStatus();

  virtual void getCreatureLight(LightInfo& light) const;
  virtual void setNormalCreatureLight();
  void setCreatureLight(const LightInfo& light);

  virtual void onThink(uint32_t interval);
  virtual void onAttacking(uint32_t interval);
  virtual void onWalk();
  virtual bool getNextStep(Direction& dir, uint32_t& flags);

  virtual void onAddTileItem(const Tile* tile, const Position& pos, const Item* item);
  virtual void onUpdateTileItem(const Tile* tile, const Position& pos,
    const Item* oldItem, const ItemType& oldType, const Item* newItem, const ItemType& newType);
  virtual void onRemoveTileItem(const Tile* tile, const Position& pos,
    const ItemType& iType, const Item* item);
  virtual void onUpdateTile(const Tile* tile, const Position& pos);

  virtual void onCreatureAppear(const Creature* creature, bool isLogin);
  virtual void onCreatureDisappear(const Creature* creature, bool isLogout);
  virtual void onCreatureMove(const Creature* creature, const Tile* newTile, const Position& newPos,
    const Tile* oldTile, const Position& oldPos, bool teleport);

  virtual void onAttackedCreatureDissapear(bool isLogout);
  virtual void onFollowCreatureDissapear(bool isLogout);

  virtual void onCreatureTurn(const Creature* creature);
  virtual void onCreatureSay(const Creature* creature, SpeakClass type, const std::string& text);

  virtual void onCreatureChangeOutfit(const Creature* creature, const OutfitType& outfit);
  virtual void onCreatureConvinced(const Creature* convincer, const Creature* creature);
  virtual void onCreatureChangeVisible(const Creature* creature, bool visible);
  virtual void onPlacedCreature();
  virtual void onRemovedCreature();

  virtual WeaponType getWeaponType();

  size_t getSummonCount() const;
  void setDropLoot(bool _lootDrop);
  void setLossSkill(bool _skillLoss);

  virtual void setParent(Cylinder* cylinder);

  virtual Position getPosition() const;
  virtual Tile* getParentTile();
  virtual const Tile* getParentTile() const;
  int32_t getWalkCache(const Position& pos) const;

  static bool canSee(const Position& myPos, const Position& pos, int32_t viewRangeX, int32_t viewRangeY);

  void addListener(Script::Listener_ptr listener);
  Script::ListenerList getListeners(Script::ListenerType type);
  void clearListeners();

  // Custom value interface
  void setCustomValue(const std::string& key, const std::string& value);
  void setCustomValue(const std::string& key, int32_t value);
  bool eraseCustomValue(const std::string& key);
  bool getCustomValue(const std::string& key, std::string& value) const;
  bool getCustomValue(const std::string& key, uint32_t& value) const;
  bool getCustomValue(const std::string& key, int32_t& value) const;

  StorageMap::const_iterator getCustomValueIteratorBegin() const;
  StorageMap::const_iterator getCustomValueIteratorEnd() const;

protected:
  static const int32_t mapWalkWidth = Map_maxViewportX * 2 + 1;
  static const int32_t mapWalkHeight = Map_maxViewportY * 2 + 1;
  bool localMapCache[mapWalkHeight][mapWalkWidth];

  virtual bool useCacheMap() const;

  Tile* _tile;
  uint32_t id;
  bool isInternalRemoved;
  bool isMapLoaded;
  bool isUpdatingPath;
  // The creature onThink event vector this creature belongs to
  // -1 represents that the creature isn't in any vector
  int32_t checkCreatureVectorIndex;
  bool creatureCheck;

  Script::ListenerList registered_listeners;
  StorageMap storageMap;

  int32_t health, healthMax;
  int32_t mana, manaMax;

  OutfitType currentOutfit;
  OutfitType defaultOutfit;

  Position masterPos;
  int32_t masterRadius;
  uint64_t lastStep;
  uint32_t lastStepCost;
  uint32_t baseSpeed;
  int32_t varSpeed;
  bool skillLoss;
  bool lootDrop;
  Direction direction;
  ConditionList conditions;
  LightInfo internalLight;

  //summon variables
  Creature* master;
  std::list<Creature*> summons;

  //follow variables
  Creature* followCreature;
  uint32_t eventWalk;
  std::list<Direction> listWalkDir;
  uint32_t walkUpdateTicks;
  bool hasFollowPath;
  bool forceUpdateFollowPath;

  //combat variables
  Creature* attackedCreature;

  struct CountBlock_t{
    int32_t total;
    int64_t ticks;
    uint32_t hits;
  };

  typedef std::map<uint32_t, CountBlock_t> CountMap;
  CountMap damageMap;
  CountMap healMap;
  CombatType lastDamageSource;
  uint32_t lastHitCreature;
  uint32_t blockCount;
  uint32_t blockTicks;

  void updateMapCache();
#ifdef __DEBUG__
  void validateMapCache();
#endif
  void updateTileCache(const Tile* tile, int32_t dx, int32_t dy);
  void updateTileCache(const Tile* tile, const Position& pos);
  void internalCreatureDisappear(const Creature* creature, bool isLogout);
  virtual void doAttacking(uint32_t interval);
  virtual bool hasExtraSwing();

  virtual uint64_t getLostExperience() const;
  virtual double getDamageRatio(Creature* attacker) const;
  DeathList getKillers(int32_t assist_count = 1);
  virtual void dropLoot(Container* corpse);
  virtual uint16_t getCorpseId() const;
  virtual void getPathSearchParams(const Creature* creature, FindPathParams& fpp) const;
  virtual Item* dropCorpse();
  virtual Item* createCorpse();

  friend class Game;
  friend class Map;
};

#endif
