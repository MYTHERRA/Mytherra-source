Mytherra Core 25.2.1 Release Notes
=================================

Mytherra Core 25.2.1 replaces 25.2.0. It fixes a weakness in the new LWMA
difficulty algorithm and moves the hard fork activation height.

**Every node running 25.2.0 or older must upgrade to 25.2.1.**

Changed activation height
-------------------------

The LWMA difficulty hard fork now activates at mainnet block height **62000**
(previously announced: 60381).

- Nodes still running **25.2.0** would switch to the old LWMA variant at
  block 60381 and split from the network. They must upgrade to 25.2.1
  **before block 60381**.
- Nodes running **25.1.0 or older** must upgrade before block 62000.

Fix: timestamp manipulation in LWMA
-----------------------------------

In 25.2.0 each solve time was clamped individually to the range 1..1800
seconds. A block with a timestamp earlier than its predecessor therefore
counted as 1 second instead of compensating for the forward-dated block before
it. A miner alternating forward-dated and back-dated timestamps could steadily
lower difficulty without a hashrate majority.

25.2.1 uses the LWMA-1 timestamp handling: the algorithm tracks the highest
timestamp seen in the window, and a block that does not exceed it counts as
1 second after it. Forward-dated timestamps are now paid back by the
following blocks. Solve times remain capped at 1800 seconds.

Shorter future block time limit
-------------------------------

- Blocks may now be at most **10 minutes** ahead of network-adjusted time
  (previously 2 hours). This limits how far timestamps can be pushed forward.
- The default `-maxtimeadjustment` is reduced from 70 to 5 minutes.

Node operators and miners must keep their system clocks synchronized (NTP).
Blocks too far in the future are not marked invalid permanently; they are
accepted once the local clock catches up.

Other changes
-------------

- `getnetworkhashps` with the default lookup now averages over the last
  120 blocks after LWMA activation (25.2.0 used a single block).

How to upgrade
==============

Same procedure as 25.2.0: back up wallets, stop the node, install 25.2.1 over
the previous installation and restart with the existing data directory. No
reindex is required. Verify with `mytherra-cli getnetworkinfo` that the
version is 25.2.1.
