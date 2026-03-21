// Copyright (C) 2019-2022 The SpaceXpanse developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "gamestatejson.hpp"

#include "channelstatejson.hpp"
#include "protoutils.hpp"

namespace spacexpanse
{

Json::Value
ChannelToGameStateJson (const ChannelData& ch, const BoardRules& r)
{
  /* Serialise one channel entry with metadata, latest state, and reinit base.  */
  const auto& id = ch.GetId ();
  const auto& meta = ch.GetMetadata ();

  Json::Value res(Json::objectValue);
  res["id"] = id.ToHex ();
  if (ch.HasDispute ())
    res["disputeheight"] = static_cast<int> (ch.GetDisputeHeight ());
  res["meta"] = ChannelMetadataToJson (meta);

  res["state"] = BoardStateToJson (r, id, meta, ch.GetLatestState ());
  res["state"]["proof"] = ProtoToBase64 (ch.GetStateProof ());
  res["reinit"] = BoardStateToJson (r, id, meta, ch.GetReinitState ());

  return res;
}

Json::Value
AllChannelsGameStateJson (ChannelsTable& tbl, const BoardRules& r)
{
  /* Enumerate all channels into deterministic JSON object keyed by channel id.  */
  Json::Value res(Json::objectValue);
  auto stmt = tbl.QueryAll ();
  while (stmt.Step ())
    {
      auto h = tbl.GetFromResult (stmt);
      res[h->GetId ().ToHex ()] = ChannelToGameStateJson (*h, r);
    }

  return res;
}

} // namespace spacexpanse
