#!/usr/bin/env python3
# Copyright (C) 2018-2022 The SpaceXpanse developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

from mover import MoverTest

import time

"""
Tests how the moverd reacts if spacexpansed is stopped and restarted intermittantly.
The connection probe & fix logic should take care of it.
"""


class StoppedSpaceXpansedTest (MoverTest):

  def getState (self):
    """
    Returns the 'state' of the GSP.
    """

    return self.rpc.game.getnullstate ()["state"]

  def run (self):
    self.generate (101)
    self.expectGameState ({"players": {}})

    # Turn on the connection check and reduce the staleness threshold
    # so we can reasonably test with it.
    self.mainLogger.info ("Turning on connection checker...")
    self.stopGameDaemon ()
    args = [
      "--spacexpanse_connection_check_ms=100",
      "--spacexpanse_zmq_staleness_ms=1000",
    ]
    self.startGameDaemon (extraArgs=args)

    self.move ("a", "k", 2)
    self.generate (1)
    self.expectGameState ({"players": {
      "a": {"x": 0, "y": 1, "dir": "up", "steps": 1},
    }})

    # If we don't stop the SpaceXpanse daemon, the connection will remain good,
    # even if no blocks happen within the staleness period (because the pings
    # will take care of it).
    self.mainLogger.info ("Pings keep the connection good...")
    time.sleep (1.5)
    self.assertEqual (self.getState (), "up-to-date")
    self.stopGameDaemon ()
    assert self.gamenode.logMatches ("seems stale, requesting a block")
    self.startGameDaemon (extraArgs=args)

    # Stop and restart the SpaceXpanse Core daemon.  The GSP will notice it lost the
    # connection, and automatically reconnect once it is back up.
    self.mainLogger.info ("Stopping SpaceXpanse daemon...")
    self.spacexpansenode.stop ()
    time.sleep (1.5)
    self.assertEqual (self.getState (), "disconnected")

    self.mainLogger.info ("Starting SpaceXpanse daemon...")
    self.spacexpansenode.start ()
    self.rpc.spacexpanse = self.spacexpansenode.rpc
    time.sleep (1.5)
    self.assertEqual (self.getState (), "up-to-date")

    # Mine another block and verify that the game updates.
    self.generate (1)
    self.expectGameState ({"players": {
      "a": {"x": 0, "y": 2},
    }})

    self.stopGameDaemon ()
    assert self.gamenode.logMatches ("re-establish the SpaceXpanse connection")
    self.startGameDaemon ()


if __name__ == "__main__":
  StoppedSpaceXpansedTest ().main ()
