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

#ifndef __OTSERV_CREATUREMANAGER_H__
#define __OTSERV_CREATUREMANAGER_H__

#include <string>
#include <stdint.h>
#include <libxml/tree.h>
#include "condition_attributes.h"

class CreatureType;
struct SpellBlock;
struct LootBlock;

class CreatureManager{
public:
  CreatureManager();
  ~CreatureManager();

  bool loadFromXml(const std::string& _datadir, bool reloading = false);
  bool isLoaded() const {return loaded;}
  bool configureSpells();
  bool reload();

  CreatureType* getMonsterType(const std::string& name);

  static uint32_t getLootRandom();

private:
  bool deserializeSpell(xmlNodePtr node, SpellBlock& sb);
  bool loadMonsterType(const std::string& file, const std::string& monster_name, bool reloading = false);

  bool loadLootContainer(xmlNodePtr, LootBlock&);
  bool loadLootItem(xmlNodePtr, LootBlock&);

  typedef std::map<std::string, CreatureType*> TypeMap;

  TypeMap creature_types;

  bool loaded;
  std::string datadir;

};

#endif
