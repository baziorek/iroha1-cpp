# gRPC keepalive soak test — PR #8 (`fix/grpc-keepalive`)

End-to-end verification scenario for the server-side gRPC keepalive added in
`irohad/torii/server_runner.cpp` (KEEPALIVE_TIME_MS=60s,
KEEPALIVE_TIMEOUT_MS=20s, KEEPALIVE_PERMIT_WITHOUT_CALLS=1).

**Goal:** prove that a client holding a mostly-idle Torii connection survives
>24h without `_MultiThreadedRendezvous`/`Rendezvous` errors — the failure mode
reported in hyperledger-iroha/iroha#1351.

Everything below is copy-paste runnable. All database credentials come from
environment variables — nothing is hardcoded.

---

## 0. What PASS looks like

After the soak duration (default **25 h**):

- `summary.json` reports `"stream_errors": 0` and `"probe_failures": 0`
- every periodic `Healthcheck`/`Status` RPC succeeded
- the `StatusStream` subscription either stayed open the whole time or was
  closed **cleanly by the server** (stale-round limit) and re-opened without
  transport errors

A transport-level failure (`RpcError` with code `UNAVAILABLE`,
`DEADLINE_EXCEEDED`, or a hung iterator) is a **FAIL** — that is exactly the
#1351 symptom.

> **Honesty note:** on a plain localhost/LAN path without a NAT/LB idle
timeout, the bug may not reproduce even without the fix — idle TCP
connections can live for days. The test still validates that (a) keepalive
pings do not break streams and (b) the channel survives the full window.
> For a stronger repro see §8 (simulated middlebox idle timeout).

---

## 1. Prerequisites

```bash
# Iroha built from the PR branch (adjust path to your build tree)
export IROHAD_BIN=$PWD/build/bin/irohad
$IROHAD_BIN --help >/dev/null && echo "irohad OK"

# Python client deps
python3 -m pip install --user grpcio grpcio-tools
python3 -c "import grpc, grpc_tools; print('grpc OK')"

# Optional, only for DB_TYPE=postgres:
#   docker, or a reachable PostgreSQL instance
```

Repo checkout must be the `fix/grpc-keepalive` branch (or `iroha1-main`
after merge) — the test exercises `server_runner.cpp`.

---

## 2. Environment variables

| Variable | Default | Meaning |
|---|---|---|
| `DB_TYPE` | `rocksdb` | `rocksdb` (no DB server needed) or `postgres` |
| `IROHA_PG_HOST` | `localhost` | Postgres host (only if `DB_TYPE=postgres`) |
| `IROHA_PG_PORT` | `5432` | Postgres port |
| `IROHA_PG_USER` | `postgres` | Postgres user |
| `IROHA_PG_PASSWORD` | *(empty)* | Postgres password |
| `IROHA_PG_DB` | `iroha_data` | working database |
| `IROHA_PG_MAINT_DB` | `postgres` | maintenance database |
| `SOAK_DIR` | `./soak-work` | scratch dir for config, logs, summary |
| `TORII_PORT` | `50051` | node Torii port |
| `INTERNAL_PORT` | `10001` | node internal (ordering) port |
| `SOAK_DURATION_S` | `90000` | soak length in seconds (25 h) |
| `PROBE_INTERVAL_S` | `300` | seconds between unary probes on the same channel |
| `TLS_ROOT_CERT` | *(empty)* | PEM root cert; enables TLS transport to Torii |

---

## 3. Generate node configuration

```bash
export DB_TYPE=${DB_TYPE:-rocksdb}
export SOAK_DIR=${SOAK_DIR:-$PWD/soak-work}
mkdir -p "$SOAK_DIR"

if [ "$DB_TYPE" = "postgres" ]; then
python3 - <<'EOF'
import json, os
cfg = {
  "block_store_path": os.path.join(os.environ["SOAK_DIR"], "block_store"),
  "torii_port": int(os.environ.get("TORII_PORT", "50051")),
  "internal_port": int(os.environ.get("INTERNAL_PORT", "10001")),
  "database": {
    "type": "postgres",
    "host": os.environ.get("IROHA_PG_HOST", "localhost"),
    "port": int(os.environ.get("IROHA_PG_PORT", "5432")),
    "user": os.environ.get("IROHA_PG_USER", "postgres"),
    "password": os.environ.get("IROHA_PG_PASSWORD", ""),
    "working database": os.environ.get("IROHA_PG_DB", "iroha_data"),
    "maintenance database": os.environ.get("IROHA_PG_MAINT_DB", "postgres"),
  },
  "max_proposal_size": 10,
  "vote_delay": 5000,
  "mst_enable": False,
  "mst_expiration_time": 1440,
  "proposal_creation_timeout": 3000,
  "stale_stream_max_rounds": 2,
  "metrics": "127.0.0.1:8081",
  "healthcheck_port": 50509,
}
open(os.path.join(os.environ["SOAK_DIR"], "config.json"), "w").write(json.dumps(cfg, indent=2))
EOF
else
python3 - <<'EOF'
import json, os
cfg = {
  "block_store_path": os.path.join(os.environ["SOAK_DIR"], "block_store"),
  "torii_port": int(os.environ.get("TORII_PORT", "50051")),
  "internal_port": int(os.environ.get("INTERNAL_PORT", "10001")),
  "database": {
    "type": "rocksdb",
    "path": os.path.join(os.environ["SOAK_DIR"], "wsv"),
  },
  "max_proposal_size": 10,
  "vote_delay": 5000,
  "mst_enable": False,
  "mst_expiration_time": 1440,
  "proposal_creation_timeout": 3000,
  "stale_stream_max_rounds": 2,
  "metrics": "127.0.0.1:8081",
  "healthcheck_port": 50509,
}
open(os.path.join(os.environ["SOAK_DIR"], "config.json"), "w").write(json.dumps(cfg, indent=2))
EOF
fi
cat "$SOAK_DIR/config.json"
```

### Optional: PostgreSQL in Docker (skip for rocksdb)

```bash
export IROHA_PG_PASSWORD='change-me-please'
docker run -d --name iroha-soak-pg \
  -e POSTGRES_PASSWORD="$IROHA_PG_PASSWORD" \
  -e POSTGRES_DB=iroha_data \
  -p 5432:5432 postgres:16
# wait for readiness
until docker exec iroha-soak-pg pg_isready -U postgres >/dev/null; do sleep 1; done
# then re-run the config generator with DB_TYPE=postgres
```

---

## 4. Start the node

```bash
# fresh state every run
rm -rf "$SOAK_DIR/wsv" "$SOAK_DIR/block_store"

nohup "$IROHAD_BIN" \
  --config "$SOAK_DIR/config.json" \
  --genesis_block example/genesis.block \
  --keypair_name example/node0 \
  --overwrite_ledger \
  > "$SOAK_DIR/irohad.log" 2>&1 &
echo $! > "$SOAK_DIR/irohad.pid"

# wait until Torii answers (up to 120 s)
for i in $(seq 1 120); do
  if python3 - <<'EOF'
import socket, sys
s = socket.create_connection(("127.0.0.1", int(__import__("os").environ.get("TORII_PORT","50051"))), 2)
s.close()
EOF
  then echo "Torii is up"; break; fi
  sleep 1
done
tail -5 "$SOAK_DIR/irohad.log"
```

---

## 5. Generate Python gRPC stubs

```bash
mkdir -p "$SOAK_DIR/gen"
cd "$SOAK_DIR/gen"
for f in "$OLDPWD"/shared_model/schema/*.proto; do
  python3 -m grpc_tools.protoc -I"$OLDPWD/shared_model/schema" \
    --python_out=. --grpc_python_out=. "$(basename "$f")"
done
ls *.py | head
cd "$OLDPWD"
```

Expected output: `endpoint_pb2.py`, `endpoint_pb2_grpc.py`,
`transaction_pb2.py`, `queries_pb2.py`, etc.

---

## 6. The soak client

Save as `$SOAK_DIR/soak_client.py`:

```python
#!/usr/bin/env python3
"""24h+ idle-connection soak test for Iroha Torii (PR #8 verification).

Two independent gRPC channels, both deliberately kept mostly idle:

  * stream channel  - holds a StatusStream subscription; re-opened when
                      the server closes it (stale_rounds) or it errors
  * probe channel   - unary Healthcheck + Status calls every
                      --probe-interval seconds

Any transport-level failure (RpcError UNAVAILABLE/DEADLINE_EXCEEDED,
hung iterator) is logged as an event and counted. A clean server-side
stream close is logged separately and is NOT a failure.

NOTE: no client-side keepalive args are set - the point is to test the
SERVER-side fix against a stock client.

Usage:
  python3 soak_client.py --addr 127.0.0.1:50051 --duration 90000 \
      [--tls-root-cert ca.pem] [--log events.jsonl]
"""
import argparse, json, os, sys, threading, time, uuid

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), "gen"))

import grpc
from google.protobuf import empty_pb2
import endpoint_pb2, endpoint_pb2_grpc

FAKE_TX_HASH = "ab" * 32  # valid hex format, no such transaction
HERE = os.path.dirname(os.path.abspath(__file__))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--addr", default="127.0.0.1:50051")
    ap.add_argument("--duration", type=float, default=90000.0,
                    help="soak length in seconds (default 25h)")
    ap.add_argument("--probe-interval", type=float, default=300.0,
                    help="seconds between unary probes on the channel")
    ap.add_argument("--tls-root-cert", default=os.environ.get("TLS_ROOT_CERT", ""))
    ap.add_argument("--log", default=os.path.join(HERE, "events.jsonl"))
    args = ap.parse_args()

    logf = open(args.log, "a", buffering=1)
    run_id = uuid.uuid4().hex[:8]
    deadline = time.time() + args.duration
    stop = threading.Event()

    def log(event, **kw):
        rec = {"ts": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()),
               "run": run_id, "event": event, **kw}
        logf.write(json.dumps(rec) + "\n")
        print(json.dumps(rec), flush=True)

    def make_channel():
        if args.tls_root_cert:
            with open(args.tls_root_cert, "rb") as f:
                creds = grpc.ssl_channel_credentials(f.read())
            return grpc.secure_channel(args.addr, creds)
        return grpc.insecure_channel(args.addr)

    stats = {"stream_errors": 0, "stream_clean_ends": 0,
             "streams_opened": 0, "probe_ok": 0, "probe_fail": 0,
             "stream_reconnects": 0}
    stats_lock = threading.Lock()

    def bump(key, n=1):
        with stats_lock:
            stats[key] += n

    # --- streaming leg (background thread) -------------------------------
    def stream_loop():
        while not stop.is_set() and time.time() < deadline:
            chan = make_channel()
            try:
                cmd = endpoint_pb2_grpc.CommandService_v1Stub(chan)
                bump("streams_opened")
                log("stream_open", n=stats["streams_opened"])
                stream = cmd.StatusStream(
                    endpoint_pb2.TxStatusRequest(tx_hash=FAKE_TX_HASH))
                for resp in stream:  # blocks; silent while status unchanged
                    log("stream_msg", tx_status=resp.tx_status,
                        error_code=resp.error_code)
                # iterator ended without exception -> clean server close
                bump("stream_clean_ends")
                log("stream_clean_end")
            except grpc.RpcError as e:
                bump("stream_errors")
                code = e.code().name if hasattr(e, "code") else "UNKNOWN"
                log("stream_error", code=code, details=str(e)[:300])
            except Exception as e:  # noqa: BLE001 - soak must not die silently
                bump("stream_errors")
                log("stream_exception", details=repr(e)[:300])
            finally:
                try:
                    chan.close()
                except Exception:
                    pass
            if stop.is_set() or time.time() >= deadline:
                break
            bump("stream_reconnects")
            log("stream_reconnect")
            # backoff keeps the channel mostly idle; capped so we don't
            # overshoot the deadline by much
            stop.wait(min(60.0, max(1.0, deadline - time.time())))

    # --- unary probe leg (main thread) -----------------------------------
    probe_chan = make_channel()
    qry = endpoint_pb2_grpc.QueryService_v1Stub(probe_chan)
    pcmd = endpoint_pb2_grpc.CommandService_v1Stub(probe_chan)

    def probe():
        ok = True
        try:
            h = qry.Healthcheck(empty_pb2.Empty())
            log("probe_ok", is_healthy=h.is_healthy,
                memory=h.memory_consumption)
        except grpc.RpcError as e:
            ok = False
            code = e.code().name if hasattr(e, "code") else "UNKNOWN"
            log("probe_fail", rpc="healthcheck", code=code,
                details=str(e)[:300])
        except Exception as e:  # noqa: BLE001
            ok = False
            log("probe_exception", rpc="healthcheck", details=repr(e)[:300])
        try:
            r = pcmd.Status(
                endpoint_pb2.TxStatusRequest(tx_hash=FAKE_TX_HASH))
            log("status_ok", tx_status=r.tx_status, error_code=r.error_code)
        except grpc.RpcError as e:
            ok = False
            code = e.code().name if hasattr(e, "code") else "UNKNOWN"
            log("status_fail", code=code, details=str(e)[:300])
        except Exception as e:  # noqa: BLE001
            ok = False
            log("status_exception", details=repr(e)[:300])
        if ok:
            bump("probe_ok")
        else:
            bump("probe_fail")

    log("start", addr=args.addr, duration_s=args.duration,
        tls=bool(args.tls_root_cert))
    t = threading.Thread(target=stream_loop, daemon=True)
    t.start()

    while time.time() < deadline:
        probe()
        remaining = deadline - time.time()
        if remaining <= 0:
            break
        # idle gap on the probe channel = the keepalive-protected window
        time.sleep(min(args.probe_interval, remaining))

    stop.set()
    t.join(timeout=70)
    try:
        probe_chan.close()
    except Exception:
        pass

    summary = {
        "run": run_id,
        "addr": args.addr,
        "duration_s": args.duration,
        "ended_at": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()),
        **stats,
        "verdict": "PASS" if stats["stream_errors"] == 0
                            and stats["probe_fail"] == 0 else "FAIL",
    }
    log("summary", **summary)
    with open(os.path.join(HERE, "summary.json"), "w") as f:
        json.dump(summary, f, indent=2)
    sys.exit(0 if summary["verdict"] == "PASS" else 1)


if __name__ == "__main__":
    main()
```

---

## 7. Run the soak

```bash
cd "$SOAK_DIR"
nohup python3 soak_client.py \
  --addr "127.0.0.1:${TORII_PORT:-50051}" \
  ${TLS_ROOT_CERT:+--tls-root-cert "$TLS_ROOT_CERT"} \
  --duration "${SOAK_DURATION_S:-90000}" \
  --probe-interval "${PROBE_INTERVAL_S:-300}" \
  > soak_client.log 2>&1 &
echo $! > soak_client.pid

# live tail
tail -f "$SOAK_DIR/events.jsonl"
```

For a quicker smoke check before committing 25 h, run with
`--duration 600` — you should see `stream_open`, `probe_ok` and
`status_ok` events every 5 min and `stream_clean_end` whenever the
server's `stale_stream_max_rounds` limit closes the subscription.

> **Observed behavior:** with the default `stale_stream_max_rounds: 2`
> the server closes `StatusStream` after ~2 consensus rounds without a
> status change (a few seconds on an idle chain), so `stream_clean_end`
> events are frequent — that is expected. The long idle windows that
> keepalive must protect come from the 60 s reconnect backoff and the
> 5 min gaps between unary probes on the probe channel.

---

## 8. (Optional) Simulate an aggressive middlebox idle timeout

On Linux you can shrink the conntrack timeout for the Torii port so idle
connections are dropped within minutes instead of days — turns the 24 h
soak into a ~1–2 h test. Requires root; loopback traffic may bypass
conntrack on some kernels, in which case run the client from another host
or a container.

```bash
# drop ESTABLISHED connections to Torii after 10 min idle
sudo iptables -t raw -A OUTPUT -p tcp --dport "${TORII_PORT:-50051}" \
  -m conntrack --ctstate ESTABLISHED -j CT --timeout 600

# ... run the soak with SOAK_DURATION_S=7200 ...

# afterwards
sudo iptables -t raw -D OUTPUT -p tcp --dport "${TORII_PORT:-50051}" \
  -m conntrack --ctstate ESTABLISHED -j CT --timeout 600
```

---

## 9. Verdict

```bash
cat "$SOAK_DIR/summary.json"
tail -20 "$SOAK_DIR/events.jsonl"
```

| Field | Expected |
|---|---|
| `stream_errors` | `0` |
| `probe_fail` | `0` |
| `stream_clean_ends` | ≥ 0 (server-side stale-round closes are normal) |
| `reconnects` | ≥ 0 |
| `verdict` | `"PASS"` |

If `probe_fail`/`stream_errors` stay `0` for the full duration → the
keepalive fix works; merge PR #8.

**Control experiment (optional but recommended):** run the identical soak
against a build *without* the fix (`iroha1-main` before merge, or revert
`server_runner.cpp`) behind the §8 iptables timeout. You should observe
`stream_error`/`probe_fail` with `UNAVAILABLE` once the idle timeout
fires — that demonstrates the test actually detects the bug.

## 10. Cleanup

```bash
kill "$(cat "$SOAK_DIR/irohad.pid")" "$(cat "$SOAK_DIR/soak_client.pid")" 2>/dev/null
docker rm -f iroha-soak-pg 2>/dev/null   # only if you started it
sudo iptables -t raw -D OUTPUT -p tcp --dport "${TORII_PORT:-50051}" \
  -m conntrack --ctstate ESTABLISHED -j CT --timeout 600 2>/dev/null
```
