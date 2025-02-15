// Copyright (C) 2019 The SpaceXpanse developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef SPACEXPANSEGAME_SIGNATURES_HPP
#define SPACEXPANSEGAME_SIGNATURES_HPP

#include "rpc-stubs/xayarpcclient.h"

#include <string>

namespace xaya
{

/**
 * Verifies the signature of a message, as per SpaceXpanse Core's "verifymessage"
 * feature.  The message must be a string suitable for passing over RPC,
 * so binary data must be encoded or hashed accordingly.  The signature is
 * the base64-encoded string as used by SpaceXpanse Core.
 *
 * This method returns the address for which the signature is valid (if any),
 * which must be compared to the expected address.  If the signature is invalid
 * in general, then the string "invalid" is returned (which is unequal to any
 * valid SpaceXpanse address).
 */
std::string VerifyMessage (SpaceXpanseRpcClient& rpc,
                           const std::string& msg, const std::string& sgn);

} // namespace xaya

#endif // SPACEXPANSEGAME_SIGNATURES_HPP
