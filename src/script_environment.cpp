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
#include "otpch.h"

#include "script_environment.h"
#include "script_listener.h"

using namespace Script;

Environment::Environment()
{
  objectID_counter = 0;
}

Environment::~Environment()
{
}

void Environment::cleanup()
{
  object_map.clear();
  objectID_counter = 0;
  Generic.OnSay.clear();
}

int32_t Environment::countObjects() const
{
  return object_map.size();
}

int32_t Environment::countSpecificListeners() const
{
  return specific_listeners.size();
}

int32_t Environment::countListeners() const
{
  return
    countSpecificListeners() +
    Generic.OnSay.size() + 
    Generic.OnUseItem.size() + 
    Generic.OnUseWeapon.size() + 
    Generic.OnUseFist.size() + 
    Generic.OnUseAnyWeapon.size() + 
    Generic.OnEquipItem.size() + 
    Generic.OnMoveInAnyCreature.size() + 
    Generic.OnMoveOutAnyCreature.size() + 
    Generic.OnMoveInCreature.size() + 
    Generic.OnMoveOutCreature.size() + 
    Generic.OnJoinChannel.size() + 
    Generic.OnLeaveChannel.size() + 
    Generic.OnAccountLogin.size() + 
    Generic.OnLogin.size() + 
    Generic.OnLogout.size() + 
    Generic.OnChangeOutfit.size() + 
    Generic.OnLook.size() + 
    Generic.OnTurn.size() + 
    Generic.OnLoad.size() + 
    Generic.OnUnload.size() + 
    Generic.OnSpawn.size() + 
    Generic.OnAdvance.size() + 
    Generic.OnTrade.size() + 
    Generic.OnShopPurchase.size() + 
    Generic.OnShopSell.size() + 
    Generic.OnShopClose.size() + 
    Generic.OnTradeBegin.size() + 
    Generic.OnTradeEnd.size() + 
    Generic.OnMoveItem.size() + 
    Generic.OnMoveItemOnItem.size() + 
    Generic.OnConditionEffect.size() + 
    Generic.OnAttack.size() + 
    Generic.OnDamage.size() + 
    Generic.OnKilled.size() + 
    Generic.OnKill.size() + 
    Generic.OnDeathBy.size() + 
    Generic.OnDeath.size() + 
    Generic.OnActorLoadSpell.size() + 
    Generic.OnActorCastSpell.size()
    ;
}

void Environment::cleanupUnusedListeners(ListenerList& list)
{
  for(ListenerList::iterator giter = list.begin(); giter != list.end();){
    if((*giter)->isActive() == false){
      giter = list.erase(giter);
    } else ++giter;
  }
}

void Environment::cleanupUnusedListeners(ListenerStringMap& list)
{
  for(ListenerStringMap::iterator giter = list.begin(), gend = list.end(); giter != gend; ++giter)
    cleanupUnusedListeners(giter->second);
}

void Environment::cleanupUnusedListeners(ListenerMap& list)
{
  for(ListenerMap::iterator giter = list.begin(), gend = list.end(); giter != gend; ++giter)
    cleanupUnusedListeners(giter->second);
}

void Environment::cleanupUnusedListeners(ListenerItemMap& list)
{
  cleanupUnusedListeners(list.ItemId);
  cleanupUnusedListeners(list.ActionId);
}

 void Environment::cleanupUnusedListeners()
 {
  cleanupUnusedListeners(Generic.OnSay);
  cleanupUnusedListeners(Generic.OnUseItem);
  cleanupUnusedListeners(Generic.OnUseWeapon);
  cleanupUnusedListeners(Generic.OnUseFist);
  cleanupUnusedListeners(Generic.OnUseAnyWeapon);
  cleanupUnusedListeners(Generic.OnEquipItem);
  cleanupUnusedListeners(Generic.OnMoveInAnyCreature);
  cleanupUnusedListeners(Generic.OnMoveOutAnyCreature);
  cleanupUnusedListeners(Generic.OnMoveInCreature);
  cleanupUnusedListeners(Generic.OnMoveOutCreature);
  cleanupUnusedListeners(Generic.OnJoinChannel);
  cleanupUnusedListeners(Generic.OnLeaveChannel);
  cleanupUnusedListeners(Generic.OnAccountLogin);
  cleanupUnusedListeners(Generic.OnLogin);
  cleanupUnusedListeners(Generic.OnLogout);
  cleanupUnusedListeners(Generic.OnChangeOutfit);
  cleanupUnusedListeners(Generic.OnLook);
  cleanupUnusedListeners(Generic.OnTurn);
  cleanupUnusedListeners(Generic.OnLoad);
  cleanupUnusedListeners(Generic.OnUnload);
  cleanupUnusedListeners(Generic.OnSpawn);
  cleanupUnusedListeners(Generic.OnAdvance);
  cleanupUnusedListeners(Generic.OnTrade);
  cleanupUnusedListeners(Generic.OnShopPurchase);
  cleanupUnusedListeners(Generic.OnShopSell);
  cleanupUnusedListeners(Generic.OnShopClose);
  cleanupUnusedListeners(Generic.OnTradeBegin);
  cleanupUnusedListeners(Generic.OnTradeEnd);
  cleanupUnusedListeners(Generic.OnMoveItem);
  cleanupUnusedListeners(Generic.OnMoveItemOnItem);
  cleanupUnusedListeners(Generic.OnConditionEffect);
  cleanupUnusedListeners(Generic.OnAttack);
  cleanupUnusedListeners(Generic.OnDamage);
  cleanupUnusedListeners(Generic.OnKilled);
  cleanupUnusedListeners(Generic.OnKill);
  cleanupUnusedListeners(Generic.OnDeathBy);
  cleanupUnusedListeners(Generic.OnDeath);
  cleanupUnusedListeners(Generic.OnActorLoadSpell);
  cleanupUnusedListeners(Generic.OnActorCastSpell);
}

void Environment::registerSpecificListener(Listener_ptr listener)
{
  specific_listeners[listener->getID()] = listener;
}

bool Environment::stopListener(ListenerList& list, uint32_t id)
{
  for(ListenerList::iterator giter = list.begin(),
    gend = list.end(); giter != gend;
    ++giter)
  {
    if((*giter)->getID() == id && (*giter)->isActive()){
      (*giter)->deactivate();
      return true;
    }
  }
  return false;
}

bool Environment::stopListener(ListenerStringMap& list, uint32_t id)
{

  for(ListenerStringMap::iterator giter = list.begin(), gend = list.end(); giter != gend; ++giter)
    if(stopListener(giter->second, id))
      return true;
  return false;
}

bool Environment::stopListener(ListenerItemMap& item_map, uint32_t id)
{

  Script::ListenerMap::iterator list_iter;
  for(list_iter = item_map.ItemId.begin(); list_iter != item_map.ItemId.end(); ++list_iter)
    if(stopListener(list_iter->second, id))
      return true;

  for(list_iter = item_map.ActionId.begin(); list_iter != item_map.ActionId.end(); ++list_iter)
    if(stopListener(list_iter->second, id))
      return true;

  return false;
}

bool Environment::stopListener(ListenerType type, uint32_t id)
{
  switch(type.value())
  {
    case enums::ON_SAY_LISTENER:
      if(stopListener(Generic.OnSay, id))
        return true;
      break;
    case enums::ON_USE_ITEM_LISTENER:
      if(stopListener(Generic.OnUseItem, id))
        return true;
      break;
    case enums::ON_USE_WEAPON_LISTENER:
      if(stopListener(Generic.OnUseWeapon, id) || stopListener(Generic.OnUseAnyWeapon, id) || stopListener(Generic.OnUseFist, id))
        return true;
      break;
    case enums::ON_EQUIP_ITEM_LISTENER:
      if(stopListener(Generic.OnEquipItem, id))
        return true;
      break;
    case enums::ON_MOVE_CREATURE_LISTENER:
      if(stopListener(Generic.OnMoveInAnyCreature, id) || stopListener(Generic.OnMoveInCreature, id) || stopListener(Generic.OnMoveOutAnyCreature, id) || stopListener(Generic.OnMoveOutCreature, id))
        return true;
      break;
    case enums::ON_MOVE_ITEM_LISTENER:
      if(stopListener(Generic.OnMoveItem, id))
        return true;
      break;
    case enums::ON_OPEN_CHANNEL_LISTENER:
      if(stopListener(Generic.OnJoinChannel, id))
        return true;
      break;
    case enums::ON_CLOSE_CHANNEL_LISTENER:
      if(stopListener(Generic.OnLeaveChannel, id))
        return true;
      break;
    case enums::ON_ACCOUNT_LOGIN_LISTENER:
      if(stopListener(Generic.OnAccountLogin, id))
        return true;
    case enums::ON_LOGIN_LISTENER:
      if(stopListener(Generic.OnLogin, id))
        return true;
      return false; // No specific listeners
    case enums::ON_LOGOUT_LISTENER:
      if(stopListener(Generic.OnLogout, id))
        return true;
      break;
    case enums::ON_CHANGE_OUTFIT_LISTENER:
      if(stopListener(Generic.OnChangeOutfit, id))
        return true;
      break;
    case enums::ON_TURN_LISTENER:
      if(stopListener(Generic.OnTurn, id))
        return true;
      break;
    case enums::ON_SPAWN_LISTENER:
      if(stopListener(Generic.OnSpawn, id))
        return true;
      return false; // No specific listeners
    case enums::ON_ADVANCE_LISTENER:
      if(stopListener(Generic.OnAdvance, id))
        return true;
    case enums::ON_SHOP_PURCHASE_LISTENER:
      if(stopListener(Generic.OnShopPurchase, id))
        return true;
      return false; // No specific listeners
    case enums::ON_SHOP_SELL_LISTENER:
      if(stopListener(Generic.OnShopSell, id))
        return true;
      return false; // No specific listeners
    case enums::ON_SHOP_CLOSE_LISTENER:
      if(stopListener(Generic.OnShopClose, id))
        return true;
      return false; // No specific listeners
    case enums::ON_TRADE_BEGIN_LISTENER:
      if(stopListener(Generic.OnTradeBegin, id))
        return true;
    case enums::ON_TRADE_END_LISTENER:
      if(stopListener(Generic.OnTradeEnd, id))
        return true;
    case enums::ON_ATTACK_LISTENER:
      if(stopListener(Generic.OnAttack, id))
        return true;
    case enums::ON_DAMAGE_LISTENER:
      if(stopListener(Generic.OnDamage, id))
        return true;
    case enums::ON_KILLED_LISTENER:
      if(stopListener(Generic.OnKilled, id))
        return true;
    case enums::ON_KILL_LISTENER:
      if(stopListener(Generic.OnKill, id))
        return true;
    case enums::ON_DEATH_BY_LISTENER:
      if(stopListener(Generic.OnDeathBy, id))
        return true;
    case enums::ON_DEATH_LISTENER:
      if(stopListener(Generic.OnDeath, id))
        return true;
    case enums::ON_ACTOR_LOAD_SPELL_LISTENER:
      if(stopListener(Generic.OnActorLoadSpell, id))
        return true;
    case enums::ON_ACTOR_CAST_SPELL_LISTENER:
      if(stopListener(Generic.OnActorCastSpell, id))
        return true;
    default:
      break;
  }
  
  // Try specific
  SpecificListenerMap::iterator iter = specific_listeners.find(id);
  if(iter != specific_listeners.end()) {
    Listener_ptr listener = iter->second;
    listener->deactivate();
    specific_listeners.erase(iter);
    return true;
  }

  return false;
}

bool Environment::stopListener(Listener_ptr listener)
{
  return stopListener(listener->type(), listener->getID());
}
