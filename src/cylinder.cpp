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

#include "cylinder.h"
#include "thing.h"
#include "tile.h"
#include "position.h"
#include "creature.h"

VirtualCylinder* VirtualCylinder::virtualCylinder = new VirtualCylinder;

int32_t Cylinder::__getIndexOfThing(const Thing* thing) const
{
  return -1;
}

int32_t Cylinder::__getFirstIndex() const
{
  return -1;
}

int32_t Cylinder::__getLastIndex() const
{
  return -1;
}

uint32_t Cylinder::__getItemTypeCount(uint16_t itemId, int32_t subType /*= -1*/) const
{
  return 0;
}

std::map<uint32_t, uint32_t>& Cylinder::__getAllItemTypeCount(std::map<uint32_t, uint32_t>& countMap) const
{
  return countMap;
}

Thing* Cylinder::__getThing(uint32_t index) const
{
  return NULL;
}

void Cylinder::__internalAddThing(Thing* thing)
{
  //
}

void Cylinder::__internalAddThing(uint32_t index, Thing* thing)
{
  //
}

VirtualCylinder::~VirtualCylinder()
{
}

//cylinder implementations
Cylinder* VirtualCylinder::getParent()
{
  return NULL;
}

bool VirtualCylinder::isRemoved() const
{
  return false;
}

const Cylinder* VirtualCylinder::getParent() const
{
  return NULL;
}

Position VirtualCylinder::getPosition() const
{
  return Position();
}

Tile* VirtualCylinder::getTile()
{
  return NULL;
}

const Tile* VirtualCylinder::getTile() const
{
  return NULL;
}

Item* VirtualCylinder::getItem()
{
  return NULL;
}

const Item* VirtualCylinder::getItem() const
{
  return NULL;
}

Creature* VirtualCylinder::getCreature()
{
  return NULL;
}

const Creature* VirtualCylinder::getCreature() const
{
  return NULL;
}

Tile* VirtualCylinder::getParentTile()
{
  return NULL;
}

const Tile* VirtualCylinder::getParentTile() const
{
  return NULL;
}

ReturnValue VirtualCylinder::__queryAdd(int32_t index, const Thing* thing, uint32_t count,
  uint32_t flags) const
{
  return RET_NOTPOSSIBLE;
}

ReturnValue VirtualCylinder::__queryMaxCount(int32_t index, const Thing* thing, uint32_t count,
  uint32_t& maxQueryCount, uint32_t flags) const
{
  return RET_NOTPOSSIBLE;
}

ReturnValue VirtualCylinder::__queryRemove(const Thing* thing, uint32_t count, uint32_t flags) const
{
  return (thing->getParent() == this ? RET_NOERROR : RET_NOTPOSSIBLE);
}

Cylinder* VirtualCylinder::__queryDestination(int32_t& index, const Thing* thing, Item** destItem,
  uint32_t& flags)
{
  return NULL;
}

void VirtualCylinder::__addThing(Creature* actor, Thing* thing)
{

}

void VirtualCylinder::__addThing(Creature* actor, int32_t index, Thing* thing)
{

}

void VirtualCylinder::__updateThing(Creature* actor, Thing* thing, uint16_t itemId, uint32_t count)
{
}

void VirtualCylinder::__replaceThing(Creature* actor, uint32_t index, Thing* thing)
{

}

void VirtualCylinder::__removeThing(Creature* actor, Thing* thing, uint32_t count)
{

}

void VirtualCylinder::postAddNotification(Creature* actor, Thing* thing, const Cylinder* oldParent, int32_t index, cylinderlink_t link /*= LINK_OWNER*/)
{

}

void VirtualCylinder::postRemoveNotification(Creature* actor, Thing* thing, const Cylinder* newParent, int32_t index, bool isCompleteRemoval,
  cylinderlink_t link /*= LINK_OWNER*/)
{

}

bool VirtualCylinder::isPushable() const
{
  return false;
}

int VirtualCylinder::getThrowRange() const
{
  return 1;
}

std::string VirtualCylinder::getDescription(int32_t lookDistance) const
{
  return "";
}

