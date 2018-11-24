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

#ifndef __OTSERV_CONTAINER_H__
#define __OTSERV_CONTAINER_H__

#include <queue>
#include <iosfwd>
#include "cylinder.h"
#include "item.h"

class ContainerIterator
{
public:
  ContainerIterator();
  ContainerIterator(const ContainerIterator& rhs);
  ~ContainerIterator();

  ContainerIterator& operator=(const ContainerIterator& rhs);
  bool operator==(const ContainerIterator& rhs);
  bool operator!=(const ContainerIterator& rhs);
  ContainerIterator& operator++();
  ContainerIterator operator++(int);
  Item* operator*();
  Item* operator->();

protected:
  ContainerIterator(Container* super);

  Container* super;
  std::queue<Container*> over;
  ItemList::iterator cur;

  friend class Container;
};

class Container : public Item, public Cylinder
{
public:
  Container(uint16_t _type);
  virtual ~Container();
  virtual Item* clone() const;

  virtual Container* getContainer();
  virtual const Container* getContainer() const;
  virtual Depot* getDepot();
  virtual const Depot* getDepot() const;

  Attr_ReadValue readAttr(AttrTypes_t attr, PropStream& propStream);
  bool unserializeItemNode(FileLoader& f, NodeStruct* node, PropStream& propStream);
  std::string getContentDescription() const;

  uint32_t size() const;
  bool empty() const;

  ContainerIterator begin();
  ContainerIterator end();
  ContainerIterator begin() const;
  ContainerIterator end() const;

  ItemList::const_iterator getItems() const;
  ItemList::const_iterator getEnd() const;
  ItemList::const_reverse_iterator getReversedItems() const;
  ItemList::const_reverse_iterator getReversedEnd() const;

  void addItem(Item* item);
  Item* getItem(uint32_t index);
  bool isHoldingItem(const Item* item) const;

  uint32_t capacity() const;
  uint32_t getItemHoldingCount() const;
  virtual double getWeight() const;

  //cylinder implementations
  virtual Cylinder* getParent();
  virtual const Cylinder* getParent() const;
  virtual bool isRemoved() const;
  virtual Position getPosition() const;
  virtual Tile* getTile();
  virtual const Tile* getTile() const;
  virtual Item* getItem();
  virtual const Item* getItem() const;
  virtual Creature* getCreature();
  virtual const Creature* getCreature() const;
  virtual Tile* getParentTile();
  virtual const Tile* getParentTile() const;
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
  virtual uint32_t __getItemTypeCount(uint16_t itemId, int32_t subType = -1) const;
  virtual std::map<uint32_t, uint32_t>& __getAllItemTypeCount(std::map<uint32_t, uint32_t>& countMap) const;
  virtual Thing* __getThing(uint32_t index) const;

  virtual void postAddNotification(Creature* actor, Thing* thing, const Cylinder* oldParent, int32_t index, cylinderlink_t link = LINK_OWNER);
  virtual void postRemoveNotification(Creature* actor, Thing* thing, const Cylinder* newParent, int32_t index, bool isCompleteRemoval, cylinderlink_t link = LINK_OWNER);

  virtual void __internalAddThing(Thing* thing);
  virtual void __internalAddThing(uint32_t index, Thing* thing);
private:
  void onAddContainerItem(Item* item);
  void onUpdateContainerItem(uint32_t index, Item* oldItem, const ItemType& oldType,
    Item* newItem, const ItemType& newType);
  void onRemoveContainerItem(uint32_t index, Item* item);

  Container* getParentContainer();
  void updateItemWeight(double diff);

protected:
  std::ostringstream& getContentDescription(std::ostringstream& os) const;

  uint32_t maxSize;
  double total_weight;
  ItemList itemlist;
  uint32_t serializationCount;

  friend class ContainerIterator;
  friend class IOMapSerialize;
};

#endif
