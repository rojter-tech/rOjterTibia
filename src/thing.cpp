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

#include "thing.h"
#include "cylinder.h"
#include "tile.h"
#include "position.h"

// Avoid unnecessary includes!
extern void g_gameUnscriptThing(Thing* thing);

Thing::Thing() :
  parent(NULL),
  m_refCount(0)
{
  //
}

Thing::~Thing()
{
  //
  //std::cout << "thing destructor " << this << std::endl;

  // Kind of ugly to put it here, but what choice is there?
  g_gameUnscriptThing(this);
}

void Thing::addRef()
{
  ++m_refCount;
}

void Thing::unRef()
{
  --m_refCount;
  if(m_refCount <= 0)
    delete this;
}

Cylinder* Thing::getParent()
{
  return parent;
}

const Cylinder* Thing::getParent() const
{
  return parent;
}

void Thing::setParent(Cylinder* cylinder)
{
  parent = cylinder;
}

Cylinder* Thing::getTopParent()
{
  //tile
  if(getParent() == NULL)
    return dynamic_cast<Cylinder*>(this);

  Cylinder* aux = getParent();
  Cylinder* prevaux = dynamic_cast<Cylinder*>(this);

  while(aux->getParent() != NULL){
    prevaux = aux;
    aux = aux->getParent();
  }

  if(dynamic_cast<Cylinder*>(prevaux)){
    return prevaux;
  }

  return aux;
}

const Cylinder* Thing::getTopParent() const
{
  //tile
  if(getParent() == NULL)
    return dynamic_cast<const Cylinder*>(this);

  const Cylinder* aux = getParent();
  const Cylinder* prevaux = dynamic_cast<const Cylinder*>(this);

  while(aux->getParent() != NULL){
    prevaux = aux;
    aux = aux->getParent();
  }

  if(dynamic_cast<const Cylinder*>(prevaux)){
    return prevaux;
  }

  return aux;
}

Tile* Thing::getParentTile()
{
  Cylinder* cylinder = getTopParent();

  #ifdef __DEBUG__MOVESYS__
  if(!cylinder){
    std::cout << "Failure: [Thing::getParentTile()],  NULL tile" << std::endl;
    DEBUG_REPORT
    return &(Tile::null_tile);
  }
  #endif

  //get root cylinder
  if(cylinder->getParent())
    cylinder = cylinder->getParent();

  return dynamic_cast<Tile*>(cylinder);
}

const Tile* Thing::getParentTile() const
{
  const Cylinder* cylinder = getTopParent();

  #ifdef __DEBUG__MOVESYS__
  if(!cylinder){
    std::cout << "Failure: [Thing::getParentTile() const],  NULL tile" << std::endl;
    DEBUG_REPORT
    return &(Tile::null_tile);
  }
  #endif

  //get root cylinder
  if(cylinder->getParent())
    cylinder = cylinder->getParent();

  return dynamic_cast<const Tile*>(cylinder);
}

Position Thing::getPosition() const
{
  const Tile* tile = getParentTile();
  if(tile){
    return tile->getPosition();
  }
  else{
    #ifdef __DEBUG__MOVESYS__
    std::cout << "Failure: [Thing::getPosition],  NULL tile" << std::endl;
    DEBUG_REPORT
    #endif
    return Tile::null_tile.getPosition();
  }
}

Item* Thing::getItem()
{
  return NULL;
}

const Item* Thing::getItem() const
{
  return NULL;
}

Tile* Thing::getTile()
{
  return NULL;
}

const Tile* Thing::getTile() const
{
  return NULL;
}

Creature* Thing::getCreature()
{
  return NULL;
}

const Creature* Thing::getCreature() const
{
  return NULL;
}

bool Thing::isRemoved() const
{
  if(parent == NULL)
    return true;

  const Cylinder* aux = getParent();
  if(aux->isRemoved())
    return true;

  return false;
}
