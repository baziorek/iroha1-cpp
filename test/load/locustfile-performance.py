import os
import time
import binascii

from locust import User, TaskSet, events, task, constant

import grpc.experimental.gevent as grpc_gevent
grpc_gevent.init_gevent()

import grpc
from iroha import Iroha, IrohaGrpc
from iroha import IrohaCrypto as ic
import iroha
import common.writer

import random
import gevent


HOSTNAME = os.environ['HOSTNAME']
ADMIN_PRIVATE_KEY = 'f101537e319568c765b2cc89698325604991dca57b9716b58016b253506cab70'

TXS = dict() # hash -> sent time
COMMITTED = set()
SENT = set()
BLOCKS = set()


def ascii_hash(tx):
    return binascii.hexlify(ic.hash(tx)).decode('ascii')

class IrohaClient(IrohaGrpc):
    """
    Simple, sample Iroha gRPC client implementation that wraps IrohaGrpc and
    fires locust events on request_success and request_failure, so that all requests
    gets tracked in locust's statistics.
    """
    def send_tx_wait(self, transaction):
        """
        Send a transaction to Iroha if there are few transactions in the queue to be committed
        :param transaction: protobuf Transaction
        :return: None
        """
        while len(SENT) - len(COMMITTED) > 100:
            time.sleep(0.01)

        hex_hash = ascii_hash(transaction)
        start_time = time.time()

        try:
            self.send_tx(transaction)
            SENT.add(hex_hash)
            TXS[hex_hash] = start_time
        except grpc.RpcError as e:
            total_time = int((time.time() - start_time) * 1000)
            events.request.fire(request_type="grpc", name='send_tx_wait', response_time=total_time, response_length=0, exception=e, tx_hash=hex_hash)


def block_listener(host):
    iroha_api = iroha.Iroha("admin@test")
    net = IrohaGrpc(host)
    query = iroha_api.blocks_query()
    ic.sign_query(query, ADMIN_PRIVATE_KEY)
    print("Listeting blocks")
    for block in net.send_blocks_stream_query(query):
        BLOCKS.add(block.block_response.block.block_v1.payload.height)
        hashes = block.block_response.block.block_v1.payload.rejected_transactions_hashes
        txs =  block.block_response.block.block_v1.payload.transactions
        for tx in txs:
            hashes.append(ascii_hash(tx))

        for hash in hashes:
            if hash not in TXS.keys():
                continue
            start_time = TXS[hash]
            COMMITTED.add(hash)
            del TXS[hash]
            total_time = int((time.time() - start_time) * 1000)
            try:
                events.request.fire(request_type="grpc", name='send_tx_wait', response_time=total_time, response_length=0, exception=None, tx_hash=hash, sent=start_time, committed=time.time())
            except Exception as e:
                print(e)

class IrohaTasks(TaskSet):
    """
    Tasks executed by each simulated Iroha user.
    """
    @task
    def send_tx(self):
        print("Locust instance (%r) executing my_task" % (self.user))
        print("""
        \n
            Sent: {}
            Committed: {}
            Diff: {}
            Blocks: {}\n
            """.format(len(SENT), len(COMMITTED), len(SENT) - len(COMMITTED), len(BLOCKS)))
        iroha = Iroha('admin@test')

        desc = str(random.random())
        tx = iroha.transaction([iroha.command(
            'TransferAsset', src_account_id='admin@test', dest_account_id='test@test', asset_id='coin#test',
            amount='0.01', description=desc
        )])

        ic.sign_transaction(tx, ADMIN_PRIVATE_KEY)
        self.user.client.send_tx_wait(tx)


class ApiUser(User):
    """
    Locust user that owns an Iroha gRPC client and a block listener,
    so that transactions are tracked in locust's statistics.
    """
    host = "127.0.0.1:50051"
    wait_time = constant(1)
    tasks = [IrohaTasks]

    def on_start(self):
        self.client = IrohaClient(self.host)
        gevent.spawn(block_listener, self.host)
