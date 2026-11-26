from datetime import datetime
from influxdb import InfluxDBClient
from locust import events


class InfluxDBWriter(object):
    """
    InfluxDB writer for locust events
    """

    def __init__(self):
        self._client = InfluxDBClient(host='influxdb', database='influxdb', use_udp=True)
        self._user_count = 0

    def spawning_complete(self, user_count, **kw):
        self._user_count = user_count

    def request(self, request_type, name, response_time, response_length, exception, tx_hash=None, sent=None, committed=None, **kw):
        now = datetime.now().isoformat()
        if exception is None:
            measurement = "request_success_duration"
            fields = {
                "value": response_time,
                "tx_hash": tx_hash,
                "sent": sent,
                "committed": committed
            }
        else:
            measurement = "request_failure_duration"
            fields = {
                "value": response_time,
                "tx_hash": tx_hash
            }
        points = [{
            "measurement": measurement,
            "tags": {
                "request_type": request_type,
                "name": name
            },
            "time": now,
            "fields": fields
        },
        {
            "measurement": "user_count",
            "time": now,
            "fields": {
                "value": self._user_count
            }
        }]
        self._client.write_points(points)


writer = InfluxDBWriter()
events.request.add_listener(writer.request)
events.spawning_complete.add_listener(writer.spawning_complete)
