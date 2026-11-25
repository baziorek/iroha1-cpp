===============
Troubleshooting
===============

Restore after hard shutdown
---------------------------

::

	[2020-11-27 10:40:01.764013860][th:1][warning] Irohad/Storage/FlatFileBlockStorage Error while block deserialization: Unexpected end of string. Expected a value.

	^

::

	[2020-11-25 10:36:19.993552669][C][Init]: Irohad startup failed: WSV state (height 4576773) is more recent than block storage (height 4576772).

Such messages may appear if the node crashed while using file-based block store. Please copy the missing blocks from another node, in which case you can use ``--reuse_state`` flag for fast startup, or remove the blocks starting from the empty file and recreate WSV from existing blocks.

Client disconnects or "Rendezvous" errors after long idle periods
-------------------------------------------------------------------

If clients (especially long-lived status streams) periodically lose their
connection to Torii after many hours of inactivity, the cause is usually
an idle timeout on an intermediate hop (NAT, load balancer, proxy)
silently dropping the HTTP/2 connection.

This fork's ``irohad`` enables **gRPC keepalive pings** on all its servers
(Torii, internal ordering, utility): the server pings idle connections
every 60 seconds and closes dead ones after a 20-second timeout, so
clients reconnect cleanly instead of hanging on a half-open channel.

If you still see drops:

- make sure you run a build that includes the keepalive fix (this fork,
  ``iroha1-main`` branch);
- check whether a proxy/LB between client and node has its own idle
  timeout shorter than the keepalive interval and raise it;
- for status streams, prefer reconnecting with backoff — see
  ``stale_stream_max_rounds`` in :ref:`configuration`.
