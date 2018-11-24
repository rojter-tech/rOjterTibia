//////////////////////////////////////////////////////////////////////
// OpenTibia - an opensource roleplaying game
//////////////////////////////////////////////////////////////////////
// Status-Singleton for OTServ
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

#ifndef __OTSERV_STATUS_H__
#define __OTSERV_STATUS_H__

#include <string>
#include <map>
#include <stdint.h>
#include <boost/noncopyable.hpp>
#include "protocol.h"

class ProtocolStatus : public Protocol
{
public:
  // static protocol information
  enum {server_sends_first = false};
  enum {protocol_identifier = 0xFF};
  enum {use_checksum = false};

#ifdef __ENABLE_SERVER_DIAGNOSTIC__
  static uint32_t protocolStatusCount;
#endif

  ProtocolStatus(Connection_ptr connection);
  virtual ~ProtocolStatus();

  virtual void onRecvFirstMessage(NetworkMessage& msg);

  static const char* protocol_name();

protected:
  static std::map<uint32_t, int64_t> ipConnectMap;

  #ifdef __DEBUG_NET_DETAIL__
  virtual void deleteProtocolTask();
  #endif
};

class Status : boost::noncopyable {
public:
  Status();

  static Status* instance();

  void addPlayer();
  void removePlayer();
  bool hasSlot() const;

  std::string getStatusString() const;
  void getInfo(uint32_t requestedInfo, OutputMessage_ptr output, NetworkMessage& msg) const;

  uint32_t getPlayersOnline() const;
  uint32_t getMaxPlayersOnline() const;

  void setMaxPlayersOnline(int max);

  uint64_t getUpTime() const;

private:
  uint64_t m_start;
  int m_playersmax, m_playersonline, m_playerspeak;
  std::string m_mapname, m_mapauthor;

};

#endif
