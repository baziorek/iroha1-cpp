# python:3.10 is the newest interpreter with prebuilt wheels for
# grpcio-tools==1.48.2 (the last release compatible with iroha's
# protobuf<=3.20.1 pin); newer Pythons would need an sdist build.
FROM python:3.10-slim@sha256:6ff506466f8b1e981719b468cc0023b8ef000b983cdec0eafe119d01f379e31d

COPY requirements-hashes.txt /tmp/requirements-hashes.txt
RUN pip install --no-cache-dir --require-hashes -r /tmp/requirements-hashes.txt

EXPOSE 8089 5557 5558

ENTRYPOINT ["/usr/local/bin/locust"]
