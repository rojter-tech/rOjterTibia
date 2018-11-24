-------------------------------------------------------------------
-- OpenTibia - an opensource roleplaying game
----------------------------------------------------------------------
-- This program is free software; you can redistribute it and-or
-- modify it under the terms of the GNU General Public License
-- as published by the Free Software Foundation; either version 2
-- of the License, or (at your option) any later version.
--
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with this program; if not, write to the Free Software Foundation,
-- Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
----------------------------------------------------------------------

BeginEnumFile("enums")

enum ("RaceType",
	"RACE_NONE",
	{"RACE_VENOM ", "venom"},
	{"RACE_BLOOD", "blood"},
	{"RACE_UNDEAD", "undead"},
	{"RACE_FIRE", "fire"},
	{"RACE_ENERGY", "energy"}
)

enum ("Direction",
	"NORTH = 0",
	"EAST = 1",
	"SOUTH = 2",
	"WEST = 3",
	"SOUTHWEST = 4",
	"SOUTHEAST = 5",
	"NORTHWEST = 6",
	"NORTHEAST = 7"
)

enum ({name="CombatType", bitmask=true},
	{"COMBAT_NONE", "unknown", null=true},
	{"COMBAT_PHYSICALDAMAGE", "physical"},
	{"COMBAT_ENERGYDAMAGE", "energy"},
	{"COMBAT_EARTHDAMAGE", "earth"},
	{"COMBAT_FIREDAMAGE", "fire"},
	{"COMBAT_SCRIPTED_HEALTH", "scriptedhealth"},
	{"COMBAT_SCRIPTED_MANA", "scriptedmana"},
	{"COMBAT_LIFEDRAIN", "lifedrain"},
	{"COMBAT_MANADRAIN", "manadrain"},
	{"COMBAT_HEALING", "healing"},
	{"COMBAT_DROWNDAMAGE", "drown"},
	{"COMBAT_ICEDAMAGE", "ice"},
	{"COMBAT_HOLYDAMAGE", "holy"},
	{"COMBAT_DEATHDAMAGE", "death"}
)

enum ("BlockType",
	"BLOCK_NONE",
	"BLOCK_DEFENSE",
	"BLOCK_ARMOR",
	"BLOCK_IMMUNITY"
)

enum ("ViolationAction",
	"ACTION_NOTATION = 0",
	"ACTION_NAMEREPORT = 1",
	"ACTION_BANISHMENT = 2",
	"ACTION_BANREPORT = 3",
	"ACTION_BANFINAL = 4",
	"ACTION_BANREPORTFINAL = 5",
	"ACTION_STATEMENT = 6",
	"ACTION_DELETION = 7"
)

enum ("SkillType",
	{"SKILL_FIST = 0", "fist fighting", "fist"},
	{"SKILL_CLUB = 1", "club fighting", "club"},
	{"SKILL_SWORD = 2", "sword fighting", "sword"},
	{"SKILL_AXE = 3", "axe fighting", "axe"},
	{"SKILL_DIST = 4", "distance fighting", "distance"},
	{"SKILL_SHIELD = 5", "shielding"},
	{"SKILL_FISH = 6", "fishing"}
)

enum ("LevelType",
	"LEVEL_FIST = SKILL_FIST",
	"LEVEL_CLUB = SKILL_CLUB",
	"LEVEL_SWORD = SKILL_SWORD",
	"LEVEL_AXE = SKILL_AXE",
	"LEVEL_DIST = SKILL_DIST",
	"LEVEL_SHIELD = SKILL_SHIELD",
	"LEVEL_FISH = SKILL_FISH",
	-- Meta-skills, used by some functions
	"LEVEL_MAGIC = 7",
	"LEVEL_EXPERIENCE = 8"
)

enum ("PlayerStatType",
	"STAT_MAXHITPOINTS",
	"STAT_MAXMANAPOINTS",
	"STAT_SOULPOINTS",
	"STAT_MAGICPOINTS"
)

enum ("LossType",
	"LOSS_EXPERIENCE = 0",
	"LOSS_MANASPENT = 1",
	"LOSS_SKILLTRIES = 2",
	"LOSS_ITEMS = 3",
	"LOSS_CONTAINERS = 4"
)

enum ("PlayerSex",
	{"SEX_FEMALE = 0", "female"},
	{"SEX_MALE = 1", "male"},
	{"SEX_FEMALE_GAMEMASTER = 2", "femalegm"},
	{"SEX_MALE_GAMEMASTER = 3", "malegm"},
	{"SEX_FEMALE_MANAGER = 4", "femalecm"},
	{"SEX_MALE_MANAGER = 5", "malecm"},
	{"SEX_FEMALE_GOD = 6", "femalegod"},
	{"SEX_MALE_GOD = 7", "malegod"}
)

enum ("ChaseMode",
	"CHASEMODE_STANDSTILL",
	"CHASEMODE_FOLLOW"
)

enum ("FightMode",
	"FIGHTMODE_ATTACK",
	"FIGHTMODE_BALANCED",
	"FIGHTMODE_DEFENSE"
)

enum ("TradeState",
	"TRADE_NONE",
	"TRADE_INITIATED",
	"TRADE_ACCEPT",
	"TRADE_ACKNOWLEDGE",
	"TRADE_TRANSFER"
)

enum ("SlotType",
	"SLOT_WHEREEVER = 0",
	"SLOT_HEAD = 1",
	"SLOT_NECKLACE = 2",
	"SLOT_BACKPACK = 3",
	"SLOT_ARMOR = 4",
	"SLOT_RIGHT = 5",
	"SLOT_LEFT = 6",
	"SLOT_LEGS = 7",
	"SLOT_FEET = 8",
	"SLOT_RING = 9",
	"SLOT_AMMO = 10",
	
	-- Special slot, covers two, not a real slot
	"SLOT_HAND = 11",
	"SLOT_TWO_HAND = SLOT_HAND",
	
	-- Usual slot iteration begins on HEAD and ends on AMMO slot,
	-- Note that iteration over these are INCLUSIVE, so use <=, not !=
	"SLOT_FIRST = SLOT_HEAD",
	"SLOT_LAST = SLOT_HAND"
)

enum ({name="SlotPosition", bitmask=true},
	{"SLOTPOSITION_NONE", "none", null=true},
	{"SLOTPOSITION_HEAD = 1", "head"},
	{"SLOTPOSITION_NECKLACE = 2", "necklace"},
	{"SLOTPOSITION_BACKPACK = 4", "backpack"},
	{"SLOTPOSITION_ARMOR = 8", "armor"},
	{"SLOTPOSITION_RIGHT = 16", "right-hand"},
	{"SLOTPOSITION_LEFT = 32", "left-hand"},
	{"SLOTPOSITION_LEGS = 64", "legs"},
	{"SLOTPOSITION_FEET = 128", "feet"},
	{"SLOTPOSITION_RING = 256", "ring"},
	{"SLOTPOSITION_AMMO = 512", "ammo"},
	{"SLOTPOSITION_DEPOT = 1024", "depot"},
	{"SLOTPOSITION_TWO_HAND = 2048", "two-hand"},
	{"SLOTPOSITION_HAND = SLOTPOSITION_LEFT | SLOTPOSITION_RIGHT", "hand"},
	{"SLOTPOSITION_WHEREEVER", "any", "anywhere", all = true}
)

enum ({name="TileProp", bitmask=true},
	{"TILEPROP_NONE", null=true},
	{"TILEPROP_PROTECTIONZONE"},
	{"TILEPROP_DEPRECATED"},
	{"TILEPROP_NOPVPZONE"},
	{"TILEPROP_NOLOGOUT"},
	{"TILEPROP_PVPZONE"},
	{"TILEPROP_REFRESH"},

	--internal usage
	{"TILEPROP_POSITIONCHANGE"},
	{"TILEPROP_FLOORCHANGE"},
	{"TILEPROP_FLOORCHANGE_DOWN"},
	{"TILEPROP_FLOORCHANGE_NORTH"},
	{"TILEPROP_FLOORCHANGE_SOUTH"},
	{"TILEPROP_FLOORCHANGE_EAST"},
	{"TILEPROP_FLOORCHANGE_WEST"},
	{"TILEPROP_BLOCKSOLID"},
	{"TILEPROP_BLOCKPATH"},
	{"TILEPROP_VERTICAL"},
	{"TILEPROP_HORIZONTAL"},
	{"TILEPROP_BLOCKPROJECTILE"},
	{"TILEPROP_BLOCKSOLIDNOTMOVEABLE"},
	{"TILEPROP_BLOCKPATHNOTMOVEABLE"},
	{"TILEPROP_BLOCKPATHNOTFIELD"},
	{"TILEPROP_HOUSE_TILE"},
	{"TILEPROP_DYNAMIC_TILE"},
	{"TILEPROP_INDEXED_TILE"}
)

enum ("ZoneType",
	"ZONE_PROTECTION",
	"ZONE_NOPVP",
	"ZONE_PVP",
	"ZONE_NOLOGOUT",
	"ZONE_NORMAL"
)

enum ("WorldType",
	"WORLD_TYPE_NOPVP",
	"WORLD_TYPE_PVP",
	"WORLD_TYPE_PVPE"
)

enum ("GameState",
	"GAME_STATE_STARTUP",
	"GAME_STATE_INIT",
	"GAME_STATE_NORMAL",
	"GAME_STATE_CLOSED",
	"GAME_STATE_SHUTDOWN",
	"GAME_STATE_CLOSING"
)

enum ("ServerSaveType",
	"SERVER_SAVE_FULL",
	"SERVER_SAVE_BINARY",
	"SERVER_SAVE_RELATIONAL",
	"SERVER_SAVE_NORMAL",
	"SERVER_SAVE_SHALLOW"
)

enum ("Script::ListenerType",
	-- Not tied to a creature
	"ON_SAY_LISTENER",
	"ON_USE_ITEM_LISTENER",
	"ON_USE_WEAPON_LISTENER",
	"ON_EQUIP_ITEM_LISTENER",
	"ON_MOVE_CREATURE_LISTENER",
	"ON_MOVE_ITEM_LISTENER",
	"ON_OPEN_CHANNEL_LISTENER",
	"ON_CLOSE_CHANNEL_LISTENER",
	"ON_ACCOUNT_LOGIN_LISTENER",
	"ON_LOGIN_LISTENER",
	"ON_LOGOUT_LISTENER",
	"ON_CHANGE_OUTFIT_LISTENER",
	"ON_LOOK_LISTENER",
	"ON_LOOKED_AT_LISTENER",
	"ON_TURN_LISTENER",
	"ON_LOAD_LISTENER",
	"ON_UNLOAD_LISTENER",
	"ON_SPAWN_LISTENER",
	"ON_KILLED_LISTENER",
	"ON_DEATH_BY_LISTENER",
	"ON_KILL_LISTENER",
	"ON_DEATH_LISTENER",
	"ON_ADVANCE_LISTENER",
	"ON_SHOP_PURCHASE_LISTENER",
	"ON_SHOP_SELL_LISTENER",
	"ON_SHOP_CLOSE_LISTENER",
	"ON_TRADE_BEGIN_LISTENER",
	"ON_TRADE_END_LISTENER",
	"ON_ATTACK_LISTENER",
	"ON_DAMAGE_LISTENER",
	"ON_CONDITION_LISTENER",
	"ON_ACTOR_LOAD_SPELL_LISTENER",
	"ON_ACTOR_CAST_SPELL_LISTENER",
	-- Creature specific only
	"ON_THINK_LISTENER",
	"ON_HEAR_LISTENER",
	"ON_SPOT_CREATURE_LISTENER",
	"ON_LOSE_CREATURE_LISTENER"
)

EndEnumFile()
