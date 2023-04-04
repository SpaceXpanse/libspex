// Copyright (C) 2019-2020 The SpaceXpanse developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef SPACEXPANSESHIPS_SCHEMA_HPP
#define SPACEXPANSESHIPS_SCHEMA_HPP

#include "spacexpansegame/sqlitestorage.hpp"

namespace ships
{

/**
 * Sets up or updates the database schema for the on-chain state of
 * SpaceXpanseships, not including data of the side channels themselves (which
 * is managed by the game-channel framework).
 */
void SetupShipsSchema (spacexpanse::SQLiteDatabase& db);

} // namespace ships

#endif // SPACEXPANSESHIPS_SCHEMA_HPP
