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

#ifndef __OTSERV_PLAYER_H__
#define __OTSERV_PLAYER_H__

#include "classes.h"
#include "otsystem.h"
#include "creature.h"
#include "cylinder.h"
#include "vocation.h"
#include "protocolgame.h"

enum skillsid_t {
  SKILL_LEVEL=0,
  SKILL_TRIES=1,
  SKILL_PERCENT=2
};

enum playerinfo_t {
  PLAYERINFO_LEVEL,
  PLAYERINFO_LEVELPERCENT,
  PLAYERINFO_HEALTH,
  PLAYERINFO_MAXHEALTH,
  PLAYERINFO_MANA,
  PLAYERINFO_MAXMANA,
  PLAYERINFO_MAGICLEVEL,
  PLAYERINFO_MAGICLEVELPERCENT,
  PLAYERINFO_SOUL,
  PLAYERINFO_MAXSOUL
};

struct ShopItem{
  ShopItem(uint16_t itemId, int32_t subType, uint32_t buyPrice, uint32_t sellPrice) :
    itemId(itemId), subType(subType), buyPrice(buyPrice), sellPrice(sellPrice) {}
  ShopItem() :
    itemId(0), subType(-1), buyPrice(0), sellPrice(0) {}

  uint16_t itemId;
  int32_t subType;
  uint32_t buyPrice;
  uint32_t sellPrice;
};

typedef std::list<ShopItem> ShopItemList;
typedef std::vector< std::pair<uint32_t, Container*> > ContainerVector;
typedef std::map<uint32_t, Depot*> DepotMap;
typedef std::set<uint32_t> VIPListSet;
typedef std::map<uint32_t, uint32_t> MuteCountMap;
typedef std::map<uint32_t, std::string> ChannelStatementMap;
typedef std::list<std::string> LearnedInstantSpellList;
typedef std::list<Party*> PartyList;

#define PLAYER_MAX_SPEED 1500
#define PLAYER_MIN_SPEED 10
const int32_t MAX_STAMINA = 42 * 60 * 60 * 1000;
const int32_t MAX_STAMINA_MINUTES = MAX_STAMINA / 60000;

class Player : public Creature, public Cylinder
{
public:
#ifdef __ENABLE_SERVER_DIAGNOSTIC__
  static uint32_t playerCount;
#endif

  Player(const std::string& name, ProtocolGame* p);
  virtual ~Player();

  virtual Player* getPlayer() {return this;}
  virtual const Player* getPlayer() const {return this;}

  static MuteCountMap muteCountMap;
  static int32_t maxMessageBuffer;
  static ChannelStatementMap channelStatementMap;
  static uint32_t channelStatementGuid;

  virtual const std::string& getName() const {return name;}
  virtual const std::string& getNameDescription() const {return name;}
  virtual std::string getDescription(int32_t lookDistance) const;

  void setGUID(uint32_t _guid) {guid = _guid;}
  uint32_t getGUID() const { return guid;}
  virtual uint32_t idRange(){ return 0x10000000;}
  static AutoList<Player> listPlayer;
  void removeList();
  void addList();
  void kickPlayer();

  static uint64_t getExpForLevel(int32_t level)
  {
    level--;
    return ((50ULL * level * level * level) - (150ULL * level * level) + (400ULL * level))/3ULL;
  }

  uint32_t getGuildId() const {return guildId;}
  const std::string& getGuildName() const {return guildName;}
  const std::string& getGuildRank() const {return guildRank;}
  const std::string& getGuildNick() const {return guildNick;}

  void setGuildRank(const std::string& rank) {guildRank = rank;}
  void setGuildNick(const std::string& nick) {guildNick = nick;}

  void setFlags(uint64_t flags){ groupFlags = flags;}
  bool hasFlag(PlayerFlags flag) const {
    uint64_t v = (uint64_t(1) << flag.value());
    return (groupFlags & v) != 0;
  }

  uint16_t getPremiumDays() const {return premiumDays;}
  bool isPremium() const {return (premiumDays > 0 || hasFlag(PlayerFlag_IsAlwaysPremium));}

  bool isOffline() const {return (getID() == 0);}
  void disconnect() {if(client) client->disconnect();}
  uint32_t getIP() const;

  void addContainer(uint32_t cid, Container* container);
  void closeContainer(uint32_t cid);
  int32_t getContainerID(const Container* container) const;
  Container* getContainer(uint32_t cid);
  bool canOpenCorpse(uint32_t ownerId);

  void setBalance(uint32_t amount) {setCustomValue("__balance", amount);}
  uint32_t getBalance() const {uint32_t amount = 0; if(getCustomValue("__balance", amount)) {return amount;} return 0;}
  bool withdrawMoney(uint32_t amount);
  bool depositMoney(uint32_t amount);
  bool transferMoneyTo(const std::string& name, uint32_t amount);

  std::string getAccountName() const {return accountName;}
  uint32_t getAccountId() const {return accountId;}
  uint32_t getLevel() const {return level;}
  uint32_t getMagicLevel() const {return getPlayerInfo(PLAYERINFO_MAGICLEVEL);}
  int16_t getAccessLevel() const {return accessLevel;}
  int16_t getViolationLevel() const {return violationLevel;}
  std::string getGroupName() const {return groupName;}
  const std::string& getAccessGroup() const {return groupName;}

  void setVocation(uint32_t vocid);
  void setVocation(Vocation* voc);
  uint32_t getVocationId() const;
  Vocation* getVocation() const {return vocation;}

  PlayerSex getSex() const {return sex;}
  bool isMale() const {return getSex().value() % 2 != 0;}
  bool isFemale() const {return getSex().value() % 2 == 0;}

  void setSex(PlayerSex);
  int32_t getPlayerInfo(playerinfo_t playerinfo) const;
  int64_t getExperience() const {return experience;}
  void addExperience(uint64_t exp);

  time_t getLastLoginSaved() const {return lastLoginSaved;}
  const Position& getLoginPosition() const {return loginPosition;}
  const Position& getTemplePosition() const {return masterPos;}
  uint32_t getTown() const {return town;}
  void setTown(uint32_t _town) {town = _town;}

  bool isLoginAttackLocked(uint32_t attackerId) const;

  virtual bool isPushable() const;
  virtual int getThrowRange() const {return 1;}
  virtual bool canSeeInvisibility() const;
  uint32_t getMuteTime();
  void addMessageBuffer();
  void removeMessageBuffer();

  double getCapacity() const {
    if(hasFlag(PlayerFlag_CannotPickupItem)){
      return 0.00;
    }
    else if(hasFlag(PlayerFlag_HasInfiniteCapacity)){
      return 10000.00;
    }
    else{
      return capacity;
    }
  }

  double getFreeCapacity() const {
    if(hasFlag(PlayerFlag_CannotPickupItem)){
      return 0.00;
    }
    else if(hasFlag(PlayerFlag_HasInfiniteCapacity)){
      return 10000.00;
    }
    else{
      return std::max(0.00, capacity - inventoryWeight);
    }
  }

  virtual int32_t getMaxHealth() const {return getPlayerInfo(PLAYERINFO_MAXHEALTH);}
  virtual int32_t getMaxMana() const {return getPlayerInfo(PLAYERINFO_MAXMANA);}
  virtual int32_t getMaxSoul() const {return getPlayerInfo(PLAYERINFO_MAXSOUL);}
  virtual int32_t getFood() const {
    Condition* condition = getCondition(CONDITION_REGENERATION);
    return (condition ? condition->getTicks() / 1000 : 0);
  }

  // Reurns the inventory item in the slot position
  Item* getInventoryItem(SlotType slot) const;
  // As above, but returns NULL if the item can not be weared in that slot (armor in hand for example)
  Item* getEquippedItem(SlotType slot) const;

  int32_t getVarSkill(SkillType skill) const {return varSkills[skill.value()];}
  void setVarSkill(SkillType skill, int32_t modifier) {varSkills[skill.value()] += modifier;}

  int32_t getVarStats(PlayerStatType stat) const {return varStats[stat.value()];}
  void setVarStats(PlayerStatType stat, int32_t modifier);
  int32_t getDefaultStats(PlayerStatType stat);

  double getRateValue(LevelType rateType) const {return rateValue[rateType.value()];}
  void setRateValue(LevelType rateType, double value){rateValue[rateType.value()] = value;}

  uint32_t getLossPercent(LossType lossType) const {return lossPercent[lossType.value()];}
  void setLossPercent(LossType lossType, uint32_t newPercent)
  {
    lossPercent[lossType.value()] = newPercent;
  }

  Depot* getDepot(uint32_t depotId, bool autoCreateDepot);
  bool addDepot(Depot* depot, uint32_t depotId);

  virtual bool canSee(const Position& pos) const;
  virtual bool canSeeCreature(const Creature* creature) const;
  virtual bool canWalkthrough(const Creature* creature) const;

  virtual RaceType getRace() const {return RACE_BLOOD;}

  //safe-trade functions
  void setTradeState(TradeState state) {tradeState = state;}
  TradeState getTradeState() const {return tradeState;}
  Item* getTradeItem() {return tradeItem;}

  //V.I.P. functions
  void notifyLogIn(Player* player);
  void notifyLogOut(Player* player);
  bool removeVIP(uint32_t guid);
  bool addVIP(uint32_t guid, std::string& name, bool isOnline, bool interal = false);

  //follow functions
  virtual bool setFollowCreature(Creature* creature, bool fullPathSearch = false);

  //follow events
  virtual void onFollowCreature(const Creature* creature);

  //walk events
  virtual void onWalk(Direction& dir);
  virtual void onWalkAborted();
  virtual void onWalkComplete();

  void checkIdleTime(uint32_t ticks);
  void resetIdle() {idleTime = 0; idleWarned = false;}
  void setIdleTime(uint32_t value, bool warned){idleTime = value; idleWarned = warned;}

  void setChaseMode(ChaseMode mode);
  void setFightMode(FightMode mode);
  void setSafeMode(bool _safeMode) {safeMode = _safeMode;}
  bool hasSafeMode() const {return safeMode;}
  IconType getIcons() const;

  //combat functions
  virtual bool setAttackedCreature(Creature* creature);
  bool isImmune(CombatType type) const;
  bool isImmune(MechanicType type) const;
  bool isCured(Condition* condition) const;
  bool hasShield() const;
  virtual bool isAttackable() const;

  virtual void changeHealth(int32_t healthChange);
  virtual void changeMana(int32_t manaChange);
  void changeSoul(int32_t soulChange);

  bool isPzLocked() const {return pzLocked; }
  bool isAutoWalking() const {return eventWalk != 0;}
  virtual BlockType blockHit(CombatType combatType, const CombatSource& combatSource, int32_t& damage,
    bool checkDefense = false, bool checkArmor = false);
  virtual void doAttacking(uint32_t interval);
  virtual bool hasExtraSwing() {return lastAttack > 0 && ((OTSYS_TIME() - lastAttack) >= getAttackSpeed());}

  int32_t getSkill(SkillType skilltype, skillsid_t skillinfo) const;
  bool getAddAttackSkill() const {return addAttackSkillPoint;}
  BlockType getLastAttackBlockType() const {return lastAttackBlockType;}

  Item* getWeapon(bool ignoreAmmu = false);
  virtual WeaponType getWeaponType();
  int32_t getWeaponSkill(const Item* item) const;
  void getShieldAndWeapon(const Item* &shield, const Item* &weapon) const;

  virtual void drainHealth(CombatType combatType, const CombatSource& combatSource, int32_t damage, bool showtext = true);
  virtual void drainMana(const CombatSource& combatSource, int32_t point, bool showtext = true);
  void addManaSpent(uint32_t amount, bool useMultiplier = true);
  void addSkillAdvance(SkillType skill, uint32_t count, bool useMultiplier = true);

  virtual int32_t getArmor() const;
  virtual int32_t getDefense() const;
  virtual double getAttackFactor() const;
  virtual double getDefenseFactor() const;

  void addCombatExhaust(uint32_t ticks);
  void addHealExhaust(uint32_t ticks);
  void addInFightTicks(uint32_t ticks, bool pzlock = false);
  void addDefaultRegeneration(uint32_t addTicks);

  virtual uint64_t getGainedExperience(Creature* attacker) const;
  void getGainExperience(uint64_t& gainExp, bool fromMonster);

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
  virtual void onKilledCreature(Creature* target);
  virtual void onGainExperience(uint64_t gainExp, bool fromMonster);
  virtual void onGainSharedExperience(uint64_t gainExp, bool fromMonster);
  virtual void onAttackedCreatureBlockHit(Creature* target, BlockType blockType);
  virtual void onBlockHit(BlockType blockType);
  virtual void onChangeZone(ZoneType zone);
  virtual void onAttackedCreatureChangeZone(ZoneType zone);
  virtual void onIdleStatus();
  virtual void onPlacedCreature();
  virtual void sendReLoginWindow();
  virtual void getCreatureLight(LightInfo& light) const;

  void setParty(Party* _party) {party = _party;}
  Party* getParty() const {return party;}
  PartyShieldType getPartyShield(const Player* player) const;
  bool isInviting(const Player* player) const;
  bool isPartner(const Player* player) const;
  void sendPlayerPartyIcons(Player* player);
  bool addPartyInvitation(Party* party);
  bool removePartyInvitation(Party* party);
  void clearPartyInvitations();

#ifdef __SKULLSYSTEM__
  SkullType getSkull() const;
  SkullType getSkullClient(const Player* player) const;
  bool hasAttacked(const Player* attacked) const;
  void addAttacked(const Player* attacked);
  void clearAttacked();
  void addUnjustifiedDead(const Player* attacked);
  void setSkull(SkullType newSkull) {skullType = newSkull;}
  void sendCreatureSkull(const Creature* creature) const
    {if(client) client->sendCreatureSkull(creature);}
  void checkSkullTicks(int32_t ticks);
#endif

  void checkRecentlyGainedExperience(uint32_t interval);
  bool canLogout();
  void broadcastLoot(Creature* creature, Container* corpse);
  bool checkPzBlockOnCombat(Player* targetPlayer);

  // Returns true if the outfit is valid according to our internal 'sent outfit list'
  bool canWearOutfit(const OutfitType& ot) const;

  //tile
  //send methods
  void sendAddTileItem(const Tile* tile, const Position& pos, const Item* item);
  void sendUpdateTileItem(const Tile* tile, const Position& pos, const Item* olditem, const Item* newitem);
  void sendRemoveTileItem(const Tile* tile, const Position& pos, uint32_t stackpos, const Item* item);
  void sendUpdateTile(const Tile* tile, const Position& pos);

  void sendCreatureAppear(const Creature* creature, const Position& pos);
  void sendCreatureDisappear(const Creature* creature, uint32_t stackpos, bool isLogout);
  void sendCreatureMove(const Creature* creature, const Tile* newTile, const Position& newPos,
    const Tile* oldTile, const Position& oldPos, uint32_t oldStackPos, bool teleport);

  void sendCreatureTurn(const Creature* creature);
  void sendCreatureSay(const Creature* creature, SpeakClass type, const std::string& text);
  void sendCreatureSquare(const Creature* creature, SquareColor color);
  void sendCreatureChangeOutfit(const Creature* creature, const OutfitType& outfit);
  void sendCreatureChangeVisible(const Creature* creature, bool visible);
  void sendCreatureLight(const Creature* creature);
  void sendCreatureShield(const Creature* creature);

  //container
  void sendAddContainerItem(const Container* container, const Item* item);
  void sendUpdateContainerItem(const Container* container, uint8_t slot, const Item* oldItem, const Item* newItem);
  void sendRemoveContainerItem(const Container* container, uint8_t slot, const Item* item);
  void sendContainer(uint32_t cid, const Container* container, bool hasParent)
    {if(client) client->sendContainer(cid, container, hasParent); }

  //inventory
  void sendAddInventoryItem(SlotType slot, const Item* item)
    {if(client) client->sendAddInventoryItem(slot, item);}
  void sendUpdateInventoryItem(SlotType slot, const Item* oldItem, const Item* newItem)
    {if(client) client->sendUpdateInventoryItem(slot, newItem);}
  void sendRemoveInventoryItem(SlotType slot, const Item* item)
    {if(client) client->sendRemoveInventoryItem(slot);}

  //event methods
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

  //virtual void onCreatureSay(const Creature* creature, SpeakClasses type, const std::string& text);
  //virtual void onCreatureTurn(const Creature* creature);
  //virtual void onCreatureChangeOutfit(const Creature* creature, const OutfitType& outfit);

  //container
  void onAddContainerItem(const Container* container, const Item* item);
  void onUpdateContainerItem(const Container* container, uint8_t slot,
    const Item* oldItem, const ItemType& oldType, const Item* newItem, const ItemType& newType);
  void onRemoveContainerItem(const Container* container, uint8_t slot, const Item* item);

  void onCloseContainer(const Container* container);
  void onSendContainer(const Container* container);
  void autoCloseContainers(const Container* container);

  //inventory
  void onAddInventoryItem(SlotType slot, Item* item);
  void onUpdateInventoryItem(SlotType slot, Item* oldItem, const ItemType& oldType,
    Item* newItem, const ItemType& newType);
  void onRemoveInventoryItem(SlotType slot, Item* item);

  //other send messages
  void sendAnimatedText(const Position& pos, unsigned char color, std::string text) const
    {if(client) client->sendAnimatedText(pos,color,text);}
  void sendCancel(const std::string& msg) const
    {if(client) client->sendCancel(msg);}
  void sendCancelMessage(ReturnValue message) const;
  void sendCancelTarget() const
    {if(client) client->sendCancelTarget();}
  void sendCancelWalk() const
    {if(client) client->sendCancelWalk();}
  void sendChangeSpeed(const Creature* creature, uint32_t newSpeed) const
    {if(client) client->sendChangeSpeed(creature, newSpeed);}
  void sendCreatureHealth(const Creature* creature) const
    {if(client) client->sendCreatureHealth(creature);}
  void sendDistanceShoot(const Position& from, const Position& to, unsigned char type) const
    {if(client) client->sendDistanceShoot(from, to, type);}
  void sendHouseWindow(House* house, uint32_t listId) const;
  void sendOutfitWindow(const std::list<Outfit>& outfitList) const;
  void sendCreatePrivateChannel(uint16_t channelId, const std::string& channelName)
    {if(client) client->sendCreatePrivateChannel(channelId, channelName);}
  void sendClosePrivate(uint16_t channelId) const
    {if(client) client->sendClosePrivate(channelId);}
  void sendIcons() const;
  void sendMagicEffect(const Position& pos, unsigned char type) const
    {if(client) client->sendMagicEffect(pos,type);}
  void sendStats();
  void sendSkills() const
    {if(client) client->sendSkills();}
  void sendTextMessage(MessageClass mclass, const std::string& message) const
    {if(client) client->sendTextMessage(mclass, message);}
  void sendTextWindow(Item* item, uint16_t maxlen, bool canWrite) const
    {if(client) client->sendTextWindow(windowTextId, item, maxlen, canWrite);}
  void sendTextWindow(uint32_t itemId, const std::string& text) const
    {if(client) client->sendTextWindow(windowTextId, itemId, text);}
  void sendToChannel(Creature* creature, SpeakClass type, const std::string& text, uint16_t channelId, uint32_t time = 0) const
    {if(client) client->sendToChannel(creature, type, text, channelId, time);}
  
  void sendShopWindow(const ShopItemList& list);
  void sendShopSaleList(const ShopItemList& list) const
    {if(client) client->sendShopSaleList(list);}
  void sendShopClose() const
    {if(client) client->sendShopClose();}
  void sendTradeItemRequest(const Player* player, const Item* item, bool ack) const
    {if(client) client->sendTradeItemRequest(player, item, ack);}
  void sendTradeClose() const
    {if(client) client->sendCloseTrade();}

  void sendWorldLight(LightInfo& lightInfo)
    {if(client) client->sendWorldLight(lightInfo);}
  void sendChannelsDialog()
    {if(client) client->sendChannelsDialog();}
  void sendOpenPrivateChannel(const std::string& receiver)
    {if(client) client->sendOpenPrivateChannel(receiver);}
  void sendOutfitWindow(const OutfitList& outfitList);
  void sendCloseContainer(uint32_t cid)
    {if(client) client->sendCloseContainer(cid);}
  void sendChannel(uint16_t channelId, const std::string& channelName)
    {if(client) client->sendChannel(channelId, channelName);}
  void sendQuestLog()
    {if(client) client->sendQuestLog();}
  void sendQuestLine(const Quest* quest)
    {if(client) client->sendQuestLine(quest);}

  void sendTutorial(uint8_t tutorialId)
    {if(client) client->sendTutorial(tutorialId);}
  void sendAddMarker(const Position& pos, uint8_t markType, const std::string& desc)
    {if (client) client->sendAddMarker(pos, markType, desc);}

  void receivePing() {last_pong = OTSYS_TIME();}

  virtual void onThink(uint32_t interval);
  virtual void onAttacking(uint32_t interval);

  virtual void postAddNotification(Creature* actor, Thing* thing, const Cylinder* oldParent, int32_t index, cylinderlink_t link = LINK_OWNER);
  virtual void postRemoveNotification(Creature* actor, Thing* thing, const Cylinder* newParent, int32_t index, bool isCompleteRemoval, cylinderlink_t link = LINK_OWNER);

  Item* getWriteItem(uint32_t& _windowTextId, uint16_t& _maxWriteLen);
  void setWriteItem(Item* item, uint16_t _maxWriteLen = 0);

  House* getEditHouse(uint32_t& _windowTextId, uint32_t& _listId);
  void setEditHouse(House* house, uint32_t listId = 0);

  void setNextAction(int64_t time) {if(time > nextAction) {nextAction = time;}}
  bool canDoAction() const {return nextAction <= OTSYS_TIME();}
  uint32_t getNextActionTime() const;
  virtual uint32_t getAttackSpeed() const;

  void learnInstantSpell(const std::string& name);
  bool hasLearnedInstantSpell(const std::string& name) const;
  void stopWalk();

  // Shop window utility functions
  void updateSaleShopList(uint32_t itemId);
  bool hasShopItemForSale(uint32_t itemId, int32_t subType);

  VIPListSet VIPList;
  uint32_t maxVipLimit;

  //items
  ContainerVector containerVec;
  void preSave();
  bool hasCapacity(const Item* item, uint32_t count) const;

  //stamina
  void addStamina(int64_t value);
  void removeStamina(int64_t value) {addStamina(-value);}
  int32_t getStaminaMinutes();
  int32_t getStamina() const {return stamina;}
  int32_t getSpentStamina() const {return MAX_STAMINA - stamina;}

  //depots
  DepotMap depots;
  uint32_t maxDepotLimit;

  //cylinder implementations
  virtual Cylinder* getParent() {return Creature::getParent();}
  virtual const Cylinder* getParent() const {return Creature::getParent();}
  virtual bool isRemoved() const {return Creature::isRemoved();}
  virtual Position getPosition() const {return Creature::getPosition();}
  virtual Tile* getTile() {return NULL;}
  virtual const Tile* getTile() const {return NULL;}
  virtual Item* getItem() {return NULL;}
  virtual const Item* getItem() const {return NULL;}
  virtual Creature* getCreature() {return this;}
  virtual const Creature* getCreature() const {return this;}
  virtual Tile* getParentTile() {return Creature::getParentTile();}
  virtual const Tile* getParentTile() const {return Creature::getParentTile();}

  virtual uint32_t __getItemTypeCount(uint16_t itemId, int32_t subType = -1) const;
  virtual std::map<uint32_t, uint32_t>& __getAllItemTypeCount(std::map<uint32_t, uint32_t>& countMap) const;

protected:
  void checkTradeState(const Item* item);

  std::string getSkillName(int skillid);
  void gainExperience(uint64_t& gainExp, bool fromMonster);
  void removeExperience(uint64_t exp, bool updateStats = true);

  void updateInventoryWeight();

  void setNextWalkActionTask(SchedulerTask* task);
  void setNextWalkTask(SchedulerTask* task);
  void setNextActionTask(SchedulerTask* task);

  void onDie();
  void die();
  virtual Item* dropCorpse();
  virtual Item* createCorpse();

  //cylinder implementations
  virtual ReturnValue __queryAdd(int32_t index, const Thing* thing, uint32_t count,
    uint32_t flags) const;
  virtual ReturnValue __queryMaxCount(int32_t index, const Thing* thing, uint32_t count, uint32_t& maxQueryCount,
    uint32_t flags) const;
  virtual ReturnValue __queryRemove(const Thing* thing, uint32_t count, uint32_t flags) const;
  virtual Cylinder* __queryDestination(int32_t& index, const Thing* thing, Item** destItem,
    uint32_t& flags);

  virtual void __addThing(Creature* actor, Thing* thing);
  virtual void __addThing(Creature* actor, int32_t index, Thing* thing);
  virtual void __updateThing(Creature* actor, Thing* thing, uint16_t itemId, uint32_t count);
  virtual void __replaceThing(Creature* actor, uint32_t index, Thing* thing);
  virtual void __removeThing(Creature* actor, Thing* thing, uint32_t count);

  virtual int32_t __getIndexOfThing(const Thing* thing) const;
  virtual int32_t __getFirstIndex() const;
  virtual int32_t __getLastIndex() const;
  virtual Thing* __getThing(uint32_t index) const;

  virtual void __internalAddThing(Thing* thing);
  virtual void __internalAddThing(uint32_t index, Thing* thing);

protected:
  ProtocolGame* client;

  uint32_t level;
  uint32_t levelPercent;
  uint32_t magLevel;
  uint32_t magLevelPercent;
  int16_t accessLevel;
  int16_t violationLevel;
  std::string groupName;
  uint64_t experience;
  CombatType damageImmunities;
  MechanicType mechanicImmunities;
  MechanicType mechanicSuppressions;
  uint32_t condition;
  int32_t stamina;
  uint32_t manaSpent;
  Vocation* vocation;
  PlayerSex sex;
  int32_t soul, soulMax;
  uint64_t groupFlags;
  uint16_t premiumDays;
  uint32_t MessageBufferTicks;
  int32_t MessageBufferCount;
  uint32_t actionTaskEvent;
  uint32_t nextStepEvent;
  uint32_t walkTaskEvent;
  SchedulerTask* walkTask;

  int32_t idleTime;
  bool idleWarned;

  double inventoryWeight;
  double capacity;

  int64_t last_ping;
  int64_t last_pong;
  int64_t nextAction;

  bool pzLocked;
  bool isConnecting;
  int32_t bloodHitCount;
  int32_t shieldBlockCount;
  BlockType lastAttackBlockType;
  bool addAttackSkillPoint;
  uint64_t lastAttack;

  ChaseMode chaseMode;
  FightMode fightMode;
  bool safeMode;

  // This list is set to the same list as sent by sendOutfitWindow
  // and used when setting the outfit to make sure the client doesn't
  // fool us
  OutfitList validOutfitList;

  ShopItemList validShopList;

  //account variables
  uint32_t accountId;
  std::string accountName;
  std::string password;
  time_t lastLoginSaved;
  time_t lastLogout;
  int64_t lastLoginMs;
  Position loginPosition;
  uint32_t lastip;

  //inventory variables
  Item* inventory[11];
  bool inventoryAbilities[11];

  //player advances variables
  uint32_t skills[SkillType::size][3];

  //extra skill modifiers
  int32_t varSkills[SkillType::size];

  //extra stat modifiers
  int32_t varStats[PlayerStatType::size];

  //loss percent variables
  uint32_t lossPercent[LossType::size];

  //rate value variables
  double rateValue[LevelType::size];

  LearnedInstantSpellList learnedInstantSpellList;

  ConditionList storedConditionList;

  //trade variables
  Player* tradePlayer;
  TradeState tradeState;
  Item* tradeItem;

  //party variables
  Party* party;
  PartyList invitePartyList;

  std::string name;
  std::string nameDescription;
  uint32_t guid;
  uint32_t town;

  //guild variables
  uint32_t guildId;
  std::string guildName;
  std::string guildRank;
  std::string guildNick;
  uint32_t guildLevel;

  LightInfo itemsLight;

  //read/write storage data
  uint32_t windowTextId;
  Item* writeItem;
  uint16_t maxWriteLen;
  House* editHouse;
  uint32_t editListId;

#ifdef __SKULLSYSTEM__
  SkullType skullType;
  int64_t lastSkullTime;
  typedef std::set<uint32_t> AttackedSet;
  AttackedSet attackedSet;
#endif

  void updateItemsLight(bool internal = false);
  virtual int32_t getStepSpeed() const;
  void updateBaseSpeed();

  static uint32_t getPercentLevel(uint64_t count, uint32_t nextLevelCount);
  virtual uint64_t getLostExperience() const;

  virtual void dropLoot(Container* corpse);
  virtual CombatType getDamageImmunities() const { return damageImmunities; }
  virtual MechanicType getMechanicImmunities() const {return mechanicImmunities;}
  virtual uint16_t getCorpseId() const;
  virtual void getPathSearchParams(const Creature* creature, FindPathParams& fpp) const;

  friend class Game;
  friend class Commands;
  friend class Map;
  friend class IOPlayer;
  friend class ProtocolGame;
  friend class LuaState;
};

#endif

