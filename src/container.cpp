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
#include "otpch.h"

#include "container.h"
#include "iomapotbm.h"
#include "player.h"
#include "game.h"

extern Game g_game;

Container::Container(uint16_t _type) : Item(_type)
{
  //std::cout << "Container constructor " << this << std::endl;
  maxSize = items[_type].maxItems;
  total_weight = 0.0;
  serializationCount = 0;
}

Container::~Container()
{
  //std::cout << "Container destructor " << this << std::endl;
  for(ItemList::iterator cit = itemlist.begin(); cit != itemlist.end(); ++cit){
    (*cit)->setParent(NULL);
    (*cit)->unRef();
  }

  itemlist.clear();
}

Item* Container::clone() const
{
  Container* _item = static_cast<Container*>(Item::clone());
  for(ItemList::const_iterator it = itemlist.begin(); it != itemlist.end(); ++it){
    _item->addItem((*it)->clone());
  }
  return _item;
}

Container* Container::getContainer()
{
  return this;
}

const Container* Container::getContainer() const
{
  return this;
}

Depot* Container::getDepot()
{
  return NULL;
}

const Depot* Container::getDepot() const
{
  return NULL;
}

Container* Container::getParentContainer()
{
  if(Cylinder* cylinder = getParent()){
    if(Item* item = cylinder->getItem()){
      return item->getContainer();
    }
  }

  return NULL;
}

void Container::addItem(Item* item)
{
  itemlist.push_back(item);
  item->setParent(this);
}

Attr_ReadValue Container::readAttr(AttrTypes_t attr, PropStream& propStream)
{
  switch(attr){
    case ATTR_CONTAINER_ITEMS:
    {
      uint32_t count;
      if(!propStream.GET_ULONG(count)){
        return ATTR_READ_ERROR;
      }
      serializationCount = count;
      return ATTR_READ_END;
    }

    default:
      break;
  }

  return Item::readAttr(attr, propStream);
}

bool Container::unserializeItemNode(FileLoader& f, NodeStruct* node, PropStream& propStream)
{
  bool ret = Item::unserializeItemNode(f, node, propStream);

  if(ret){
    unsigned long type;
    NodeStruct* nodeItem = f.getChildNode(node, type);
    while(nodeItem){
      //load container items
      if(type == OTBM_ITEM){
        PropStream itemPropStream;
        f.getProps(nodeItem, itemPropStream);

        Item* item = Item::CreateItem(itemPropStream);
        if(!item){
          return false;
        }

        if(!item->unserializeItemNode(f, nodeItem, itemPropStream)){
          return false;
        }

        addItem(item);
        total_weight += item->getWeight();
        if(Container* parent_container = getParentContainer()) {
          parent_container->updateItemWeight(item->getWeight());
        }
      }
      else /*unknown type*/
        return false;

      nodeItem = f.getNextNode(nodeItem, type);
    }

    return true;
  }

  return false;
}

void Container::updateItemWeight(double diff)
{
  total_weight += diff;
  if(Container* parent_container = getParentContainer()){
    parent_container->updateItemWeight(diff);
  }
}

double Container::getWeight() const
{
  return Item::getWeight() + total_weight;
}

Cylinder* Container::getParent()
{
  return Thing::getParent();
}

const Cylinder* Container::getParent() const
{
  return Thing::getParent();
}

bool Container::isRemoved() const
{
  return Thing::isRemoved();
}

Position Container::getPosition() const
{
  return Thing::getPosition();
}

Tile* Container::getTile()
{
  return NULL;
}

const Tile* Container::getTile() const
{
  return NULL;
}

Item* Container::getItem()
{
  return this;
}

const Item* Container::getItem() const
{
  return this;
}

Creature* Container::getCreature()
{
  return NULL;
}

const Creature* Container::getCreature() const
{
  return NULL;
}

Tile* Container::getParentTile()
{
  return Thing::getParentTile();
}

const Tile* Container::getParentTile() const
{
  return Thing::getParentTile();
}

std::string Container::getContentDescription() const
{
  std::ostringstream os;
  return getContentDescription(os).str();
}

uint32_t Container::size() const
{
  return (uint32_t)itemlist.size();
}

bool Container::empty() const
{
  return itemlist.empty();
}

std::ostringstream& Container::getContentDescription(std::ostringstream& os) const
{
  bool firstitem = true;
  Container* evil = const_cast<Container*>(this);
  for(ContainerIterator cit = evil->begin(); cit != evil->end(); ++cit)
  {
    Item* i = *cit;

    if(firstitem)
      firstitem = false;
    else
      os << ", ";

    os << i->getLongName();
  }

  if(firstitem)
    os << "nothing";

  return os;
}

Item* Container::getItem(uint32_t index)
{
  size_t n = 0;
  for (ItemList::const_iterator cit = getItems(); cit != getEnd(); ++cit) {
    if(n == index)
      return *cit;
    else
      ++n;
  }

  return NULL;
}

uint32_t Container::getItemHoldingCount() const
{
  uint32_t counter = 0;

  for(ContainerIterator iter = begin(); iter != end(); ++iter){
    ++counter;
  }

  return counter;
}

bool Container::isHoldingItem(const Item* item) const
{
  for(ContainerIterator cit = begin(); cit != end(); ++cit){
    if(*cit == item)
      return true;
  }
  return false;
}

uint32_t Container::capacity() const
{
  return maxSize;
}

void Container::onAddContainerItem(Item* item)
{
  const Position& cylinderMapPos = getPosition();

  SpectatorVec list;
  SpectatorVec::iterator it;
  g_game.getSpectators(list, cylinderMapPos, false, false, 2, 2, 2, 2);

  //send to client
  Player* player = NULL;
  for(it = list.begin(); it != list.end(); ++it) {
    if((player = (*it)->getPlayer())){
      player->sendAddContainerItem(this, item);
    }
  }

  //event methods
  for(it = list.begin(); it != list.end(); ++it) {
    if((player = (*it)->getPlayer())){
      player->onAddContainerItem(this, item);
    }
  }
}

void Container::onUpdateContainerItem(uint32_t index, Item* oldItem, const ItemType& oldType,
  Item* newItem, const ItemType& newType)
{
  const Position& cylinderMapPos = getPosition();

  SpectatorVec list;
  SpectatorVec::iterator it;
  g_game.getSpectators(list, cylinderMapPos, false, false, 2, 2, 2, 2);

  //send to client
  Player* player = NULL;
  for(it = list.begin(); it != list.end(); ++it) {
    if((player = (*it)->getPlayer())){
      player->sendUpdateContainerItem(this, index, oldItem, newItem);
    }
  }

  //event methods
  for(it = list.begin(); it != list.end(); ++it) {
    if((player = (*it)->getPlayer())){
      player->onUpdateContainerItem(this, index, oldItem, oldType, newItem, newType);
    }
  }
}

void Container::onRemoveContainerItem(uint32_t index, Item* item)
{
  const Position& cylinderMapPos = getPosition();

  SpectatorVec list;
  SpectatorVec::iterator it;
  g_game.getSpectators(list, cylinderMapPos, false, false, 2, 2, 2, 2);

  //send change to client
  Player* player = NULL;
  for(it = list.begin(); it != list.end(); ++it) {
    if((player = (*it)->getPlayer())){
      player->sendRemoveContainerItem(this, index, item);
    }
  }

  //event methods
  for(it = list.begin(); it != list.end(); ++it) {
    if((player = (*it)->getPlayer())){
      player->onRemoveContainerItem(this, index, item);
    }
  }
}

ReturnValue Container::__queryAdd(int32_t index, const Thing* thing, uint32_t count,
  uint32_t flags) const
{
  const Item* item = thing->getItem();
  if(item == NULL){
    return RET_NOTPOSSIBLE;
  }

  if(!item->isPickupable()){
    return RET_CANNOTPICKUP;
  }

  if(item == this){
    return RET_THISISIMPOSSIBLE;
  }

  if(const Container* container = item->getContainer()){
    const Cylinder* cylinder = getParent();
    while(cylinder){
      if(cylinder == container){
        return RET_THISISIMPOSSIBLE;
      }
      cylinder = cylinder->getParent();
    }
  }

  if(!hasBitSet(FLAG_IGNORECAPACITY, flags)){
    if(index == INDEX_WHEREEVER){
      if(size() >= capacity())
        return RET_CONTAINERNOTENOUGHROOM;
    }

    const Cylinder* topParent = getTopParent();
    if(topParent){
      if(topParent->getCreature()){
        const Player* player = topParent->getCreature()->getPlayer();
        if(!player->hasCapacity(item, count)){
          return RET_NOTENOUGHCAPACITY;
        }
      }
    }
  }

  return RET_NOERROR;
}

ReturnValue Container::__queryMaxCount(int32_t index, const Thing* thing, uint32_t count,
  uint32_t& maxQueryCount, uint32_t flags) const
{
  const Item* item = thing->getItem();
  if(item == NULL){
    maxQueryCount = 0;
    return RET_NOTPOSSIBLE;
  }

  if(hasBitSet(FLAG_IGNORECAPACITY, flags)){
    maxQueryCount = std::max((uint32_t)1, count);
    return RET_NOERROR;
  }

  int32_t freeSlots = std::max((int32_t)(capacity() - size()), (int32_t)0);

  if(item->isStackable()){
    uint32_t n = 0;

    if(index == INDEX_WHEREEVER){
      //Iterate through every item and check how much free stackable slots there is.
      uint32_t slotIndex = 0;
      for(ItemList::const_iterator cit = itemlist.begin(); cit != itemlist.end(); ++cit, ++slotIndex){

        if((*cit) != item && (*cit)->getID() == item->getID() && (*cit)->getItemCount() < 100){
          uint32_t remainder = (100 - (*cit)->getItemCount());
          if(__queryAdd(slotIndex, item, remainder, flags) == RET_NOERROR){
            n += remainder;
          }
        }
      }
    }
    else{
      const Thing* destThing = __getThing(index);
      const Item* destItem = NULL;
      if(destThing)
        destItem = destThing->getItem();

      if(destItem && destItem->getID() == item->getID() && destItem->getItemCount() < 100){
        uint32_t remainder = 100 - destItem->getItemCount();
        if(__queryAdd(index, item, remainder, flags) == RET_NOERROR){
          n = remainder;
        }
      }
    }

    maxQueryCount = freeSlots * 100 + n;

    if(maxQueryCount < count){
      return RET_CONTAINERNOTENOUGHROOM;
    }
  }
  else{
    maxQueryCount = freeSlots;

    if(maxQueryCount == 0){
      return RET_CONTAINERNOTENOUGHROOM;
    }
  }

  return RET_NOERROR;
}

ReturnValue Container::__queryRemove(const Thing* thing, uint32_t count, uint32_t flags) const
{
  int32_t index = __getIndexOfThing(thing);

  if(index == -1){
    return RET_NOTPOSSIBLE;
  }

  const Item* item = thing->getItem();
  if(item == NULL){
    return RET_NOTPOSSIBLE;
  }

  if(count == 0 || (item->isStackable() && count > item->getItemCount())){
    return RET_NOTPOSSIBLE;
  }

  if(!item->isMoveable() && !hasBitSet(FLAG_IGNORENOTMOVEABLE, flags)){
    return RET_NOTMOVEABLE;
  }

  return RET_NOERROR;
}

Cylinder* Container::__queryDestination(int32_t& index, const Thing* thing, Item** destItem,
  uint32_t& flags)
{
  if(index == 254 /*move up*/){
    index = INDEX_WHEREEVER;
    *destItem = NULL;

    Container* parentContainer = dynamic_cast<Container*>(getParent());
    if(parentContainer)
      return parentContainer;

    return this;
  }

  if(index == 255 /*add wherever*/){
    index = INDEX_WHEREEVER;
    *destItem = NULL;
  }
  else if(index >= (int32_t)capacity()){
    /*
    if you have a container, maximize it to show all 20 slots
    then you open a bag that is inside the container you will have a bag with 8 slots
    and a "grey" area where the other 12 slots where from the container
    if you drop the item on that grey area
    the client calculates the slot position as if the bag has 20 slots
    */

    index = INDEX_WHEREEVER;
    *destItem = NULL;
  }

  const Item* item = thing->getItem();
  if(item == NULL){
    return this;
  }

  if(item->isStackable()){
    if(item->getParent() != this){
      //try find a suitable item to stack with
      uint32_t n = 0;
      for(ItemList::iterator cit = itemlist.begin(); cit != itemlist.end(); ++cit){
        if((*cit) != item && (*cit)->getID() == item->getID() && (*cit)->getItemCount() < 100){
          *destItem = (*cit);
          index = n;
          return this;
        }

        ++n;
      }
    }

    if(index != INDEX_WHEREEVER){
      Thing* destThing = __getThing(index);
      if(destThing)
        *destItem = destThing->getItem();

      Cylinder* subCylinder = dynamic_cast<Cylinder*>(*destItem);

      if(subCylinder){
        index = INDEX_WHEREEVER;
        *destItem = NULL;
        return subCylinder;
      }
    }
  }

  return this;
}

void Container::__addThing(Creature* actor, Thing* thing)
{
  return __addThing(actor, 0, thing);
}

void Container::__addThing(Creature* actor, int32_t index, Thing* thing)
{
  if(index >= (int32_t)capacity()){
#ifdef __DEBUG__MOVESYS__
    std::cout << "Failure: [Container::__addThing], index:" << index << ", index >= capacity()" << std::endl;
    DEBUG_REPORT
#endif
    return /*RET_NOTPOSSIBLE*/;
  }
  Item* item = thing->getItem();

  if(item == NULL){
#ifdef __DEBUG__MOVESYS__
    std::cout << "Failure: [Container::__addThing] item == NULL" << std::endl;
    DEBUG_REPORT
#endif
    return /*RET_NOTPOSSIBLE*/;
  }

#ifdef __DEBUG__MOVESYS__
  if(index != INDEX_WHEREEVER){
    if(size() >= capacity()){
      std::cout << "Failure: [Container::__addThing] size() >= capacity()" << std::endl;
      DEBUG_REPORT
      return /*RET_CONTAINERNOTENOUGHROOM*/;
    }
  }
#endif

  item->setParent(this);
  itemlist.push_front(item);
  total_weight += item->getWeight();
  if(Container* parent_container = getParentContainer()) {
    parent_container->updateItemWeight(item->getWeight());
  }

  //send change to client
  if(getParent() && (getParent() != VirtualCylinder::virtualCylinder)){
    onAddContainerItem(item);
  }
}

void Container::__updateThing(Creature* actor, Thing* thing, uint16_t itemId, uint32_t count)
{
  int32_t index = __getIndexOfThing(thing);
  if(index == -1){
#ifdef __DEBUG__MOVESYS__
    std::cout << "Failure: [Container::__updateThing] index == -1" << std::endl;
    DEBUG_REPORT
#endif
    return /*RET_NOTPOSSIBLE*/;
  }

  Item* item = thing->getItem();
  if(item == NULL){
#ifdef __DEBUG__MOVESYS__
    std::cout << "Failure: [Container::__updateThing] item == NULL" << std::endl;
    DEBUG_REPORT
#endif
    return /*RET_NOTPOSSIBLE*/;
  }

  const ItemType& oldType = Item::items[item->getID()];
  const ItemType& newType = Item::items[itemId];

  const double old_weight = item->getWeight();

  item->setID(itemId);
  item->setSubType(count);

  const double diff_weight = -old_weight + item->getWeight();
  total_weight += diff_weight;
  if(Container* parent_container = getParentContainer()) {
    parent_container->updateItemWeight(diff_weight);
  }

  //send change to client
  if(getParent()){
    onUpdateContainerItem(index, item, oldType, item, newType);
  }
}

void Container::__replaceThing(Creature* actor, uint32_t index, Thing* thing)
{
  Item* item = thing->getItem();
  if(item == NULL){
#ifdef __DEBUG__MOVESYS__
    std::cout << "Failure: [Container::__replaceThing] item == NULL" << std::endl;
    DEBUG_REPORT
#endif
    return /*RET_NOTPOSSIBLE*/;
  }

  uint32_t count = 0;
  ItemList::iterator cit = itemlist.end();
  for(cit = itemlist.begin(); cit != itemlist.end(); ++cit){
    if(count == index)
      break;
    else
      ++count;
  }

  if(cit == itemlist.end()){
#ifdef __DEBUG__MOVESYS__
    std::cout << "Failure: [Container::__updateThing] item not found" << std::endl;
    DEBUG_REPORT
#endif
    return /*RET_NOTPOSSIBLE*/;
  }

  total_weight -= (*cit)->getWeight();
  total_weight += item->getWeight();

  if(Container* parent_container = getParentContainer()) {
    parent_container->updateItemWeight(-(*cit)->getWeight() + item->getWeight());
  }

  itemlist.insert(cit, item);
  item->setParent(this);

  //send change to client
  if(getParent()){
    const ItemType& oldType = Item::items[(*cit)->getID()];
    const ItemType& newType = Item::items[item->getID()];
    onUpdateContainerItem(index, *cit, oldType, item, newType);
  }

  (*cit)->setParent(NULL);
  itemlist.erase(cit);
}

void Container::__removeThing(Creature* actor, Thing* thing, uint32_t count)
{
  Item* item = thing->getItem();
  if(item == NULL){
#ifdef __DEBUG__MOVESYS__
    std::cout << "Failure: [Container::__removeThing] item == NULL" << std::endl;
    DEBUG_REPORT
#endif
    return /*RET_NOTPOSSIBLE*/;
  }

  int32_t index = __getIndexOfThing(thing);
  if(index == -1){
#ifdef __DEBUG__MOVESYS__
    std::cout << "Failure: [Container::__removeThing] index == -1" << std::endl;
    DEBUG_REPORT
#endif
    return /*RET_NOTPOSSIBLE*/;
  }

  ItemList::iterator cit = std::find(itemlist.begin(), itemlist.end(), thing);
  if(cit == itemlist.end()){
#ifdef __DEBUG__MOVESYS__
    std::cout << "Failure: [Container::__removeThing] item not found" << std::endl;
    DEBUG_REPORT
#endif
    return /*RET_NOTPOSSIBLE*/;
  }

  if(item->isStackable() && count != item->getItemCount()){
    uint8_t newCount = (uint8_t)std::max((int32_t)0, (int32_t)(item->getItemCount() - count));

    const double old_weight = -item->getWeight();
    item->setItemCount(newCount);
    const double diff_weight = old_weight + item->getWeight();
    total_weight += diff_weight;

    //send change to client
    if(getParent()){
      if(Container* parent_container = getParentContainer()) {
        parent_container->updateItemWeight(diff_weight);
      }
      const ItemType& it = Item::items[item->getID()];
      onUpdateContainerItem(index, item, it, item, it);
    }
  }
  else{
    //send change to client
    if(getParent()){
      if(Container* parent_container = getParentContainer()) {
        parent_container->updateItemWeight(-item->getWeight());\
      }
      onRemoveContainerItem(index, item);
    }

    total_weight -= item->getWeight();
    item->setParent(NULL);
    itemlist.erase(cit);
  }
}

int32_t Container::__getIndexOfThing(const Thing* thing) const
{
  uint32_t index = 0;
  for(ItemList::const_iterator cit = getItems(); cit != getEnd(); ++cit){
    if(*cit == thing)
      return index;
    else
      ++index;
  }

  return -1;
}

int32_t Container::__getFirstIndex() const
{
  return 0;
}

int32_t Container::__getLastIndex() const
{
  return size();
}

uint32_t Container::__getItemTypeCount(uint16_t itemId, int32_t subType /*= -1*/) const
{
  uint32_t count = 0;
  for(ItemList::const_iterator it = itemlist.begin(); it != itemlist.end(); ++it){
    if((*it)->getID() == itemId){
      count += countByType(*it, subType);
    }
  }

  return count;
}

std::map<uint32_t, uint32_t>& Container::__getAllItemTypeCount(std::map<uint32_t, uint32_t>& countMap) const
{
  for(ItemList::const_iterator it = itemlist.begin(); it != itemlist.end(); ++it){
    countMap[(*it)->getID()] += (*it)->getItemCount();
  }

  return countMap;
}

Thing* Container::__getThing(uint32_t index) const
{
  if(index > size())
    return NULL;

  uint32_t count = 0;
  for(ItemList::const_iterator cit = itemlist.begin(); cit != itemlist.end(); ++cit){
    if(count == index)
      return *cit;
    else
      ++count;
  }

  return NULL;
}

void Container::postAddNotification(Creature* actor, Thing* thing, const Cylinder* oldParent, int32_t index, cylinderlink_t link /*= LINK_OWNER*/)
{
  Cylinder* topParent = getTopParent();

  if(topParent->getCreature()){
    topParent->postAddNotification(actor, thing, oldParent, index, LINK_TOPPARENT);
  }
  else{
    if(topParent == this){
      //let the tile class notify surrounding players
      if(topParent->getParent()){
        topParent->getParent()->postAddNotification(actor, thing, oldParent, index, LINK_NEAR);
      }
    }
    else{
      topParent->postAddNotification(actor, thing, oldParent, index, LINK_PARENT);
    }
  }
}

void Container::postRemoveNotification(Creature* actor, Thing* thing,  const Cylinder* newParent, int32_t index, bool isCompleteRemoval, cylinderlink_t link /*= LINK_OWNER*/)
{
  Cylinder* topParent = getTopParent();

  if(topParent->getCreature()){
    topParent->postRemoveNotification(actor, thing, newParent, index, isCompleteRemoval, LINK_TOPPARENT);
  }
  else{
    if(topParent == this){
      //let the tile class notify surrounding players
      if(topParent->getParent()){
        topParent->getParent()->postRemoveNotification(actor, thing, newParent, index, isCompleteRemoval, LINK_NEAR);
      }
    }
    else{
      topParent->postRemoveNotification(actor, thing, newParent, index, isCompleteRemoval, LINK_PARENT);
    }
  }
}

void Container::__internalAddThing(Thing* thing)
{
  __internalAddThing(0, thing);
}

void Container::__internalAddThing(uint32_t index, Thing* thing)
{
#ifdef __DEBUG__MOVESYS__
  std::cout << "[Container::__internalAddThing] index: " << index << std::endl;
#endif

  Item* item = thing->getItem();
  if(item == NULL){
#ifdef __DEBUG__MOVESYS__
    std::cout << "Failure: [Container::__internalAddThing] item == NULL" << std::endl;
#endif
    return;
  }

  /*
  if(index < 0 || index >= capacity()){
#ifdef __DEBUG__MOVESYS__
    std::cout << "Failure: [Container::__internalAddThing] - index is out of range" << std::endl;
#endif
    return;
  }
  */

  item->setParent(this);
  itemlist.push_front(item);

  total_weight += item->getWeight();
  if(Container* parent_container = getParentContainer()) {
    parent_container->updateItemWeight(item->getWeight());
  }
}

ContainerIterator Container::begin()
{
  ContainerIterator cit(this);
  if(!itemlist.empty()){
    cit.over.push(this);
    cit.cur = itemlist.begin();
  }

  return cit;
}

ContainerIterator Container::end()
{
  ContainerIterator cit(this);
  return cit;
}

// Very evil constructors, look away if you are sensitive!
ContainerIterator Container::begin() const
{
  Container* evil = const_cast<Container*>(this);
  return evil->begin();
}

ContainerIterator Container::end() const
{
  Container* evil = const_cast<Container*>(this);
  return evil->end();
}

ItemList::const_iterator Container::getItems() const
{
  return itemlist.begin();
}

ItemList::const_iterator Container::getEnd() const
{
  return itemlist.end();
}

ItemList::const_reverse_iterator Container::getReversedItems() const
{
  return itemlist.rbegin();
}

ItemList::const_reverse_iterator Container::getReversedEnd() const
{
  return itemlist.rend();
}

ContainerIterator::ContainerIterator():
  super(NULL) {}

ContainerIterator::ContainerIterator(Container* super):
  super(super) {}

ContainerIterator::~ContainerIterator() {}

ContainerIterator::ContainerIterator(const ContainerIterator& rhs):
  super(rhs.super), over(rhs.over), cur(rhs.cur) {}

bool ContainerIterator::operator==(const ContainerIterator& rhs)
{
  return !(*this != rhs);
}

bool ContainerIterator::operator!=(const ContainerIterator& rhs)
{
  assert(super);
  // iterators that belong to different container are never equal
  if(super != rhs.super)
    return true;

  // If both are empty, we are equal (end iterators)
  if(over.empty() && rhs.over.empty())
    return false;

  // if we are empty, but the other is not, we are not equal
  if(over.empty())
    return true;

  // if the other is empty, but we are not, we are not equal
  if(rhs.over.empty())
    return true;

  // If we don't point to the same sub-container, we are not equal
  if(over.front() != rhs.over.front())
    return true;

  // We both iterator over the same container, compare our iterators
  return cur != rhs.cur;
}

ContainerIterator& ContainerIterator::operator=(const ContainerIterator& rhs)
{
  this->super = rhs.super;
  this->cur = rhs.cur;
  this->over = rhs.over;
  return *this;
}

Item* ContainerIterator::operator*()
{
  assert(super);
  return *cur;
}

Item* ContainerIterator::operator->()
{
  return *(*this);
}

ContainerIterator& ContainerIterator::operator++()
{
  assert(super);
  // Does the iterator point to an item
  if(Item* i = *cur){
    Container* c = i->getContainer();
    // If so, is it a container and non-empty?
    if(c && !c->empty()){
      // Add it as a sub-container
      over.push(c);
    }
  }

  // increment internal iterator
  ++cur;
  // If we point to the end of the current sub-container
  if(cur == over.front()->itemlist.end()){
    // pop that sub-container
    over.pop();
    // If there are no sub-containers left, return ourselves (end iterator)
    if(over.empty()){
      return *this;
    }

    // Else, start iterating over the first item in the next sub-container
    cur = over.front()->itemlist.begin();
  }

  // return ourselves
  return *this;
}

ContainerIterator ContainerIterator::operator++(int)
{
  ContainerIterator tmp(*this);
  ++*this;
  return tmp;
}
