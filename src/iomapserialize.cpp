//////////////////////////////////////////////////////////////////////
// OpenTibia - an opensource roleplaying game
//////////////////////////////////////////////////////////////////////
// Base class for the map serialization
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

#include "iomapserialize.h"
#include "otsystem.h"
#include "player.h"
#include "ioplayer.h"
#include "configmanager.h"
#include "game.h"
#include "house.h"
#include "depot.h"
#include "housetile.h"
#include "singleton.h"

extern ConfigManager g_config;
extern Game g_game;

IOMapSerialize* IOMapSerialize::getInstance()
{
  static Singleton<IOMapSerialize> instance;
  return instance.get();
}

bool IOMapSerialize::loadMap(Map* map)
{
  int64_t start = OTSYS_TIME();
  bool s = false;

  if(g_config.getString(ConfigManager::MAP_STORAGE_TYPE) == "relational")
    s = loadMapRelational(map);
  else if(g_config.getString(ConfigManager::MAP_STORAGE_TYPE) == "binary")
    s = loadMapBinary(map);
  else
    std::cout << "[IOMapSerialize::loadMap] Unknown map storage type" << std::endl;

  std::cout << "Notice: Map load (" << g_config.getString(ConfigManager::MAP_STORAGE_TYPE) << ") took : " <<
    (OTSYS_TIME() - start)/(1000.) << " s" << std::endl;

  return s;
}

bool IOMapSerialize::saveMap(Map* map)
{
  bool s = false;

  if(g_config.getString(ConfigManager::MAP_STORAGE_TYPE) == "relational")
    s = saveMapRelational(map);
  else if(g_config.getString(ConfigManager::MAP_STORAGE_TYPE) == "binary")
    s = saveMapBinary(map);
  else
    std::cout << "[IOMapSerialize::saveMap] Unknown map storage type" << std::endl;

  return s;
}

bool IOMapSerialize::loadMapRelational(Map* map)
{
  /*
  DatabaseDriver* db = DatabaseDriver::instance();
  DBQuery query;

  for(HouseMap::iterator it = Houses::getInstance()->getHouseBegin(); it != Houses::getInstance()->getHouseEnd(); ++it){
    House* house = it->second;

    query.str("");
    query << "SELECT * FROM `tiles` "
      "LEFT JOIN `houses` ON `tiles`.`house_id` "
      "WHERE `tiles`.`world_id` = " << g_config.getNumber(ConfigManager::WORLD_ID) << " AND `houses`.`map_id` = " << house->getHouseId();
    DBResult* result = db->storeQuery(query.str());

    if(result){
      do{
        int32_t tileId = result->getDataInt("id");

        query.str("");
        query << "SELECT * FROM `tile_items` WHERE `tile_id` = " << tileId << " ORDER BY `sid` DESC";

        DBResult* result_items = db->storeQuery(query.str());
        if(result_items){
          if(house->getPendingDepotTransfer()){
            Player* player = g_game.getPlayerByGuidEx(house->getHouseOwner());
            if(player){
              Depot* depot = player->getDepot(player->getTown(), true);

              loadItems(db, result_items, depot, true);

              if(player->isOffline()){
                IOPlayer::instance()->savePlayer(player);
                delete player;
              }
            }
          }
          else{
            int32_t x = result->getDataInt("x");
            int32_t y = result->getDataInt("y");
            int32_t z = result->getDataInt("z");

            Tile* tile = map->getParentTile(x, y, z);
            if(!tile){
              std::cout << "ERROR: Unserialization of invalid tile in IOMapSerialize::loadMapRelational() - " << Position(x, y,z)  << std::endl;
              continue;
            }

            loadItems(db, result_items, tile);
          }

          db->freeResult(result_items);
        }

      }while(result->next());

      db->freeResult(result);
    }
    else{
      //backward compatibility
      for(HouseTileList::iterator it = house->getTileBegin(); it != house->getTileEnd(); ++it){
        const Position& tilePos = (*it)->getPosition();

        query.str("");
        query << "SELECT `id` FROM `tiles` WHERE `world_id` = " << g_config.getNumber(ConfigManager::WORLD_ID)
          << " AND `x` = " << tilePos.x
          << " AND `y` = " << tilePos.y
          << " AND `z` = " << tilePos.z;

        DBResult* result = db->storeQuery(query.str());

        if(result){
          int32_t tileId = result->getDataInt("id");

          query.str("");
          query << "SELECT * FROM `tile_items` WHERE `tile_id` = " << tileId << " ORDER BY `sid` DESC";

          DBResult* result_items = db->storeQuery(query.str());
          if(result_items){
            if(house->getPendingDepotTransfer()){
              Player* player = g_game.getPlayerByGuidEx(house->getHouseOwner());
              if(player){
                Depot* depot = player->getDepot(player->getTown(), true);

                loadItems(db, result_items, depot);

                if(player->isOffline()){
                  IOPlayer::instance()->savePlayer(player);
                  delete player;
                }
              }
            }
            else{
              loadItems(db, result_items, (*it));
            }

            db->freeResult(result_items);
          }

          db->freeResult(result);
        }
      }
    }
  }
  */
  return true;
}

bool IOMapSerialize::loadItems(DatabaseDriver* db, DBResult* result, Cylinder* parent, bool depotTransfer /*= false*/)
{
  /*
  typedef std::map<int32_t,std::pair<Item*,int32_t> > ItemMap;
  ItemMap itemMap;

  Item* item = NULL;
  Tile* tile = NULL;
  if(!parent->getItem()){
    tile = parent->getParentTile();
  }

  do{
    int32_t sid = result->getDataInt("sid");
    int32_t pid = result->getDataInt("pid");
    int32_t id = result->getDataInt("itemtype");
    int32_t count = result->getDataInt("count");
    item = NULL;

    unsigned long attrSize = 0;
    const char* attr = result->getDataStream("attributes", attrSize);
    PropStream propStream;
    propStream.init(attr, attrSize);

    const ItemType& iType = Item::items[id];
    if(iType.moveable || pid != 0){
      //create a new item
      item = Item::CreateItem(id, count);

      if(item){
        if(item->unserializeAttr(propStream)){
          if(pid == 0){
            parent->__internalAddThing(item);
            g_game.startDecay(item);
          }
        }
        else{
          std::cout << "WARNING: Serialize error in IOMapSerialize::loadTile()" << std::endl;
        }
      }
      else
        continue;
    }
    else{
      if(tile){
        item = tile->items_getItemWithItemId(id);
        if(!item && iType.type != ITEM_TYPE_NONE){
          item = tile->items_getItemWithType(iType.type);
        }
      }
    }

    if(item){
      if(item->unserializeAttr(propStream)){
        item = g_game.transformItem(NULL, item, id);
        if(item){
          std::pair<Item*, int32_t> myPair(item, pid);
          itemMap[sid] = myPair;
        }
      }
      else{
        std::cout << "WARNING: Serialize error in IOMapSerialize::loadItems()" << std::endl;
      }
    }
    else{
      //The map changed since the last save, just read the attributes
      Item* dummy = Item::CreateItem(id);
      if(dummy){
        dummy->unserializeAttr(propStream);

        if(depotTransfer){
          //simply swap dummy with parent
          std::pair<Item*, int32_t> myPair(parent->getItem(), pid);
          itemMap[sid] = myPair;
        }
        else{
          std::cout << "WARNING: IOMapSerialize::loadTile(). NULL item at " << tile->getPosition() << ". id = " << id << ", sid = " << sid << ", pid = " << pid << std::endl;
        }

        delete dummy;
      }
    }
  }while(result->next());

  ItemMap::reverse_iterator it;
  ItemMap::iterator it2;

  for(it = itemMap.rbegin(); it != itemMap.rend(); ++it){
    Item* item = it->second.first;
    int pid = it->second.second;

    it2 = itemMap.find(pid);
    if(it2 != itemMap.end()){
      if(Container* container = it2->second.first->getContainer()){
        container->__internalAddThing(item);
        g_game.startDecay(item);
      }
    }
  }
  */
  return true;
}

bool IOMapSerialize::saveMapRelational(Map* map)
{
  /*
  Database* db = Database::instance();
  DBQuery query;
  DBTransaction transaction(db);

  //Start the transaction
  if(!transaction.begin())
    return false;

  //clear old tile data
  query << "DELETE FROM `tiles` WHERE `world_id` = " << g_config.getNumber(ConfigManager::WORLD_ID);
  if(!db->executeQuery(query.str())){
    return false;
  }
  query.str("");

  for(HouseMap::iterator it = Houses::getInstance()->getHouseBegin();
    it != Houses::getInstance()->getHouseEnd(); ++it){

    //save house items
    House* house = it->second;
    for(HouseTileList::iterator it = house->getTileBegin(); it != house->getTileEnd(); ++it){
      if(!saveItems(db, house->getHouseId(), *it)){
        return false;
      }
    }
  }

  //End the transaction
  return transaction.commit();
  */
  return true;
}

bool IOMapSerialize::saveItems(DatabaseDriver* db, uint32_t houseId, const Tile* tile)
{
  /*
  typedef std::list<std::pair<Container*, int32_t> > ContainerStackList;
  typedef ContainerStackList::value_type ContainerStackList_Pair;
  ContainerStackList containerStackList;

  uint32_t tileId = 0;
  int runningID = 0;
  Item* item = NULL;
  Container* container = NULL;

  int parentid = 0;
  DBQuery query;

  DBInsert stmt(db);

  stmt.setQuery("INSERT INTO `tile_items` (`tile_id`, `sid`, `pid`, `itemtype`, `count`, `attributes`) VALUES ");

  for(uint32_t i = 0; i < tile->getThingCount(); ++i){
    item = tile->__getThing(i)->getItem();

    if(!item)
      continue;

    if(!(item->isMoveable() ||
      item->getDoor() ||
      (item->getContainer() && item->getContainer()->size() != 0)||
      item->canWriteText() ||
      item->getBed() ) )
      continue;

    if(tileId == 0){
      const Position& tilePos = tile->getPosition();
      query << "INSERT INTO `tiles` (`world_id`, `house_id`, `x`, `y`, `z`) VALUES ("
      << ", " << g_config.getNumber(ConfigManager::WORLD_ID) << ", " << houseId << ", "
      << tilePos.x << ", " << tilePos.y << ", " << tilePos.z << ")";

      if(!db->executeQuery(query.str()))
        return false;

      tileId = (uint32_t)db->getLastInsertedRowID();
      query.str("");
    }
    ++runningID;

    uint32_t attributesSize;

    PropWriteStream propWriteStream;
    item->serializeAttr(propWriteStream);
    const char* attributes = propWriteStream.getStream(attributesSize);

    query << tileId << ", " << runningID << ", " << parentid << ", " << item->getID() << ", " << (int32_t)item->getSubType() << ", " << db->escapeBlob(attributes, attributesSize);

    if(!stmt.addRow(query))
      return false;

    if(item->getContainer())
      containerStackList.push_back(ContainerStackList_Pair(item->getContainer(), runningID));
  }

  if(tileId == 0)
    return false;

  while(containerStackList.size() > 0){
    ContainerStackList_Pair csPair = containerStackList.front();
    container = csPair.first;
    parentid = csPair.second;
    containerStackList.pop_front();

    for(ItemList::const_iterator it = container->getItems(); it != container->getEnd(); ++it){
      item = (*it);
      ++runningID;
      if(item->getContainer())
        containerStackList.push_back(ContainerStackList_Pair(item->getContainer(), runningID));

      uint32_t attributesSize;

      PropWriteStream propWriteStream;
      item->serializeAttr(propWriteStream);
      const char* attributes = propWriteStream.getStream(attributesSize);

      query << tileId << ", " << runningID << ", " << parentid << ", " << item->getID() << ", " << (int32_t)item->getSubType() << ", " << db->escapeBlob(attributes, attributesSize);

      if(!stmt.addRow(query))
        return false;
    }
  }

  if(!stmt.execute())
    return false;
    */
  return true;
}

bool IOMapSerialize::loadMapBinary(Map* map)
{
  DatabaseDriver* db = DatabaseDriver::instance();
  DBQuery query;
  DBResult_ptr result;

  query << "SELECT * FROM `map_store` WHERE `world_id` = " << g_config.getNumber(ConfigManager::WORLD_ID);
  for (result = db->storeQuery(query); result; result = result->advance()){
    int32_t houseid = result->getDataInt("house_id");
    House* house = Houses::getInstance()->getHouse(houseid);

    unsigned long attrSize = 0;
    const char* attr = result->getDataStream("data", attrSize);
    PropStream propStream;
    propStream.init(attr, attrSize);

    while(propStream.size()) {
      uint32_t item_count = 0;
      uint16_t x = 0, y = 0;
      uint8_t z = 0;

      propStream.GET_USHORT(x);
      propStream.GET_USHORT(y);
      propStream.GET_UCHAR(z);

      if(house && house->getPendingDepotTransfer()){
        Player* player = g_game.getPlayerByGuidEx(house->getHouseOwner());
        if(player){
          Depot* depot = player->getDepot(player->getTown(), true);

          propStream.GET_ULONG(item_count);
          while(item_count--){
            loadItem(propStream, depot, true);
          }

          if(player->isOffline()){
            IOPlayer::instance()->savePlayer(player);
            delete player;
          }
        }
      }
      else{
        Tile* tile = map->getParentTile(x, y, z);
        if(!tile){
          std::cout << "ERROR: Unserialization of invalid tile in IOMapSerialize::loadTile()" << std::endl;
          break;
        }

        propStream.GET_ULONG(item_count);
        while(item_count--){
          loadItem(propStream, tile);
        }
      }
    }
  }

  return true;
}

bool IOMapSerialize::loadContainer(PropStream& propStream, Container* container)
{
  while(container->serializationCount > 0){
    if(!loadItem(propStream, container)){
      std::cout << "WARNING: Unserialization error for containing item in IOMapSerialize::loadContainer() - " << container->getID() << std::endl;
      return false;
    }
    container->serializationCount--;
  }

  uint8_t endAttr = 0;
  propStream.GET_UCHAR(endAttr);

  if(endAttr != 0x00){
    std::cout << "WARNING: Unserialization error for containing item in IOMapSerialize::loadContainer() - " << container->getID() << std::endl;
    return false;
  }

  return true;
}

bool IOMapSerialize::loadItem(PropStream& propStream, Cylinder* parent, bool depotTransfer /*= false*/)
{
  Item* item = NULL;

  uint16_t id = 0;
  propStream.GET_USHORT(id);

  const ItemType& iType = Item::items[id];

  Tile* tile = NULL;
  if(!parent->getItem()){
    tile = parent->getParentTile();
  }

  bool isInContainer = (!depotTransfer && !tile);

  if(iType.moveable || /* or object in a container*/ isInContainer){
    //create a new item
    item = Item::CreateItem(id);

    if(item){
      if(item->unserializeAttr(propStream)){
        Container* container = item->getContainer();
        if(container){
          if(!loadContainer(propStream, container)){
            delete item;
            return false;
          }
        }

        if(parent){
          parent->__internalAddThing(item);
          g_game.startDecay(item);
        }
        else{
          delete item;
        }
      }
      else{
        std::cout << "WARNING: Unserialization error in IOMapSerialize::loadItem()" << id << std::endl;
        delete item;
        return false;
      }
    }
  }
  else{
    if(tile){
      // Stationary items like doors/beds/blackboards/bookcases
      item = tile->items_getItemWithItemId(id);
      if(!item && iType.type != ITEM_TYPE_NONE){
        item = tile->items_getItemWithType(iType.type);
      }
    }

    if(item){
      if(item->unserializeAttr(propStream)){
        Container* container = item->getContainer();
        if(container){
          if(!loadContainer(propStream, container)){
            return false;
          }
        }

        item = g_game.transformItem(NULL, item, id);
      }
      else{
        std::cout << "WARNING: Unserialization error in IOMapSerialize::loadItem()" << id << std::endl;
      }
    }
    else{
      //The map changed since the last save, just read the attributes
      Item* dummy = Item::CreateItem(id);
      if(dummy){
        dummy->unserializeAttr(propStream);
        Container* container = dummy->getContainer();
        if(container){
          if(!loadContainer(propStream, container)){
            delete dummy;
            return false;
          }

          if(depotTransfer){
            for(ItemList::const_iterator it = container->getItems(); it != container->getEnd(); ++it){
              parent->__addThing(NULL, *it);
            }

            container->itemlist.clear();
            delete dummy;
            return true;
          }
        }

        delete dummy;
      }
    }
  }

  return true;
}

bool IOMapSerialize::saveMapBinary(Map* map)
{
  DatabaseDriver* db = DatabaseDriver::instance();
  DBQuery query;
  DBTransaction transaction(db);
  DBInsert stmt(db);
  stmt.setQuery("INSERT INTO `map_store` (`world_id`, `house_id`, `data`) VALUES ");


  //Start the transaction
  if(!transaction.begin())
    return false;

  query << "DELETE FROM `map_store` WHERE `world_id` = " << g_config.getNumber(ConfigManager::WORLD_ID);
  if(!db->executeQuery(query))
    return false;

  //clear old tile data
  for(HouseMap::iterator it = Houses::getInstance()->getHouseBegin();
    it != Houses::getInstance()->getHouseEnd();
    ++it)
  {
    //save house items
    House* house = it->second;
    PropWriteStream stream;
    for(HouseTileList::iterator tile_iter = house->getTileBegin();
      tile_iter != house->getTileEnd();
      ++tile_iter)
    {
      if(!saveTile(stream, *tile_iter)){
        return false;
      }
    }

    uint32_t attributesSize;
    const char* attributes = stream.getStream(attributesSize);

    query.reset();
    query << g_config.getNumber(ConfigManager::WORLD_ID) << ", " << it->second->getHouseId() << ", " << db->escapeBlob(attributes, attributesSize);

    if(!stmt.addRow(query.str()))
      return false;
  }

  if(!stmt.execute())
    return false;

  //End the transaction
  return transaction.commit();
}

bool IOMapSerialize::saveItem(PropWriteStream& stream, const Item* item)
{
  const Container* container = item->getContainer();

  // Write ID & props
  stream.ADD_USHORT(item->getID());
  item->serializeAttr(stream);

  if(container){
    // Hack our way into the attributes
    stream.ADD_UCHAR(ATTR_CONTAINER_ITEMS);
    stream.ADD_ULONG(container->size());
    for(ItemList::const_reverse_iterator i = container->getReversedItems(); i != container->getReversedEnd(); ++i){
      saveItem(stream, *i);
    }
  }

  stream.ADD_UCHAR(0x00); // attr end

  return true;
}

bool IOMapSerialize::saveTile(PropWriteStream& stream, const Tile* tile)
{
  std::vector<Item*> items;
  for(int32_t i = tile->getThingCount(); i > 0; --i)
  {
    Item* item = tile->__getThing(i - 1)->getItem();
    if(!item)
      continue;

    // Note that these are NEGATED, ie. these are the items that will be saved.
    if(!(  item->isMoveable() ||
        item->getDoor() ||
        (item->getContainer() && item->getContainer()->size() != 0) ||
        item->canWriteText() ||
        item->getBed()))
      continue;

    items.push_back(item);
  }

  if(items.size() > 0) {
    stream.ADD_USHORT(tile->getPosition().x);
    stream.ADD_USHORT(tile->getPosition().y);
    stream.ADD_UCHAR(tile->getPosition().z);
    stream.ADD_ULONG(items.size());

    for(std::vector<Item*>::iterator iter = items.begin();
      iter != items.end();
      ++iter)
    {
      saveItem(stream, *iter);
    }
  }

  return true;
}

bool IOMapSerialize::updateHouseInfo()
{
  DatabaseDriver* db = DatabaseDriver::instance();
  DBQuery query;

  for(HouseMap::iterator it = Houses::getInstance()->getHouseBegin(); it != Houses::getInstance()->getHouseEnd(); ++it){
    House* house = it->second;

    query.reset();
    query << "SELECT * FROM `houses` WHERE `world_id` = " << g_config.getNumber(ConfigManager::WORLD_ID) << " AND `map_id` = " << house->getHouseId();
    DBResult_ptr result(db->storeQuery(query));
    if(result){

      query.reset();
      query << "UPDATE `houses` SET ";

      if(house->hasSyncFlag(House::HOUSE_SYNC_TOWNID)){
        query << "`town_id` = " << house->getTownId() << ", ";
      }

      if(house->hasSyncFlag(House::HOUSE_SYNC_NAME)){
        query << "`name` = " << db->escapeString(house->getName()) << ", ";
      }

      if(house->hasSyncFlag(House::HOUSE_SYNC_RENT)){
        query << "`rent` = " << house->getRent() << ", ";
      }

      if(house->hasSyncFlag(House::HOUSE_SYNC_GUILDHALL)){
        query << "`guildhall` = " << (house->isGuildHall() ? 1 : 0) << ", ";
      }

      query << "`tiles` = " << house->getTileCount() << ", ";
      query << "`beds` = " << house->getBedCount() << ", ";
      query << "`doors` = " << house->getDoorCount();

      query << " WHERE `world_id` = " << g_config.getNumber(ConfigManager::WORLD_ID) << " AND `map_id` = " << house->getHouseId();
    }
    else{
      query.reset();
      query << "INSERT INTO `houses` (`world_id`, `map_id`, `town_id`, `name`, `rent`, `guildhall`, `tiles`, `beds`, `doors`)" << "VALUES ("
        << g_config.getNumber(ConfigManager::WORLD_ID) << ", "
        << house->getHouseId() << ", "
        << house->getTownId() << ", "
        << db->escapeString(house->getName()) << ", "
        << house->getRent() << ", "
        << (house->isGuildHall() ? 1 : 0) << ", "
        << house->getTileCount() << ", "
        << house->getBedCount() << ", "
        << house->getDoorCount() << ")";
    }

    if(!db->executeQuery(query)){
      return false;
    }
  }

  return true;
}

bool IOMapSerialize::processHouseAuctions()
{
  DatabaseDriver* db = DatabaseDriver::instance();
  DBQuery query;
  DBResult_ptr result_set;

  time_t currentTime = std::time(NULL);
  query <<
    "SELECT `house_auctions`.* "
    "FROM `house_auctions` "
    "LEFT JOIN `houses` ON `houses`.`id` = `house_auctions`.`house_id` "
    "WHERE `endtime` <" << currentTime << " AND `world_id` = " << g_config.getNumber(ConfigManager::WORLD_ID);

  bool success = true;
  for (result_set = db->storeQuery(query); result_set; result_set->advance()){
    int32_t houseid = result_set->getDataInt("house_id");
    int32_t playerid = result_set->getDataInt("player_id");

    House* house = Houses::getInstance()->getHouse(houseid);
    if(!house){
      success = false;
      continue;
    }

    house->setHouseOwner(playerid);
    Houses::getInstance()->payHouse(house, currentTime);

    query.reset();
    query << "DELETE * FROM `house_auctions` WHERE `house_id` =" << houseid;
    db->executeQuery(query);
  }

  return success;
}

bool IOMapSerialize::loadHouseInfo(Map* map)
{
  DatabaseDriver* db = DatabaseDriver::instance();
  DBQuery query;
  DBResult_ptr result;

  query << "SELECT * FROM `houses` WHERE `world_id` = " << g_config.getNumber(ConfigManager::WORLD_ID);
  for(result = db->storeQuery(query); result; result = result->advance()){
    int32_t houseid = result->getDataInt("map_id");
    House* house = Houses::getInstance()->getHouse(houseid);
    if(house){
      int32_t ownerid = result->getDataInt("owner_id");
      int32_t paid = result->getDataInt("paid");
      int32_t payRentWarnings = result->getDataInt("warnings");
      uint32_t lastWarning = result->getDataInt("lastwarning");
      bool clear = (result->getDataInt("clear") != 0);

      house->setHouseOwner(ownerid);
      house->setPaidUntil(paid);
      house->setPayRentWarnings(payRentWarnings);
      house->setLastWarning(lastWarning);

      if(clear){
        house->setPendingDepotTransfer(true);
      }
    }
  }

  for(HouseMap::iterator it = Houses::getInstance()->getHouseBegin(); it != Houses::getInstance()->getHouseEnd(); ++it){
    House* house = it->second;
    if(house->getHouseOwner() != 0 && house->getHouseId() != 0){
      query.reset();
      query <<
        "SELECT `listid`, `list` "
        "FROM `house_lists` "
        "LEFT JOIN `houses` ON `house_lists`.`house_id` = `houses`.`id` "
        "WHERE `world_id` = " << g_config.getNumber(ConfigManager::WORLD_ID) << " AND `house_id` = " << house->getHouseId();

      for(result = db->storeQuery(query); result; result = result->advance()){
        int32_t listid = result->getDataInt("listid");
        std::string list = result->getDataString("list");
        house->setAccessList(listid, list);
      }
    }
  }

  return true;
}

bool IOMapSerialize::saveHouseInfo(Map* map)
{
  DatabaseDriver* db = DatabaseDriver::instance();
  DBQuery query;
  DBTransaction transaction(db);

  if(!transaction.begin())
    return false;

  query.reset();
  query <<
    "DELETE FROM `house_lists` "
    "WHERE `world_id` = " << g_config.getNumber(ConfigManager::WORLD_ID);
  if(!db->executeQuery(query)) {
    return false;
  }

  DBInsert houselist_insert(db);
  houselist_insert.setQuery("INSERT INTO `house_lists` (`house_id`, `listid`, `list`) VALUES ");

  for(HouseMap::iterator it = Houses::getInstance()->getHouseBegin(); it != Houses::getInstance()->getHouseEnd(); ++it){
    House* house = it->second;

    // Fetch house GUID
    DBResult_ptr fetch_guid;
    query.reset();
    query << "SELECT `id` "
      "FROM `houses` "
      "WHERE `world_id` = " << g_config.getNumber(ConfigManager::WORLD_ID) << " AND `map_id` = " << house->getHouseId();

    if(!(fetch_guid = db->storeQuery(query)))
      return false;

    uint32_t house_guid = fetch_guid->getDataUInt("id");

    // Update house stats
    query.reset();
    query << "UPDATE `houses` SET "
      << "`owner` = " << house->getHouseOwner() << ", "
      << "`paid` = " << house->getPaidUntil() << ", "
      << "`warnings` = " << house->getPayRentWarnings() << ", "
      << "`lastwarning` = " << house->getLastWarning() << ", "
      << "`clear` = " << 0
      << "WHERE `id` = " << house_guid;

    if(!db->executeQuery(query)){
      return false;
    }

    // Update house list
    std::string listText;
    if(house->getAccessList(GUEST_LIST, listText) && listText != ""){
      query << house_guid << ", " << GUEST_LIST << ", " << db->escapeString(listText);

      if(!houselist_insert.addRowAndReset(query)){
        return false;
      }
    }
    if(house->getAccessList(SUBOWNER_LIST, listText) && listText != ""){
      query << house_guid << ", " << SUBOWNER_LIST << ", " << db->escapeString(listText);

      if(!houselist_insert.addRowAndReset(query)){
        return false;
      }
    }

    for(HouseDoorList::iterator it = house->getDoorBegin(); it != house->getDoorEnd(); ++it){
      const Door* door = *it;
      if(door->getAccessList(listText) && listText != ""){
        query << house_guid << ", " << door->getDoorId() << ", " << db->escapeString(listText);

        if(!houselist_insert.addRowAndReset(query)){
          return false;
        }
      }
    }
  }

  if(!houselist_insert.execute()){
    return false;
  }

  return transaction.commit();
}

