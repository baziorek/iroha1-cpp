FROM python:3.6.6-alpine3.8@sha256:ac1cf9845670783d36e86f793ad163f479ac2703c497761e5e84646868516ba9

COPY requirements-hashes.txt /tmp/requirements-hashes.txt
RUN apk --no-cache add g++ zeromq-dev \
      && pip install --require-hashes -r /tmp/requirements-hashes.txt

EXPOSE 8089 5557 5558

ENTRYPOINT ["/usr/local/bin/locust"]

