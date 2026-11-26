import os
import time
import grpc
from iroha import Iroha, IrohaGrpc
from iroha import IrohaCrypto as ic

from locust import User, TaskSet, events, task, between

import common.writer


HOSTNAME = os.environ['HOSTNAME']
ADMIN_PRIVATE_KEY = 'f101537e319568c765b2cc89698325604991dca57b9716b58016b253506cab70'

class IrohaClient(IrohaGrpc):
    """
    Simple, sample Iroha gRPC client implementation that wraps IrohaGrpc and
    fires locust events on request_success and request_failure, so that all requests
    gets tracked in locust's statistics.
    """
    def __getattribute__(self, name):
        func = IrohaGrpc.__getattribute__(self, name)
        if hasattr(func, '__call__'):
            def wrapper(*args, **kwargs):
                start_time = time.time()
                try:
                    result = func(*args, **kwargs)
                except grpc.RpcError as e:
                    total_time = int((time.time() - start_time) * 1000)
                    events.request.fire(request_type="grpc", name=name, response_time=total_time, response_length=0, exception=e)
                else:
                    total_time = int((time.time() - start_time) * 1000)
                    events.request.fire(request_type="grpc", name=name, response_time=total_time, response_length=0, exception=None)
                    # In this example, I've hardcoded response_length=0. If we would want the response length to be
                    # reported correctly in the statistics, we would probably need to hook in at a lower level
                    return result

            return wrapper
        else:
            return func


class IrohaTasks(TaskSet):
    """
    Tasks executed by each simulated Iroha user.
    """
    @task
    def send_tx(self):
        iroha = Iroha('admin@test')

        tx = iroha.transaction([iroha.command(
            'TransferAsset', src_account_id='admin@test', dest_account_id='test@test', asset_id='coin#test',
            amount='0.01', description=HOSTNAME
        )])
        ic.sign_transaction(tx, ADMIN_PRIVATE_KEY)

        self.user.client.send_tx(tx)


class ApiUser(User):
    """
    Locust user that owns an Iroha gRPC client, so that all requests
    get tracked in locust's statistics.
    """
    host = "127.0.0.1:50051"
    wait_time = between(0.001, 0.01)
    tasks = [IrohaTasks]

    def on_start(self):
        self.client = IrohaClient(self.host)
