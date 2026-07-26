# Server Design — Scaling Kung-Fu Chess to Cloud Scale

This document answers the final-week CTD assignment: designing a server architecture that supports 100 million registered users and 10 million concurrent active users, using Docker and Kubernetes/K3s.

## 1. Current State

The existing server side (`server/GameServer.h/.cpp`) is a single process with:
- **One global game** — a single engine, a single board, 2 seats + spectators.
- A main thread that advances the game every 30ms (`SERVER_TICK_MS`) and broadcasts a full snapshot (JSON) to all connected clients.
- A transport layer (`server/network/NetworkServer`) already separated from the game logic — a first, natural step toward running multiple instances, since `GameServer` no longer depends directly on ixwebsocket, only on connection IDs.
- No DB, no real authentication (username is just a label), no concept of a "room/lobby".

This document describes how to extend this into a full cloud architecture.

## 2. Guiding Principle

The system should be split by **how often its state changes** and **what kind of load it carries**:

| Concern | Change frequency | Load type |
|---|---|---|
| User identity, ELO, stats | Rare (once per login/match end) | Many reads, few writes |
| Matchmaking | A few seconds | Many small, short requests |
| Room location (room → node) | For the lifetime of a match (30-90 seconds) | Light tracking |
| Actual game state (board, moves) | Every 30ms | Heavy, real-time traffic |

Each row = a separate service = a separate Docker image, managed by Kubernetes/K3s. This way every layer can scale independently, according to its own specific needs.

## 3. The Five Services

### 3.1 Auth Service
**Role**: registration, login, reading/writing username+password+ELO to the DB.

- **Fully stateless** — remembers nothing between requests. Every request carries all the info it needs (or a token).
- Allows running N identical instances behind a plain load balancer, scaling horizontally with zero coordination between instances.
- The easiest service to scale, since it has no shared state besides the DB itself.

### 3.2 Matchmaking Service
**Role**: the "Play" button → finds an opponent with ELO ±100, one-minute timeout.

- The problem: with multiple instances, each one is "blind" to the others' queue — two players could each wait on a different instance and never get matched.
- **Solution**: a shared external queue (Redis, a sorted set by ELO) that all instances read/write. This way it doesn't matter which instance a request landed on — the shared state lives in Redis, not in the pod's memory.
- Once a match is found, this service calls the Room Service to create a room.

### 3.3 Room/Lobby Service
**Role**: creating/joining a room, assigning a room ID, routing players to the right node.

- Holds a **registry**: a mapping of `room_id → game-server node/address`, stored in Redis, not in local memory, so every instance of the Room Service (and other services) can know "where to send" a player who wants to join an existing room.
- When a new game is created: picks a game-server node with low load (e.g. fewest active rooms right now), registers it, and returns that node's address to the client.
- This is what makes "everyone can play with everyone" possible — no matter which node a game actually runs on, any player in the world can ask the registry "where is room X?" and get an answer.

### 3.4 Game Server instances (what we already built)
**Role**: actually running the game — the existing `GameServer`/`GameEngine`/`NetworkServer`.

- Many small, identical instances, **each running several rooms/games concurrently** (not a single global game like today — requires changing to `map<RoomId, unique_ptr<GameEngine>>` + a mutex per room, exactly the gap already identified in the existing code).
- The number of rooms each node can host is bounded by the broadcast traffic volume it can absorb (see the calculation in section 4).
- **Ephemeral**: since a match only lasts 30-90 seconds, there's no point maintaining long-lived state on a specific node. Kubernetes can spin pods up/down dynamically based on load, with no "memory" that needs to survive restarts.
- **The client connects via WebSocket directly to the specific node** running its room — not through a central gateway layer. This way the heavy traffic (snapshot broadcasts every 30ms) doesn't funnel through a single bottleneck, but spreads across all nodes in parallel.

### 3.5 Database
**Role**: persistent storage of users, ELO, statistics.

**Why not SQLite**: SQLite is a single file on local disk, with no built-in support for multiple concurrent writers over the network, and no replication/sharding. It fits an embedded/single-process use case, not hundreds of Auth Service instances reading/writing concurrently from around the world, nor a dataset of 100 million users with a high-availability requirement.

**What instead**: a distributed DB, matched to the access pattern:
- **Managed Postgres with sharding** (e.g. by hash of user ID) — fits if the data is mostly relational (user↔stats↔match history) and strong consistency on ELO is desired.
- **Cassandra/DynamoDB** — fits if eventual consistency and very high write throughput are preferred over strict atomicity (e.g. match history).
- Either way: **replication** (primary + read replicas) for failover and distributing read load (the Auth Service is mostly reads, not writes).

## 4. Answers to the Numeric Questions

### Question 3 — Network Traffic Volume
An active user makes a move roughly every ~2 seconds. A single `move` message is small (tens of bytes of JSON). But the server broadcasts a **full snapshot** to all clients every 30ms — a rate of ~33 messages/second **per room**, regardless of the actual move rate.

Rough calculation for a single room (2 players + a few spectators, snapshot ~1-2KB):
- 33 snapshots/sec × ~1.5KB × 2 sides (uploaded per room) ≈ **~100KB/sec per room** leaving the node.
- With 10 million concurrently active users, assuming ~4 players/spectators per room on average → **~2.5 million active rooms** → **~250GB/sec** total traffic if all of them run in parallel at any given moment.

**Conclusion**: this is a massive sum no single node could carry — this is exactly why the load **must** be spread across thousands of game-server nodes, and why broadcast rate should be reduced (e.g. sending a snapshot only when something changed / delta-encoding, instead of a constant full snapshot — a recommended future improvement). It's also why the client connects directly to its room's node rather than through a central gateway — otherwise all 250GB/sec would flow through a single point.

### Question 4 — Short Match Duration (30-90 seconds)
This is exactly what enables the following roles for the Docker containers:
- **Ephemeral game-server pods** — there's no point in "long-lived state" on a specific node, since at any given moment most of the games that ran on it a minute ago have already ended. K8s can rotate pods (rolling restarts, scale up/down) without risking the loss of "long" games.
- **Simple failure recovery**: if a node goes down, there's no point in precisely restoring the game (that would only pay off if a match lasted hours) — the practical solution: K8s spins up a new pod, and the games that were on the fallen node are treated as an immediate final result (a loss/draw for both sides, like a timeout).
- **Fast, effective autoscaling**: since games are short, the scheduler can react to load within a few seconds — new pods get "picked up" by the next matchmaking round almost immediately.

## 5. Communication Flow Diagram

```
Client → Auth Service (login/register) ──────────────→ DB
Client → Matchmaking Service ←──────→ Redis (ELO queue)
Client → Room Service ←──────────────→ Redis (registry: room_id → node)
Client → Game-Server node (direct WebSocket) ← this is the heavy path, actual gameplay
Auth / Room / Matchmaking Service ──→ DB (only occasional writes: match result, new ELO)
```

## 6. Kubernetes/K3s — What Manages What

- **Auth Service, Matchmaking Service, Room Service**: regular Deployments with a Horizontal Pod Autoscaler (HPA) by CPU/request count — all stateless, easy to scale.
- **Game-Server**: Deployment/StatefulSet with autoscaling by number of active rooms rather than CPU alone (custom metric) — each pod "fills up" to a room ceiling, and only then is a new pod added.
- **Redis**: a managed cluster (Redis Cluster/Sentinel) — not a single pod, so it also isn't a single point of failure.
- **DB**: outside the K8s cluster, or as an external managed service (RDS/Cloud SQL) with replication — it's not a good idea to run a complex stateful DB inside the same K3s cluster that manages ephemeral workloads.
- A Load Balancer/Ingress at the cluster entrance distributes HTTP(S) requests to Auth/Matchmaking/Room; WebSocket connections to the game-server are routed directly to the specific pod (via a NodePort/LoadBalancer service per node, not through the regular Ingress).

## 7. Failure Scenarios

| Failure | Response |
|---|---|
| A game-server node goes down | K8s spins up a new pod; the games that were on it end as a timeout/draw (see section 4) — no need for precise recovery since the match is short anyway |
| The DB (primary) goes down | Reads (login, ELO check) go to a read replica; writes wait for a replica to fail over to primary; no data loss thanks to replication + regular backups |
| A node runs out of disk space | Mostly relevant to logs — the solution: **don't** store logs on the pod's local disk (which disappears anyway when the pod goes down), but ship them immediately to a central log aggregation service/external storage (e.g. ELK/CloudWatch/Loki). This way the node's disk never accumulates anything and can't run out |
| Redis goes down | If it's a single instance — matchmaking/routing gets stuck. Solution: run Redis in a cluster/Sentinel configuration with replication, not a single instance |

## 8. Gaps to Address in the Existing Code

- Replace the single `GameEngine` in `GameServer` with `map<RoomId, unique_ptr<GameEngine>>` + a mutex per room, to support multiple rooms on the same node.
- Add a real session token (instead of a username string) to identify a returning player.
- Implement the registry (room→node) in Redis, and the Room/Matchmaking Services as new services on top of the DB.
- Consider delta-encoding/change-only broadcasts instead of a full snapshot every tick, to reduce the traffic volume calculated in section 4.
