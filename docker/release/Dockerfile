FROM ubuntu:20.04@sha256:8feb4d8ca5354def3d8fce243717141ce31e2c428701f6682bd2fafe15388214

# Install iroha and iroha_shepherd
COPY iroha.deb /tmp/iroha.deb
COPY iroha_shepherd.deb /tmp/iroha_shepherd.deb
RUN set -e; apt-get update; \
    apt-get install -y /tmp/iroha.deb /tmp/iroha_shepherd.deb; \
    rm -f /tmp/iroha.deb /tmp/iroha_shepherd.deb; \
    apt-get -y clean && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /opt/iroha_data

COPY entrypoint.sh wait-for-it.sh /
RUN chmod +x /entrypoint.sh /wait-for-it.sh
ENTRYPOINT ["/entrypoint.sh"]
CMD ["irohad"]
