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

#include "depot.h"
#include "tools.h"
#include "position.h"

Depot::Depot(uint16_t _type) :
Container(_type)
{
  depotId = 0;
  maxSize = 30;
  maxDepotLimit = 1500;
}

Depot::~Depot()
{
  //
}

Depot* Depot::getDepot()
{
  return this;
}

const Depot* Depot::getDepot() const
{
  return this;
}

Attr_ReadValue Depot::readAttr(AttrTypes_t attr, PropStream& propStream)
{
  if(ATTR_DEPOT_ID == attr){
    uint16_t _depotId;
    if(!propStream.GET_USHORT(_depotId)){
      return ATTR_READ_ERROR;
    }

    setDepotId(_depotId);
    return ATTR_READ_CONTINUE;
  }
  else
    return Item::readAttr(attr, propStream);
}

uint32_t Depot::getDepotId() const
{
  return depotId;
}

void Depot::setMaxDepotLimit(uint32_t maxitems)
{
  maxDepotLimit = maxitems;
}

void Depot::setDepotId(uint32_t id)
{
  depotId = id;
}

Cylinder* Depot::getParent()
{
  return Item::getParent();
}

const Cylinder* Depot::getParent() const
{
  return Item::getParent();
}

bool Depot::isRemoved() const
{
  return Item::isRemoved();
}

Position Depot::getPosition() const
{
  return Item::getPosition();
}

Tile* Depot::getParentTile()
{
  return Item::getParentTile();
}

const Tile* Depot::getParentTile() const
{
  return Item::getParentTile();
}

Item* Depot::getItem()
{
  return this;
}

const Item* Depot::getItem() const
{
  return this;
}

Creature* Depot::getCreature()
{
  return NULL;
}

const Creature* Depot::getCreature() const
{
  return NULL;
}

ReturnValue Depot::__queryAdd(int32_t index, const Thing* thing, uint32_t count,
  uint32_t flags) const
{
  const Item* item = thing->getItem();
  if(item == NULL){
    return RET_NOTPOSSIBLE;
  }

  if(!hasBitSet(FLAG_IGNORECAPACITY, flags)){
    int addCount = 0;

    if((item->isStackable() && item->getItemCount() != count)){
      addCount = 1;
    }

    if(item->getTopParent() != this){
      if(const Container* container = item->getContainer()){
        addCount = container->getItemHoldingCount() + 1;
      }
      else{
        addCount = 1;
      }
    }

    if(getItemHoldingCount() + addCount > maxDepotLimit){
      return RET_DEPOTISFULL;
    }
  }

  return Container::__queryAdd(index, thing, count, flags);
}

ReturnValue Depot::__queryMaxCount(int32_t index, const Thing* thing, uint32_t count,
  uint32_t& maxQueryCount, uint32_t flags) const
{
  return Container::__queryMaxCount(index, thing, count, maxQueryCount, flags);
}

void Depot::postAddNotification(Creature* actor, Thing* thing, const Cylinder* oldParent, int32_t index, cylinderlink_t link /*= LINK_OWNER*/)
{
  if(getParent() != NULL){
    getParent()->postAddNotification(actor, thing, oldParent, index, LINK_PARENT);
  }
}

void Depot::postRemoveNotification(Creature* actor, Thing* thing, const Cylinder* newParent, int32_t index, bool isCompleteRemoval, cylinderlink_t link /*= LINK_OWNER*/)
{
  if(getParent() != NULL){
    getParent()->postRemoveNotification(actor, thing, newParent, index, isCompleteRemoval, LINK_PARENT);
  }
}

bool Depot::canRemove() const
{
  return false;
}
