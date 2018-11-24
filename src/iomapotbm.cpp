//////////////////////////////////////////////////////////////////////
// OpenTibia - an opensource roleplaying game
//////////////////////////////////////////////////////////////////////
// OTBM map loader
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

#include "iomapotbm.h"
#include "otsystem.h"
#include "game.h"
#include "map.h"
#include "tile.h"
#include "housetile.h"
#include "town.h"

typedef uint8_t attribute_t;
typedef uint32_t flags_t;

extern Game g_game;

/*
  OTBM_ROOTV1
  |
  |--- OTBM_MAP_DATA
  |  |
  |  |--- OTBM_TILE_AREA
  |  |  |--- OTBM_TILE
  |  |  |--- OTBM_TILE_SQUARE (not implemented)
  |  |  |--- OTBM_TILE_REF (not implemented)
  |  |  |--- OTBM_HOUSETILE
  |  |
  |  |--- OTBM_SPAWNS (not implemented)
  |  |  |--- OTBM_SPAWN_AREA (not implemented)
  |  |  |--- OTBM_MONSTER (not implemented)
  |  |
  |  |--- OTBM_TOWNS
  |    |--- OTBM_TOWN
  |
  |--- OTBM_ITEM_DEF (not implemented)
*/

Tile* IOMapOTBM::createTile(Item*& ground, Item* item, const Position& p)
{
  Tile* tile;
  if(ground){
    if((item && item->blockSolid()) || ground->blockSolid()){
      // Tile is blocking with possibly some decoration, should be static
      tile = new StaticTile(p.x, p.y, p.z);
    }
    else{
      // Tile is not blocking with possibly multiple items, use dynamic
      tile = new DynamicTile(p.x, p.y, p.z);
    }
    
    tile->__internalAddThing(ground);
    g_game.startDecay(ground);
    ground = NULL;
  }
  else{
    // No ground on this tile, so it will always block
    tile = new StaticTile(p.x, p.y, p.z);
  }
  return tile;
}

bool IOMapOTBM::loadMap(Map* map, const std::string& identifier)
{
  int64_t start = OTSYS_TIME();

  FileLoader f;
  if(!f.openFile(identifier.c_str(), false, true)){
    std::stringstream ss;
    ss << "Could not open the file " << identifier << ".";
    setLastErrorString(ss.str());
    return false;
  }

  unsigned long type;
  PropStream propStream;

  NodeStruct* root = f.getChildNode(NULL, type);

  if(!f.getProps(root, propStream)){
    setLastErrorString("Could not read root property.");
    return false;
  }

  OTBM_root_header* root_header;
  if(!propStream.GET_STRUCT(root_header)){
    setLastErrorString("Could not read header.");
    return false;
  }

  int header_version = root_header->version;
  if(header_version <= 0){
    //In otbm version 1 the count variable after splashes/fluidcontainers and stackables
    //are saved as attributes instead, this solves alot of problems with items
    //that is changed (stackable/charges/fluidcontainer/splash) during an update.
    setLastErrorString("This map needs to be upgraded by using the latest map editor version to be able to load correctly.");
    return false;
  }

  if(header_version > OTBM_4){
    setLastErrorString("Unknown OTBM version detected, please update your server.");
    return false;
  }

  if(root_header->majorVersionItems < 3){
    setLastErrorString("This map needs to be upgraded by using the latest map editor version to be able to load correctly.");
    return false;
  }

  if(root_header->majorVersionItems > (unsigned long)Items::dwMajorVersion){
    setLastErrorString("The map was saved with a different items.otb version, an upgraded items.otb is required.");
    return false;
  }

  // Prevent load maps saved with items.otb previous to
  // version 800, because of the change to stackable of
  // itemid 3965
  if(root_header->minorVersionItems < CLIENT_VERSION_810){
    setLastErrorString("This map needs to be updated.");
    return false;
  }

  if(root_header->minorVersionItems > (unsigned long)Items::dwMinorVersion){
    std::cout << "Warning: [OTBM loader] This map needs an updated items.otb." <<std::endl;
  }

  std::cout << "Map size: " << root_header->width << "x" << root_header->height << std::endl;
  map->mapWidth = root_header->width;
  map->mapHeight = root_header->height;

  NodeStruct* nodeMap = f.getChildNode(root, type);

  if(type != OTBM_MAP_DATA){
    setLastErrorString("Could not read data node.");
    return false;
  }

  if(!f.getProps(nodeMap, propStream)){
    setLastErrorString("Could not read map data attributes.");
    return false;
  }

  unsigned char attribute;
  std::string mapDescription;
  std::string tmp;
  while(propStream.GET_UCHAR(attribute)){
    switch(attribute){
    case OTBM_ATTR_DESCRIPTION:
      if(!propStream.GET_STRING(mapDescription)){
        setLastErrorString("Invalid description tag.");
        return false;
      }

      std::cout << "Map description: " << mapDescription << std::endl;
      break;
    case OTBM_ATTR_EXT_SPAWN_FILE:
      if(!propStream.GET_STRING(tmp)){
        setLastErrorString("Invalid spawn tag.");
        return false;
      }

      map->spawnfile = identifier.substr(0, identifier.rfind('/') + 1);
      map->spawnfile += tmp;

      break;
    case OTBM_ATTR_EXT_HOUSE_FILE:
      if(!propStream.GET_STRING(tmp)){
        setLastErrorString("Invalid house tag.");
        return false;
      }

      map->housefile = identifier.substr(0, identifier.rfind('/') + 1);
      map->housefile += tmp;
      break;

    default:
      setLastErrorString("Unknown header node.");
      return false;
      break;
    }
  }

  NodeStruct* nodeMapData = f.getChildNode(nodeMap, type);
  while(nodeMapData != NULL){
    if(f.getError() != ERROR_NONE){
      setLastErrorString("Invalid map node.");
      return false;
    }

    if(type == OTBM_TILE_AREA){
      if(!f.getProps(nodeMapData, propStream)){
        setLastErrorString("Invalid map node.");
        return false;
      }

      OTBM_Tile_area_coords* area_coord;
      if(!propStream.GET_STRUCT(area_coord)){
        setLastErrorString("Invalid map node.");
        return false;
      }

      int base_x, base_y, base_z;
      base_x = area_coord->_x;
      base_y = area_coord->_y;
      base_z = area_coord->_z;

      NodeStruct* nodeTile = f.getChildNode(nodeMapData, type);
      while(nodeTile != NULL){
        if(f.getError() != ERROR_NONE){
          setLastErrorString("Could not read node data.");
          return false;
        }

        if(type == OTBM_TILE || type == OTBM_HOUSETILE){
          if(!f.getProps(nodeTile, propStream)){
            setLastErrorString("Could not read node data.");
            return false;
          }

          OTBM_Tile_coords* tile_coord;
          if(!propStream.GET_STRUCT(tile_coord)){
            setLastErrorString("Could not read tile position.");
            return false;
          }

          Position p;
          p.x = base_x + tile_coord->_x;
          p.y = base_y + tile_coord->_y;
          p.z = base_z;

          bool isHouseTile = false;
          House* house = NULL;
          Tile* tile = NULL;
          Item* ground_item = NULL;
          uint32_t tileflags = enums::TILEPROP_NONE;

          if(type == OTBM_HOUSETILE){
            uint32_t _houseid;
            if(!propStream.GET_ULONG(_houseid)){
              std::stringstream ss;
              ss << p << "Could not read house id.";
              setLastErrorString(ss.str());
              return false;
            }

            house = Houses::getInstance()->getHouse(_houseid, true);
            if(!house){
              std::stringstream ss;
              ss << p << "Could not create house id: " << _houseid;
              setLastErrorString(ss.str());
              return false;
            }

            tile = new HouseTile(p.x, p.y, p.z, house);
            house->addTile(static_cast<HouseTile*>(tile));
            isHouseTile = true;
          }

          //read tile attributes
          unsigned char attribute;
          while(propStream.GET_UCHAR(attribute)){
            switch(attribute){
            case OTBM_ATTR_TILE_FLAGS:
            {
              uint32_t flags;
              if(!propStream.GET_ULONG(flags)){
                std::stringstream ss;
                ss << p << "Failed to read tile flags.";
                setLastErrorString(ss.str());
                return false;
              }

              if((flags & enums::TILEPROP_PROTECTIONZONE) == enums::TILEPROP_PROTECTIONZONE){
                tileflags |= enums::TILEPROP_PROTECTIONZONE;
              }
              else if((flags & enums::TILEPROP_NOPVPZONE) == enums::TILEPROP_NOPVPZONE){
                tileflags |= enums::TILEPROP_NOPVPZONE;
              }
              else if((flags & enums::TILEPROP_PVPZONE) == enums::TILEPROP_PVPZONE){
                tileflags |= enums::TILEPROP_PVPZONE;
              }

              if((flags & enums::TILEPROP_NOLOGOUT) == enums::TILEPROP_NOLOGOUT){
                tileflags |= enums::TILEPROP_NOLOGOUT;
              }

              if((flags & enums::TILEPROP_REFRESH) == enums::TILEPROP_REFRESH){
                if(house){
                  std::cout << "Warning: " << p << " House tile flagged as refreshing!";
                }
                tileflags |= enums::TILEPROP_REFRESH;
              }

              break;
            }

            case OTBM_ATTR_ITEM:
            {
              Item* item = Item::CreateItem(propStream);
              if(!item){
                std::stringstream ss;
                ss << p << "Failed to create item.";
                setLastErrorString(ss.str());
                return false;
              }

              if(isHouseTile && item->isMoveable()){
                std::cout << "Warning: Moveable item at " << p << " in house id = " << house->getHouseId() << ", id = " << item->getID() << std::endl;
                delete item;
                item = NULL;
              }
              else{
                if(tile){
                  tile->__internalAddThing(item);
                  g_game.startDecay(item);
                }
                else if(item->isGroundTile()){
                  if(ground_item)
                    delete ground_item;
                  ground_item = item;
                }
                else{ // !tile
                  tile = createTile(ground_item, item, p);
                  tile->__internalAddThing(item);
                  g_game.startDecay(item);
                }
              }

              break;
            }

            default:
              std::stringstream ss;
              ss << p << "Unknown tile attribute.";
              setLastErrorString(ss.str());
              return false;
              break;
            }
          }

          NodeStruct* nodeItem = f.getChildNode(nodeTile, type);
          while(nodeItem){
            if(type == OTBM_ITEM){

              PropStream propStream;
              f.getProps(nodeItem, propStream);

              Item* item = Item::CreateItem(propStream);
              if(!item){
                std::stringstream ss;
                ss << p << "Failed to create item.";
                setLastErrorString(ss.str());
                return false;
              }

              if(item->unserializeItemNode(f, nodeItem, propStream)){
                if(isHouseTile && item->isMoveable()){
                  std::cout << "Warning: Moveable item at " << p << " in house id = " << house->getHouseId() << ", id= " << item->getID() << std::endl;
                  delete item;
                }
                else{
                  if(tile){
                    tile->__internalAddThing(item);
                    g_game.startDecay(item);
                  }
                  else if(item->isGroundTile()){
                    if(ground_item)
                      delete ground_item;
                    ground_item = item;
                  }
                  else{ // !tile
                    tile = createTile(ground_item, item, p);
                    tile->__internalAddThing(item);
                    g_game.startDecay(item);
                  }
                }
              }
              else{
                std::stringstream ss;
                ss << p << "Failed to load item " << item->getID() << ".";
                setLastErrorString(ss.str());
                delete item;
                return false;
              }
            }
            else{
              std::stringstream ss;
              ss << p << "Unknown node type.";
              setLastErrorString(ss.str());
            }

            nodeItem = f.getNextNode(nodeItem, type);
          }

          if(!tile)
            tile = createTile(ground_item, NULL, p);

          tile->setFlag((TileProp)tileflags);

          map->setTile(p, tile);
        }
        else{
          setLastErrorString("Unknown tile node.");
          return false;
        }

        nodeTile = f.getNextNode(nodeTile, type);
      }
    }
    else if(type == OTBM_TOWNS){
      NodeStruct* nodeTown = f.getChildNode(nodeMapData, type);
      while(nodeTown != NULL){
        if(type == OTBM_TOWN){
          if(!f.getProps(nodeTown, propStream)){
            setLastErrorString("Could not read town data.");
            return false;
          }

          uint32_t townid = 0;
          if(!propStream.GET_ULONG(townid)){
            setLastErrorString("Could not read town id.");
            return false;
          }

          Town* town = Towns::getInstance()->getTown(townid);
          if(!town){
            town = new Town(townid);
            Towns::getInstance()->addTown(townid, town);
          }

          std::string townName = "";
          if(!propStream.GET_STRING(townName)){
            setLastErrorString("Could not read town name.");
            return false;
          }

          town->setName(townName);

          OTBM_TownTemple_coords *town_coords;
          if(!propStream.GET_STRUCT(town_coords)){
            setLastErrorString("Could not read town coordinates.");
            return false;
          }

          Position pos;
          pos.x = town_coords->_x;
          pos.y = town_coords->_y;
          pos.z = town_coords->_z;
          town->setTemplePos(pos);
        }
        else{
          setLastErrorString("Unknown town node.");
          return false;
        }

        nodeTown = f.getNextNode(nodeTown, type);
      }
    }
    else if(type == OTBM_WAYPOINTS && header_version >= 2){
      NodeStruct* nodeWaypoint = f.getChildNode(nodeMapData, type);
      while(nodeWaypoint != NULL){
        if(type == OTBM_WAYPOINT){
          if(!f.getProps(nodeWaypoint, propStream)){
            setLastErrorString("Could not read waypoint data.");
            return false;
          }

          std::string name;
          Position pos;

          if(!propStream.GET_STRING(name)){
            setLastErrorString("Could not read waypoint name.");
            return false;
          }

          OTBM_TownTemple_coords* wp_coords;
          if(!propStream.GET_STRUCT(wp_coords)){
            setLastErrorString("Could not read waypoint coordinates.");
            return false;
          }

          pos.x = wp_coords->_x;
          pos.y = wp_coords->_y;
          pos.z = wp_coords->_z;
          
          Waypoint_ptr wp(new Waypoint(name, pos));
          map->waypoints.addWaypoint(wp);
        }
        else{
          setLastErrorString("Unknown waypoint node.");
          return false;
        }

        nodeWaypoint = f.getNextNode(nodeWaypoint, type);
      }
    }
    else{
      setLastErrorString("Unknown map node.");
      return false;
    }

    nodeMapData = f.getNextNode(nodeMapData, type);
  }

  std::cout << "Notice: [OTBM Loader] Loading time : " << (OTSYS_TIME() - start)/(1000.) << " s" << std::endl;
  return true;
}
