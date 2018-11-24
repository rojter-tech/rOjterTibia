//////////////////////////////////////////////////////////////////////
// OpenTibia - an opensource roleplaying game
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

#ifndef __OTSERV_SCRIPT_EVENT__
#define __OTSERV_SCRIPT_EVENT__

#include "classes.h"
#include "script_environment.h"
#include "script_listener.h"
#include "condition_attributes.h"
#include "account.h"
#include "chat.h"
#include "const.h"
#include "boost/any.hpp"
#include "boost_common.h"

// Forward declarations
class LuaState;
class LuaThread;
typedef boost::shared_ptr<LuaThread> LuaThread_ptr;

// These are actually defined in the .cpp file, so you CAN ONLY USE THEM IN script_event.cpp
template<class T, class ScriptInformation>
  bool dispatchEvent(T* e, Script::Manager& state, Script::Environment& environment, Script::ListenerList& specific_list);
template<class T>
  bool dispatchEvent(T* e, Script::Manager& state, Script::Environment& environment, Script::ListenerList& specific_list);

namespace Script {

  /****************** GUIDE READ THIS TO ADD AN EVENT! *****************&*/
  /* To add a new event
   * 1. Create the event class, with all it's members
   *    easiest is to copy an existing event that's similar.
   * 2. Add the listener type to enums.h
   * 3. Expose a registerListener function to lua (or many)
   * 4. Add the class to Environment::stopListener
   * 5. Add callback from an arbitrary location in otserv source
   */

  ///////////////////////////////////////////////////////////////////////////////
  // Event template

  class Event {
  public:
    Event();
    virtual ~Event();

    virtual std::string getName() const = 0;

    // Runs the event (ie. triggers all concerned listeners)
    virtual bool dispatch(Manager& state, Script::Environment& environment) = 0;

    // Lua stack manipulation, push
    virtual void push_instance(LuaState& state, Script::Environment& environment) = 0;
    // update, peek at top table and fill this event with values from it)
    virtual void update_instance(Manager& state, Script::Environment& environment, LuaThread_ptr thread) = 0;

    // Should not be called directly (calling functions
    // can't be made fiend due to compiler limitations)
    bool call(Manager& stae, Environment& environment, Listener_ptr listener);
  protected:

    int reference;
    bool propagate_by_default;

  };

  ////////////////////////////////////////////////////////////////
  // OnServerLoad event
  // Triggered when the server has finished loading (after map),
  // or when scripts are reloaded

  namespace OnServerLoad {
    class Event : public Script::Event {
    public:
      Event(bool real_startup);
      ~Event();

      std::string getName() const {return "OnServerLoad";}

      // Runs the event
      bool dispatch(Manager& state, Environment& environment);

      // Lua stack manipulation
      void push_instance(LuaState& state, Environment& environment);
      void update_instance(Manager& state, Script::Environment& environment, LuaThread_ptr thread);

    protected:
      bool real_startup;
    };
  }

  ////////////////////////////////////////////////////////////////
  // OnServerUnload event
  // Triggered when server is shutdown, or right before a /reload

  namespace OnServerUnload {
    class Event : public Script::Event {
    public:
      Event(bool real_shutdown);
      ~Event();

      std::string getName() const {return "OnServerUnload";}

      // Runs the event
      bool dispatch(Manager& state, Environment& environment);

      // Lua stack manipulation
      void push_instance(LuaState& state, Environment& environment);
      void update_instance(Manager& state, Script::Environment& environment, LuaThread_ptr thread);

    protected:
      bool real_shutdown;
    };
  }

  ///////////////////////////////////////////////////////////////////////////////
  // OnSay event
  // Triggered when a creature talks

  namespace OnSay {
    enum FilterType {
      FILTER_ALL,
      FILTER_SUBSTRING,
      FILTER_MATCH_BEGINNING,
      FILTER_EXACT,
    };

    struct ScriptInformation {
      std::string filter;
      FilterType method;
      bool case_sensitive;
    };

    class Event : public Script::Event {
    public:
      Event(Creature* speaker, SpeakClass& speak_class, ChatChannel* channel, std::string& text);
      ~Event();

      std::string getName() const {return "OnSay";}

      // Runs the event
      bool dispatch(Manager& state, Environment& environment);

      // This checks if the script information matches this events prerequiste (data members)
      bool check_match(const ScriptInformation& info);

      // Lua stack manipulation
      void push_instance(LuaState& state, Environment& environment);
      void update_instance(Manager& state, Script::Environment& environment, LuaThread_ptr thread);

    protected:
      Creature* speaker;
      SpeakClass& speak_class;
      ChatChannel* channel;
      std::string& text;
    };
  }


  ///////////////////////////////////////////////////////////////////////////////
  // OnHear event
  // Triggered when a creature hears another creature speak

  namespace OnHear {
    struct ScriptInformation {
    };

    class Event : public Script::Event {
    public:
      Event(Creature* creature, Creature* talking_creature, const std::string& message, const SpeakClass& speak_class);
      ~Event();

      std::string getName() const {return "OnHear";}

      // Runs the event
      bool dispatch(Manager& state, Environment& environment);

      // Lua stack manipulation
      void push_instance(LuaState& state, Environment& environment);
      void update_instance(Manager& state, Script::Environment& environment, LuaThread_ptr thread);

    protected:
      Creature* creature;
      Creature* talking_creature;
      const std::string& message;
      const SpeakClass& speak_class;
    };
  }

  ////////////////////////////////////////////////////////////////
  // OnUseItem event
  // Triggered when a player uses an item

  namespace OnUseItem {
    enum FilterType {
      FILTER_ITEMID,
      FILTER_ACTIONID,
    };

    struct ScriptInformation {
      FilterType method;
      int32_t id;
    };

    class Event : public Script::Event {
    public:
      Event(Player* user, Item* item, const PositionEx* toPos, Creature* targetCreature, Item* targetItem, ReturnValue& retval);
      Event(Player* user, Item* item, ReturnValue& retval);
      ~Event();

      std::string getName() const {return "OnUseItem";}

      // Runs the event
      bool dispatch(Manager& state, Environment& environment);

      // This checks if the script information matches this events prerequiste (data members)
      bool check_match(const ScriptInformation& info);

      // Lua stack manipulation
      void push_instance(LuaState& state, Environment& environment);
      void update_instance(Manager& state, Script::Environment& environment, LuaThread_ptr thread);

    protected:
      Player* user;
      Item* item;
      const PositionEx* targetPos;
      Creature* targetCreature;
      Item* targetItem;
      ReturnValue& retval;
    };
  }

  ///////////////////////////////////////////////////////////////////////////////
  // OnUseWeapon event
  // Triggered when a player is using weapons
  namespace OnUseWeapon {
    enum FilterType {
      FILTER_ALL, //
      FILTER_FIST, // no weapon
      FILTER_ITEMID, // specific weapon by item id
      FILTER_ACTIONID // specific weapon by action id
    };

    struct ScriptInformation {
      FilterType method;
      int32_t id; // either item or action id
    };

    class Event : public Script::Event {
    public:
      Event(Player* player, Creature* attacked, Item* weapon);
      ~Event();

      std::string getName() const {return "OnUseWeapon";}

      // Runs the event
      bool dispatch(Manager& state, Environment& environment);

      // This checks if the script information matches this events prerequisite
      bool check_match(const ScriptInformation& info);

      // Lua stack manipulation
      void push_instance(LuaState& state, Environment& environment);
      void update_instance(Manager& state, Script::Environment& environment, LuaThread_ptr thread);

    protected:
      Player *player;
      Creature *attacked;
      Item *weapon;
    };
  }

  ////////////////////////////////////////////////////////////////
  // OnEquipItem event
  // Triggered when a player equip an item

  namespace OnEquipItem {
    enum FilterType {
      FILTER_ITEMID,
      FILTER_ACTIONID,
    };

    struct ScriptInformation {
      FilterType method;
      int32_t id;
      SlotPosition slotPos;
      bool equip;
      bool postEvent;
    };

    class Event : public Script::Event {
    public:
      // For events that will trigger after the move is completed
      Event(Player* user, Item* item, SlotType slot, bool equip);

      // For events that will trigger before the move is completed
      Event(Player* user, Item* item, SlotType slot, bool equip, ReturnValue& retval);

      ~Event();

      std::string getName() const {return "OnEquipItem";}

      // Runs the event
      bool dispatch(Manager& state, Environment& environment);

      // This checks if the script information matches this events prerequiste (data members)
      bool check_match(const ScriptInformation& info);

      // Lua stack manipulation
      void push_instance(LuaState& state, Environment& environment);
      void update_instance(Manager& state, Script::Environment& environment, LuaThread_ptr thread);

    private:
      ReturnValue dummyRetVal;

    protected:
      Player* user;
      Item* item;
      SlotPosition slotPos;
      bool equip;
      bool postEvent;
      ReturnValue& retval;
    };
  }

  ////////////////////////////////////////////////////////////////
  // OnMoveCreature event
  // Triggered when a creature moves
  // If tied to item / action item, will only be triggered ONCE
  // if several identical items are on a single tile (ie. once per
  // item/action id, not once per item)

  namespace OnMoveCreature {
    enum FilterType {
      FILTER_ITEMID,
      FILTER_ACTIONID,
      FILTER_NONE,
    };

    enum MoveType {
      TYPE_NONE,
      TYPE_MOVE,
      TYPE_STEPIN,
      TYPE_STEPOUT,
    };

    struct ScriptInformation {
      FilterType method;
      int32_t id;
      uint32_t slot;
      MoveType moveType;
    };

    class Event : public Script::Event {
    public:
      Event(Creature* actor, Creature* moving_creature, Tile* fromTile, Tile* toTile);
      ~Event();

      std::string getName() const {return "OnMoveCreature";}

      // Runs the event
      bool dispatch(Manager& state, Environment& environment);

      // This checks if the script information matches this events prerequiste (data members)
      bool check_match(const ScriptInformation& info);

      // Lua stack manipulation
      void push_instance(LuaState& state, Environment& environment);
      void update_instance(Manager& state, Script::Environment& environment, LuaThread_ptr thread);

    protected:
      bool isMatch(const ScriptInformation& info, Tile* tile);

      Creature* actor;
      Creature* moving_creature;
      Tile* fromTile;
      Tile* toTile;
      Item* item;
      MoveType moveType;
    };
  }

  ////////////////////////////////////////////////////////////////
  // OnTurn event
  // Triggered when a creature turns

  namespace OnTurn {
    class Event : public Script::Event {
    public:
      Event(Creature* creature, Direction direction);
      ~Event();

      std::string getName() const {return "OnTurn";}

      // Runs the event
      bool dispatch(Manager& state, Environment& environment);

      // Lua stack manipulation
      void push_instance(LuaState& state, Environment& environment);
      void update_instance(Manager& state, Script::Environment& environment, LuaThread_ptr thread);

    protected:
      Creature* creature;
      Direction direction;
    };
  }

  ////////////////////////////////////////////////////////////////
  // OnMoveItem event
  // Triggered when an item is moved, updated, added or removed from a tile

  namespace OnMoveItem {
    enum FilterType {
      FILTER_ITEMID,
      FILTER_ACTIONID,
    };

    struct ScriptInformation {
      FilterType method;
      int32_t id;
      bool addItem;
      bool isItemOnTile;
      bool postEvent;
    };

    class Event : public Script::Event {
    public:
      // For events that will trigger after the move is completed
      Event(Creature* actor, Item* item, Tile* tile, bool addItem);

      // For events that will trigger before the move is completed
      Event(Creature* actor, Item* item, Tile* tile, bool addItem, ReturnValue& retval);

      ~Event();

      std::string getName() const {return "OnMoveItem";}

      // Runs the event
      bool dispatch(Manager& state, Environment& environment);

      // This checks if the script information matches this events prerequiste (data members)
      bool check_match(const ScriptInformation& info);

      // Lua stack manipulation
      void push_instance(LuaState& state, Environment& environment);
      void update_instance(Manager& state, Script::Environment& environment, LuaThread_ptr thread);

    private:
      ReturnValue dummyRetVal;

    protected:
      Creature* actor;
      Item* item;
      Tile* tile;
      bool addItem;
      bool postEvent;
      ReturnValue& retval;
    };
  }


  ///////////////////////////////////////////////////////////////////////////////
  // OnJoinChannel event
  // Triggered when a player opens a new chat channel

  namespace OnJoinChannel {
    class Event : public Script::Event {
    public:
      Event(Player* chatter, ChatChannel* chat);
      ~Event();

      std::string getName() const {return "OnJoinChannel";}

      // Runs the event
      bool dispatch(Manager& state, Environment& environment);

      // Lua stack manipulation
      void push_instance(LuaState& state, Environment& environment);
      void update_instance(Manager& state, Script::Environment& environment, LuaThread_ptr thread);

    protected:
      Player* chatter;
      ChatChannel* channel;
    };
  }

  ///////////////////////////////////////////////////////////////////////////////
  // OnLeaveChannel event
  // Triggered when a player closes an existing chat channel

  namespace OnLeaveChannel {
    class Event : public Script::Event {
    public:
      Event(Player* chatter, ChatChannel* chat);
      ~Event();

      std::string getName() const {return "OnLeaveChannel";}

      // Runs the event
      bool dispatch(Manager& state, Environment& environment);

      // Lua stack manipulation
      void push_instance(LuaState& state, Environment& environment);
      void update_instance(Manager& state, Script::Environment& environment, LuaThread_ptr thread);

    protected:
      Player* chatter;
      ChatChannel* channel;
    };
  }

  ///////////////////////////////////////////////////////////////////////////////
  // OnAccountLogin event
  // Triggered when the character list is retrieved from the database

  namespace OnAccountLogin {
    enum FilterType {
      FILTER_NAME
    };

    struct ScriptInformation {
      FilterType method;
      std::string name;
    };

    class Event : public Script::Event {
    public:
      Event(std::string& name, uint32_t& number, std::string& password,
        time_t& premiumEnd, uint32_t& warnings, std::list<AccountCharacter>& charList);
      ~Event();

      std::string getName() const {return "OnAccountLogin";}

      // Runs the event
      bool dispatch(Manager& state, Environment& environment);

      // This checks if the script information matches this events prerequiste (data members)
      bool check_match(const ScriptInformation& info);

      // Lua stack manipulation
      void push_instance(LuaState& state, Environment& environment);
      void update_instance(Manager& state, Script::Environment& environment, LuaThread_ptr thread);

    protected:
      std::string& name;
      uint32_t& number;
      std::string& password;
      time_t& premiumEnd;
      uint32_t& warnings;
      std::list<AccountCharacter>& charList;
    };
  }  

  ///////////////////////////////////////////////////////////////////////////////
  // OnLogin event
  // Triggered when a player enters the server

  namespace OnLogin {
    class Event : public Script::Event {
    public:
      Event(Player* player);
      ~Event();

      std::string getName() const {return "OnLogin";}

      // Runs the event
      bool dispatch(Manager& state, Environment& environment);

      // Lua stack manipulation
      void push_instance(LuaState& state, Environment& environment);
      void update_instance(Manager& state, Script::Environment& environment, LuaThread_ptr thread);

    protected:
      Player* player;
    };
  }

  ///////////////////////////////////////////////////////////////////////////////
  // OnLogout event
  // Triggered when a player leaves the server

  namespace OnLogout {
    class Event : public Script::Event {
    public:
      Event(Player* player, bool forced, bool timeout);
      ~Event();

      std::string getName() const {return "OnLogout";}

      // Runs the event
      bool dispatch(Manager& state, Environment& environment);

      // Lua stack manipulation
      void push_instance(LuaState& state, Environment& environment);
      void update_instance(Manager& state, Script::Environment& environment, LuaThread_ptr thread);

    protected:
      Player* player;
      bool forced;
      bool timeout;
    };
  }

  ///////////////////////////////////////////////////////////////////////////////
  // OnChangeOut event
  // Triggered when a player request the outfit dialog

  namespace OnChangeOutfit {
    class Event : public Script::Event {
    public:
      Event(Player* player, std::list<Outfit>& outfitList);
      ~Event();

      std::string getName() const {return "OnChangeOutfit";}

      // Runs the event
      bool dispatch(Manager& state, Environment& environment);

      // Lua stack manipulation
      void push_instance(LuaState& state, Environment& environment);
      void update_instance(Manager& state, Script::Environment& environment, LuaThread_ptr thread);

    protected:
      Player* player;
      std::list<Outfit>& outfitList;
    };
  }

  ///////////////////////////////////////////////////////////////////////////////
  // OnLook event
  // Triggered when a looks at an object

  namespace OnLook {
    enum FilterType {
      FILTER_NONE,
      FILTER_ITEMID,
      FILTER_ACTIONID
    };

    struct ScriptInformation {
      FilterType method;
      int32_t id;
    };

    class Event : public Script::Event {
    public:
      Event(Player* player, std::string& desc, Thing* object);
      ~Event();

      std::string getName() const {return "OnLook";}

      // Runs the event
      bool dispatch(Manager& state, Environment& environment);

      // This checks if the script information matches this events prerequiste (data members)
      bool check_match(const ScriptInformation& info);

      // Lua stack manipulation
      void push_instance(LuaState& state, Environment& environment);
      void update_instance(Manager& state, Script::Environment& environment, LuaThread_ptr thread);

    protected:
      Player* player;
      std::string& desc;
      Thing* object;
    };
  }

  ///////////////////////////////////////////////////////////////////////////////
  // OnSpotCreature event
  // Triggered when a creature spots another creature (OnCreatureAppear)

  namespace OnSpotCreature {
    class Event : public Script::Event {
    public:
      Event(Creature* creature, Creature* spotted_creature);
      ~Event();

      std::string getName() const {return "OnSpotCreature";}

      // Runs the event
      bool dispatch(Manager& state, Environment& environment);

      // Lua stack manipulation
      void push_instance(LuaState& state, Environment& environment);
      void update_instance(Manager& state, Script::Environment& environment, LuaThread_ptr thread);

    protected:
      Creature* creature;
      Creature* spotted_creature;
    };
  }

  ///////////////////////////////////////////////////////////////////////////////
  // OnLoseCreature event
  // Triggered when a creature loses a creature (OnCreatureDisappear)

  namespace OnLoseCreature {
    class Event : public Script::Event {
    public:
      Event(Creature* creature, Creature* lose_creature);
      ~Event();

      std::string getName() const {return "OnLoseCreature";}

      // Runs the event
      bool dispatch(Manager& state, Environment& environment);

      // Lua stack manipulation
      void push_instance(LuaState& state, Environment& environment);
      void update_instance(Manager& state, Script::Environment& environment, LuaThread_ptr thread);

    protected:
      Creature* creature;
      Creature* lose_creature;
    };
  }

  ///////////////////////////////////////////////////////////////////////////////
  // OnSpawn event
  // Triggered when a creature is spawned

  namespace OnSpawn {
    class Event : public Script::Event {
    public:
      Event(Actor* actor, bool reloading = false);
      ~Event();

      std::string getName() const {return "OnSpawn";}

      // Runs the event
      bool dispatch(Manager& state, Environment& environment);

      // Lua stack manipulation
      void push_instance(LuaState& state, Environment& environment);
      void update_instance(Manager& state, Script::Environment& environment, LuaThread_ptr thread);

    protected:
      Actor* actor;
      bool reloading;
    };
  }

  ///////////////////////////////////////////////////////////////////////////////
  // OnThink event
  // Triggered when a creature thinks (ie. all the time)

  namespace OnThink {
    class Event : public Script::Event {
    public:
      Event(Creature* creature, int32_t interval);
      ~Event();

      std::string getName() const {return "OnThink";}

      // Runs the event
      bool dispatch(Manager& state, Environment& environment);

      // Lua stack manipulation
      void push_instance(LuaState& state, Environment& environment);
      void update_instance(Manager& state, Script::Environment& environment, LuaThread_ptr thread);

    protected:
      Creature* creature;
      int32_t interval;
    };
  }

  ///////////////////////////////////////////////////////////////////////////////
  // OnAdvance event
  // Triggered when a player advances in a skill

  namespace OnAdvance {
    enum FilterType {
      FILTER_ALL,
      FILTER_SKILL
    };

    struct ScriptInformation {
      FilterType method;
      LevelType skill;
    };

    class Event : public Script::Event {
    public:
      Event(Player* player, LevelType skill, uint32_t oldskilllevel, uint32_t newskilllevel);
      ~Event();

      std::string getName() const {return "OnAdvance";}

      // Runs the event
      bool dispatch(Manager& state, Environment& environment);

      // This checks if the script information matches this events prerequiste (data members)
      bool check_match(const ScriptInformation& info);

      // Lua stack manipulation
      void push_instance(LuaState& state, Environment& environment);
      void update_instance(Manager& state, Script::Environment& environment, LuaThread_ptr thread);

    protected:
      Player* player;
      LevelType skill;
      uint32_t oldSkillLevel;
      uint32_t newSkillLevel;
    };
  }


  ////////////////////////////////////////////////////////////////
  // OnShopPurchase event
  // Triggered when a player wants to purchase an item from a NPC

  namespace OnShopPurchase {
    class Event : public Script::Event {
    public:
      Event(Player* player, uint16_t itemId, int32_t type, uint32_t amount, bool ignoreCapacity, bool buyWithBackpack);
      ~Event();

      std::string getName() const {return "onShopPurchase";}

      // Runs the event
      bool dispatch(Manager& state, Environment& environment);

      // Lua stack manipulation
      void push_instance(LuaState& state, Environment& environment);
      void update_instance(Manager& state, Script::Environment& environment, LuaThread_ptr thread);

    protected:
      Player* player;
      uint16_t itemId;
      int32_t type;
      uint32_t amount;
      bool ignoreCapacity;
      bool buyWithBackpack;
    };
  }

  ////////////////////////////////////////////////////////////////
  // OnShopSell event
  // Triggered when a player wants to sell an item to a NPC

  namespace OnShopSell {
    class Event : public Script::Event {
    public:
      Event(Player* player, uint16_t itemId, int32_t type, uint32_t amount);
      ~Event();

      std::string getName() const {return "onShopSell";}

      // Runs the event
      bool dispatch(Manager& state, Environment& environment);

      // Lua stack manipulation
      void push_instance(LuaState& state, Environment& environment);
      void update_instance(Manager& state, Script::Environment& environment, LuaThread_ptr thread);

    protected:
      Player* player;
      uint16_t itemId;
      int32_t type;
      uint32_t amount;
    };
  }

  ////////////////////////////////////////////////////////////////
  // OnShopClose event
  // Triggered when a player closes the shop window

  namespace OnShopClose {
    class Event : public Script::Event {
    public:
      Event(Player* player);
      ~Event();

      std::string getName() const {return "OnShopClose";}

      // Runs the event
      bool dispatch(Manager& state, Environment& environment);

      // Lua stack manipulation
      void push_instance(LuaState& state, Environment& environment);
      void update_instance(Manager& state, Script::Environment& environment, LuaThread_ptr thread);

    protected:
      Player* player;
    };
  }

  ////////////////////////////////////////////////////////////////
  // OnTradeBegin event
  // Triggered when a player initiate a trade with another player

  namespace OnTradeBegin {
    enum FilterType {
      FILTER_ALL,
      FILTER_ITEMID,
      FILTER_ACTIONID,
    };

    struct ScriptInformation {
      FilterType method;
      int32_t id;
    };

    class Event : public Script::Event {
    public:
      Event(Player* player1, Item* item1, Player* player2, Item* item2);
      ~Event();

      std::string getName() const {return "OnTradeBegin";}

      // Runs the event
      bool dispatch(Manager& state, Environment& environment);

      // This checks if the script information matches this events prerequiste (data members)
      bool check_match(const ScriptInformation& info);

      // Lua stack manipulation
      void push_instance(LuaState& state, Environment& environment);
      void update_instance(Manager& state, Script::Environment& environment, LuaThread_ptr thread);

    protected:
      Player* player1;
      Item* item1;
      Player* player2;
      Item* item2;
    };
  }

  ////////////////////////////////////////////////////////////////
  // OnTradeEnd event
  // Triggered when a trade is ended (cancelled or completed)

  namespace OnTradeEnd {
    enum FilterType {
      FILTER_ALL,
      FILTER_ITEMID,
      FILTER_ACTIONID,
    };

    struct ScriptInformation {
      FilterType method;
      int32_t id;
    };

    class Event : public Script::Event {
    public:
      Event(Player* player1, Item* item1, Player* player2, Item* item2, bool isCompleted);
      ~Event();

      std::string getName() const {return "OnTradeEnd";}

      // Runs the event
      bool dispatch(Manager& state, Environment& environment);

      // This checks if the script information matches this events prerequiste (data members)
      bool check_match(const ScriptInformation& info);

      // Lua stack manipulation
      void push_instance(LuaState& state, Environment& environment);
      void update_instance(Manager& state, Script::Environment& environment, LuaThread_ptr thread);

    protected:
      Player* player1;
      Item* item1;
      Player* player2;
      Item* item2;
      bool isCompleted;
    };
  }

  ///////////////////////////////////////////////////////////////////////////////
  // OnConditionEffect event
  // Triggered when a condition effect is added/removed or ticks

  namespace OnConditionEffect {
    enum FilterType {
      FILTER_BEGIN,
      FILTER_END,
      FILTER_TICK,
    };

    struct ScriptInformation {
      FilterType method;
      std::string name;
    };

    class Event : public Script::Event {
    public:
      Event(Creature* creature, ConditionEffect& effect);
      Event(Creature* creature, ConditionEffect& effect, ConditionEnd reason);
      Event(Creature* creature, ConditionEffect& effect, uint32_t ticks);
      ~Event();

      std::string getName() const {return "OnConditionEffect";}

      // Runs the event
      bool dispatch(Manager& state, Environment& environment);

      // This checks if the script information matches this events prerequiste (data members)
      bool check_match(const ScriptInformation& info);

      // Lua stack manipulation
      void push_instance(LuaState& state, Environment& environment);
      void update_instance(Manager& state, Script::Environment& environment, LuaThread_ptr thread);

    protected:
      Creature* creature;
      ConditionEffect& effect;
      ConditionEnd reason;
      uint32_t ticks;

      enum EventType{
        EVENT_BEGIN,
        EVENT_END,
        EVENT_TICK
      } eventType;

    };
  }

  ///////////////////////////////////////////////////////////////////////////////
  // OnAttack event
  // Triggered when a creature attacks another creature (usually every 2 seconds)

  namespace OnAttack {
    enum FilterType {
      FILTER_ALL,
      FILTER_NAME,
      FILTER_PLAYER,
      FILTER_ATTACKED_NAME,
      FILTER_ATTACKED_PLAYER,
      FILTER_ATTACKED_ACTOR,
      FILTER_PLAYER_ATTACK_ACTOR,
      FILTER_PLAYER_ATTACK_PLAYER,
      FILTER_ACTOR_ATTACK_ACTOR,
      FILTER_ACTOR_ATTACK_PLAYER
    };

    struct ScriptInformation {
      FilterType method;
      std::string name;
    };

    class Event : public Script::Event {
    public:
      Event(Creature* creature, Creature* attacked);
      ~Event();

      std::string getName() const {return "OnAttack";}

      // Runs the event
      bool dispatch(Manager& state, Environment& environment);

      // This checks if the script information matches this events prerequiste (data members)
      bool check_match(const ScriptInformation& info);

      // Lua stack manipulation
      void push_instance(LuaState& state, Environment& environment);
      void update_instance(Manager& state, Script::Environment& environment, LuaThread_ptr thread);

    protected:
      Creature* creature;
      Creature* attacked;
    };
  }

  ///////////////////////////////////////////////////////////////////////////////
  // OnDamage event
  // Triggered when a creature is taking damage (or heals)

  namespace OnDamage {
    enum FilterType {
      FILTER_ALL,
      FILTER_NAME,
      FILTER_PLAYER,
      FILTER_ATTACKER_NAME,
      FILTER_ATTACKER_PLAYER,
      FILTER_ATTACKER_ACTOR,
      FILTER_PLAYER_DAMAGE_PLAYER,
      FILTER_PLAYER_DAMAGE_ACTOR,
      FILTER_ACTOR_DAMAGE_ACTOR,
      FILTER_ACTOR_DAMAGE_PLAYER,
      FILTER_TYPE
    };

    struct ScriptInformation {
      FilterType method;
      std::string name;
      CombatType combatType;
    };

    class Event : public Script::Event {
    public:
      Event(CombatType& combatType, CombatSource& combatSource, Creature* creature, int32_t& value);
      ~Event();

      std::string getName() const {return "OnDamage";}

      // Runs the event
      bool dispatch(Manager& state, Environment& environment);

      // This checks if the script information matches this events prerequiste (data members)
      bool check_match(const ScriptInformation& info);

      // Lua stack manipulation
      void push_instance(LuaState& state, Environment& environment);
      void update_instance(Manager& state, Script::Environment& environment, LuaThread_ptr thread);

    protected:
      CombatType& combatType;
      CombatSource& combatSource;
      Creature* creature;
      int32_t& value;
    };
  }

  ///////////////////////////////////////////////////////////////////////////////
  // OnKill event
  // Triggered when a creature reaches 0 health

  namespace OnKill {
    enum FilterType {
      FILTER_ALL,
      FILTER_NAME,
      FILTER_PLAYER,
      FILTER_KILLER_NAME,
      FILTER_KILLER_PLAYER,
      FILTER_KILLER_ACTOR,
      FILTER_PLAYER_KILL_PLAYER,
      FILTER_PLAYER_KILL_ACTOR,
      FILTER_ACTOR_KILL_ACTOR,
      FILTER_ACTOR_KILL_PLAYER
    };

    struct ScriptInformation {
      FilterType method;
      std::string name;
    };

    class Event : public Script::Event {
    public:
      Event(Creature* creature, CombatSource& combatSource);
      ~Event();

      std::string getName() const {return "OnKill";}

      // Runs the event
      bool dispatch(Manager& state, Environment& environment);

      // This checks if the script information matches this events prerequiste (data members)
      bool check_match(const ScriptInformation& info);

      // Lua stack manipulation
      void push_instance(LuaState& state, Environment& environment);
      void update_instance(Manager& state, Script::Environment& environment, LuaThread_ptr thread);

    protected:
      Creature* creature;
      CombatSource& combatSource;
    };
  }

  ///////////////////////////////////////////////////////////////////////////////
  // OnDeath event
  // Triggered when a creature dies (after death-delay)

  namespace OnDeath {
    enum FilterType {
      FILTER_ALL,
      FILTER_NAME,
      FILTER_PLAYER,
      FILTER_KILLER_NAME,
      FILTER_KILLER_PLAYER,
      FILTER_KILLER_ACTOR,
      FILTER_PLAYER_DEATH_BY_PLAYER,
      FILTER_PLAYER_DEATH_BY_ACTOR,
      FILTER_ACTOR_DEATH_BY_ACTOR,
      FILTER_ACTOR_DEATH_BY_PLAYER
    };

    struct ScriptInformation {
      FilterType method;
      std::string name;
    };

    class Event : public Script::Event {
    public:
      Event(Creature* creature, Item* corpse, Creature* killer);
      ~Event();

      std::string getName() const {return "OnDeath";}

      // Runs the event
      bool dispatch(Manager& state, Environment& environment);

      // This checks if the script information matches this events prerequiste (data members)
      bool check_match(const ScriptInformation& info);

      // Lua stack manipulation
      void push_instance(LuaState& state, Environment& environment);
      void update_instance(Manager& state, Script::Environment& environment, LuaThread_ptr thread);

    protected:
      Creature* creature;
      Item* corpse;
      Creature* killer;
    };
  }

  ///////////////////////////////////////////////////////////////////////////////
  // OnActorLoadSpell event
  // Triggered while loading a spell (happens during startup/reload)

  namespace OnActorLoadSpell {
    class Event : public Script::Event {
    public:
      Event(const SpellBlock& spell);
      ~Event();

      std::string getName() const {return "OnActorLoadSpell";}

      // Runs the event
      bool dispatch(Manager& state, Environment& environment);

      // Lua stack manipulation
      void push_instance(LuaState& state, Environment& environment);
      void update_instance(Manager& state, Script::Environment& environment, LuaThread_ptr thread);

    protected:
      Actor* actor;
      const SpellBlock& spell;
    };
  }

  ///////////////////////////////////////////////////////////////////////////////
  // OnActorCastSpell event
  // Triggered when an Actor casts a spell

  namespace OnActorCastSpell {
    class Event : public Script::Event {
    public:
      Event(Actor* actor, Creature* target, const std::string& name);
      ~Event();

      std::string getName() const {return "OnActorCastSpell";}

      // Runs the event
      bool dispatch(Manager& state, Environment& environment);

      // Lua stack manipulation
      void push_instance(LuaState& state, Environment& environment);
      void update_instance(Manager& state, Script::Environment& environment, LuaThread_ptr thread);

    protected:
      Actor* actor;
      Creature* target;
      std::string name;
    };
  }
}

///////////////////////////////////////////////////////////////////////////////
// Implementation details

template<class T, class ScriptInformation>
bool dispatchEvent(T* e, Script::Manager& state, Script::Environment& environment, Script::Listener_ptr listener)
{
  if(listener->isActive() == false)
    return false;

  const ScriptInformation& info = boost::any_cast<const ScriptInformation>(listener->getData());

  // Call handler
  if(e->check_match(info)) {
    if(e->call(state, environment, listener) == true) {
      // Handled
      return true;
    }
  }
  return false;
}

template<class T>
bool dispatchEvent(T* e, Script::Manager& state, Script::Environment& environment, Script::Listener_ptr listener)
{
  if(listener->isActive() == false)
    return false;

  // Call handler
  if(e->call(state, environment, listener) == true) {
    // Handled
    return true;
  }
  return false;
}

template<class T, class ScriptInformation>
bool dispatchEvent(T* e, Script::Manager& state, Script::Environment& environment, Script::ListenerList& specific_list)
{
  if(specific_list.size() == 0)
    return false;

  for(Script::ListenerList::iterator event_iter = specific_list.begin();
    event_iter != specific_list.end();
    ++event_iter)
  {
    if(dispatchEvent<T, ScriptInformation>(e, state, environment, *event_iter))
      return true;
  }
  return false;
}

template<class T> // No script information!
bool dispatchEvent(T* e, Script::Manager& state, Script::Environment& environment, Script::ListenerList& specific_list)
{
  if(specific_list.size() == 0)
    return false;

  for(Script::ListenerList::iterator event_iter = specific_list.begin();
    event_iter != specific_list.end();
    ++event_iter)
  {
    if(dispatchEvent<T>(e, state, environment, *event_iter))
      return true;
  }
  return false;
}

#endif // __OTSERV_SCRIPT_EVENT__
