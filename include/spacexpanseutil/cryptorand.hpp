// Copyright (C) 2019 The SpaceXpanse developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef SPACEXPANSEUTIL_CRYPTORAND_HPP
#define SPACEXPANSEUTIL_CRYPTORAND_HPP

#include "uint256.hpp"

namespace spacexpanse
{

/**
 * Generator for secure random data, i.e. not deterministic like Random.
 * This can be used e.g. to generate hash commitments and salt values for
 * channel games.
 */
class CryptoRand
{

public:

  CryptoRand () = default;

  CryptoRand (const CryptoRand&) = delete;
  void operator= (const CryptoRand&) = delete;

  /**
   * Returns a random value of given type (e.g. uint256).
   */
  template <typename T>
    T Get ();

};

} // namespace spacexpanse

#endif // SPACEXPANSEUTIL_CRYPTORAND_HPP
