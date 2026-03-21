// Copyright (C) 2019 The SpaceXpanse developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "coord.hpp"

#include <glog/logging.h>

namespace ships
{

Direction
operator- (const Direction d)
{
  /* Map every direction to its opposite.  */
  switch (d)
    {
    case Direction::LEFT:
      return Direction::RIGHT;
    case Direction::RIGHT:
      return Direction::LEFT;

    case Direction::UP:
      return Direction::DOWN;
    case Direction::DOWN:
      return Direction::UP;
    }

  LOG (FATAL) << "Invalid direction: " << static_cast<int> (d);
}

Coord::Coord (const int ind)
{
  /* Convert linear index to matrix coordinates.  */
  column = ind % SIDE;
  row = ind / SIDE;

  /* Note:  The C++ language guarantees that (row * SIDE + column == ind)
     with the definitions above.  This implies, in particular, that at least
     one of row and column is negative for a negative ind.  Thus that case
     is "handled fine", because all we need is that it yields an out-of-board
     coordinate.  It does not matter which value is negative.  */
}

bool
Coord::IsOnBoard () const
{
  /* Board bounds are checked independently for row and column.  */
  if (row < 0 || row >= SIDE)
    return false;
  if (column < 0 || column >= SIDE)
    return false;

  return true;
}

int
Coord::GetIndex () const
{
  /* Convert matrix coordinates back to a linear index.  */
  CHECK (IsOnBoard ());

  const int res = row * SIDE + column;
  CHECK_GE (res, 0);
  CHECK_LT (res, CELLS);

  return res;
}

Coord
Coord::operator+ (const Direction d) const
{
  /* Return neighbouring coordinates in the selected direction.  */
  switch (d)
    {
    case Direction::LEFT:
      return Coord (row, column - 1);
    case Direction::RIGHT:
      return Coord (row, column + 1);

    case Direction::UP:
      return Coord (row - 1, column);
    case Direction::DOWN:
      return Coord (row + 1, column);
    }

  LOG (FATAL) << "Invalid direction: " << static_cast<int> (d);
}

Coord
Coord::operator- (const Direction d) const
{
  return (*this) + (-d);
}

bool
operator== (const Coord& a, const Coord& b)
{
  /* Coordinates are equal only if both row and column match.  */
  return a.row == b.row && a.column == b.column;
}

bool
operator!= (const Coord& a, const Coord& b)
{
  return !(a == b);
}

} // namespace ships
