# Peer-to-peer multiplayer registry

`signalboxes.csv` gives every map a stable code. `connections.csv` defines
reciprocal continuation portals. Both files are deliberately plain text so a
route author can register additional signalboxes without recompiling RailOS.

Live WTT routing between registered boxes is derived automatically. RailOS
looks at the owning box of the preceding or following WTT calls, restricts the
train to portals registered between those two boxes, then uses the railway
topology to select a valid track. `wtt_boundaries.csv` is only needed at the
outer edge of the currently modelled network, where the adjacent railway has no
multiplayer map yet. Adding another box to a chain therefore replaces the edge
fallback with its `connections.csv` entries; it does not require every service
or timing point to be mapped manually.

Each process writes a detailed diagnostic log beneath `Multiplayer logs`. The
log includes session and discovery state, WTT loading and per-service validation,
portal resolution, wire-level non-discovery packets, train offers and retries,
receiver rejection reasons, acknowledgements, chat and boundary-state traffic.

For a box with only one registered neighbour the portal fields may be left
blank. Boxes with multiple portals or neighbours must use the continuation
element ID so an exiting train is routed to the matching track unambiguously.

The current registry contains both tracks of the Guildford (`GD`) to Reigate
(`RG`) test link. Guildford portals `125-21` and `125-22` match Reigate portals
`33-19` and `33-20` respectively.

Peers discover each other directly on the same computer and by local-network
broadcast. There is no central game server. Internet play across routers would
need manual port forwarding or a later NAT-traversal/rendezvous option.

When two connected boxes share a session, each continuation also exchanges the
aspect of the first inward-facing signal on the neighbouring map. A route ending
at that continuation uses the remote aspect as its forward target: for example,
a red signal on the receiving map makes the preceding four-aspect signal on the
sending map show single yellow. The remote state feeds the normal signalling
calculation; it is not drawn as an extra signal.

## Two-window test

1. Open one copy of RailOS and load `Railways/MULTIPLAYER-GD.rly` with
   `Program timetables/MULTIPLAYER-GD 0800.ttb`.
2. Open a second copy and load `Railways/MULTIPLAYER-RG.rly` with
   `Program timetables/MULTIPLAYER-RG 0800.ttb`.
3. In the Guildford copy, open **MP**, choose **GD**, and select **New session**.
4. In the Reigate copy, open **MP**, choose **RG**, and select
   **Join best session**. Both windows should show the same session ID.
5. Start both timetables and set the routes for `2G01`. Reigate holds its
   continuation entry until Guildford's train leaves through `125-21`, then
   creates the same service at `33-19`.

The generator `CreateTestTimetables.ps1` documents the service and can rebuild
both binary timetable files after editing the times or stopping pattern.
