// Copyright (C) 2019 The Xaya developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "gamestatejson.hpp"

#include "testgame.hpp"

#include <xayautil/hash.hpp>
#include <xayautil/uint256.hpp>

#include <gtest/gtest.h>

#include <glog/logging.h>

#include <sstream>

namespace xaya
{
namespace
{

Json::Value
ParseJson (const std::string& str)
{
  std::istringstream in(str);
  Json::Value res;
  in >> res;
  CHECK (in);

  return res;
}

class GameStateJsonTests : public TestGameFixture
{

protected:

  ChannelsTable tbl;

  /** Test channel set up with state (100, 2).   */
  const xaya::uint256 id1 = xaya::SHA256::Hash ("channel 1");

  /** Test channel set up with state (40, 10) and a dispute.  */
  const xaya::uint256 id2 = xaya::SHA256::Hash ("channel 2");

  GameStateJsonTests ()
    : tbl(game)
  {
    auto h = tbl.CreateNew (id1);
    auto* p = h->MutableMetadata ().add_participants ();
    p->set_name ("foo");
    p->set_address ("addr 1");
    p = h->MutableMetadata ().add_participants ();
    p->set_name ("bar");
    p->set_address ("addr 2");
    h->SetState ("100 2");
    h.reset ();

    h = tbl.CreateNew (id2);
    p = h->MutableMetadata ().add_participants ();
    p->set_name ("foo");
    p->set_address ("addr 1");
    p = h->MutableMetadata ().add_participants ();
    p->set_name ("baz");
    p->set_address ("addr 2");
    h->SetState ("40 10");
    h->SetDisputeHeight (55);
    h.reset ();
  }

};

TEST_F (GameStateJsonTests, WithoutDispute)
{
  auto expected = ParseJson (R"(
    {
      "meta":
        {
          "participants":
            [
              {"name": "foo", "address": "addr 1"},
              {"name": "bar", "address": "addr 2"}
            ]
        },
      "state":
        {
          "data": {"count": 2, "number": 100},
          "turncount": 2,
          "whoseturn": null
        }
    }
  )");
  expected["id"] = id1.ToHex ();

  auto h = tbl.GetById (id1);
  EXPECT_EQ (ChannelToGameStateJson (*h, game.rules), expected);
}

TEST_F (GameStateJsonTests, WithDispute)
{
  auto expected = ParseJson (R"(
    {
      "disputeheight": 55,
      "meta":
        {
          "participants":
            [
              {"name": "foo", "address": "addr 1"},
              {"name": "baz", "address": "addr 2"}
            ]
        },
      "state":
        {
          "data": {"count": 10, "number": 40},
          "turncount": 10,
          "whoseturn": 0
        }
    }
  )");
  expected["id"] = id2.ToHex ();

  auto h = tbl.GetById (id2);
  EXPECT_EQ (ChannelToGameStateJson (*h, game.rules), expected);
}

TEST_F (GameStateJsonTests, InvalidState)
{
  const auto id = xaya::SHA256::Hash ("third channel");
  auto h = tbl.CreateNew (id);
  h->SetState ("invalid state");

  EXPECT_DEATH (ChannelToGameStateJson (*h, game.rules),
                "invalid state on chain");
}

TEST_F (GameStateJsonTests, AllChannels)
{
  Json::Value expected(Json::objectValue);

  auto h = tbl.GetById (id1);
  expected[h->GetId ().ToHex ()] = ChannelToGameStateJson (*h, game.rules);
  h = tbl.GetById (id2);
  expected[h->GetId ().ToHex ()] = ChannelToGameStateJson (*h, game.rules);

  EXPECT_EQ (AllChannelsGameStateJson (tbl, game.rules), expected);
}

} // anonymous namespace
} // namespace xaya
