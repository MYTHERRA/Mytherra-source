Mytherra Core 25.2.0 Release Notes
=================================

Mytherra Core 25.2.0 introduces a consensus change to stabilize block production
and refreshes the MYT application and installer artwork.

Difficulty adjustment hard fork
------------------------------

MYT targets a block interval of 5 minutes. The previous difficulty algorithm
adjusted only once every 12 blocks and could not respond promptly to large
changes in mining hashrate.

Starting at mainnet block height **60381**, difficulty is adjusted on every
block using a linearly weighted moving average (LWMA) of the last 60 solve
times. The target block interval remains 300 seconds. Solve times used by the
algorithm are clamped to between 1 second and 1800 seconds. Block subsidy and
halving rules are unchanged.

This is a hard fork. **All full-node operators, mining pools, solo miners,
exchanges and explorer backends must upgrade before block 60381.** Nodes that
have not upgraded will apply the previous consensus rules from that height and
can follow a different chain.

No new blockchain, wallet or data directory is required. Existing balances,
transactions, addresses and blockchain history are preserved.

How to upgrade
==============

Back up any wallets before upgrading. Shut down the old node and wait until it
has completely stopped. Then install Mytherra Core 25.2.0 over the previous
installation and start the node with its existing configuration and data
directory. A reindex, rescan or fresh synchronization is not required.

For a manual Linux installation using `/usr/local/bin`:

```
mytherra-cli stop
# Wait until mytherrad has fully stopped and the new binary is ready.
mytherrad -daemon
mytherra-cli getblockchaininfo
```

Before replacing the binary, operators should preserve the previous binary and
verify the release checksum. After the upgrade, confirm that the version is
25.2.0, the node is synchronizing and its best block hash matches the network.

Pools and miners should verify that their block templates come from an upgraded
node. Exchanges and explorer operators should upgrade their backend nodes and
consider temporarily pausing deposits and withdrawals around activation if
their operational procedures require it.

Before activation, the previous binary can be restored if an operational issue
occurs. After activation, restoring a pre-25.2.0 node can result in following a
different chain and should only be done as part of a coordinated recovery.

## Artwork

The MYT logo has been updated in the Qt wallet, operating-system icons and
Windows installer. Logo artwork is distributed under the MIT license with
attribution to The Mytherra project.
