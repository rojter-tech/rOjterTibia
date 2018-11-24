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

#ifndef __OTSERV_PARTY_H__
#define __OTSERV_PARTY_H__

#include <vector>
#include <map>
#include "const.h"

class Player;

typedef std::vector<Player*> PlayerVector;

class Party
{
public:
  Party(Player* _leader);
  ~Party();

  Player* getLeader() const;
  void setLeader(Player* _leader);

  void disband();
  bool invitePlayer(Player* player);
  bool joinParty(Player* player);
  bool revokeInvitation(Player* player);
  bool passPartyLeadership(Player* player);
  bool leaveParty(Player* player);

  bool removeInvite(Player* player);

  bool isPlayerMember(const Player* player) const;
  bool isPlayerInvited(const Player* player) const;
  void updateAllPartyIcons();
  void updatePartyIcons(Player* player);
  void broadcastPartyMessage(MessageClass msgClass, const std::string& msg, bool sendToInvitations = false);
  bool disbandParty() const;
  bool canOpenCorpse(uint32_t ownerId);

  void shareExperience(uint64_t experience, bool fromMonster);
  bool setSharedExperience(Player* player, bool _sharedExpActive);
  bool isSharedExperienceActive() const;
  bool isSharedExperienceEnabled() const;
  bool canUseSharedExperience(const Player* player) const;
  void updateSharedExperience();

  void addPlayerHealedMember(Player* player, uint32_t points);
  void addPlayerDamageMonster(Player* player, uint32_t points);
  void clearPlayerPoints(Player* player);

  const PlayerVector& getMemberList() const;

private:
  bool sharedExpActive;
  bool sharedExpEnabled;

  Player* leader;
  PlayerVector memberList;
  PlayerVector inviteList;

  struct CountBlock_t{
    int32_t totalHeal;
    int32_t totalDamage;
    int64_t ticks;
  };
  typedef std::map<uint32_t, CountBlock_t> CountMap;
  CountMap pointMap;

  bool canEnableSharedExperience();
  /*
  void hasAttackedMonster(Player* player);
  void hasHealedMember(Player* player);
  */
};

#endif

