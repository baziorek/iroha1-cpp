FROM ubuntu:24.04@sha256:562111b88caa79c71391c0c4d3c2114a7d5c35915c20561fd893cb1e07929e6b

# number of concurrent threads during build
# usage: docker build --build-arg PARALLELISM=8 -t name/name .
ARG PARALLELISM=1

ENV IROHA_HOME /opt/iroha
ENV IROHA_BUILD /opt/iroha/build

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && \
    apt-get -y --no-install-recommends install \
            apt-utils software-properties-common wget gpg-agent \
            libtool \
            # compilers (gcc-13 is the default gcc on ubuntu 24.04; keep both for CI matrix)
            build-essential g++-13 g++-14 cmake ninja-build \
            gdb gdbserver \
            # CI dependencies
            git ssh tar gzip ca-certificates gnupg \
            # code coverage
            lcov \
            # Python3
            python3-dev python3-pip python-is-python3 \
            # other
            curl file ccache libssl-dev \
            bison flex \
            gcovr cppcheck doxygen rsync graphviz graphviz-dev vim zip unzip pkg-config \
            jq autoconf \
            postgresql postgresql-contrib; \
    if [ $(uname -m) = "x86_64" ] ;then \
        apt-get -y --no-install-recommends install \
            clang lldb libc++-dev libc++abi-dev clang-format; \
    fi; \
    apt-get -y clean && \
    rm -rf /var/lib/apt/lists/* && \
    rm -rf /var/cache/apt/archives/*

RUN cd /opt/ && \
    git clone https://github.com/sobolevn/git-secret.git git-secret && \
    cd git-secret && make build && \
    PREFIX="/usr/local" make install

## golang stuff for iroha+burrow
RUN curl -fL https://golang.org/dl/go1.17.2.linux-$(dpkg --print-architecture).tar.gz | tar -C /opt -xz
ENV GOPATH=/opt/gopath
RUN mkdir ${GOPATH}
ENV PATH=${PATH}:/opt/go/bin:${GOPATH}/bin
RUN go install github.com/golang/protobuf/protoc-gen-go@v1.5.4
RUN export PATH="$PATH:$(go env GOPATH)/bin" # according to: https://grpc.io/docs/languages/go/quickstart/

## Rust-lang stuff for iroha+ursa
ENV RUSTUP_HOME=/opt/rust
ENV CARGO_HOME=/opt/rust
RUN set -e; \
    arch="$(dpkg --print-architecture)"; \
    case "$arch" in \
        amd64)   target=x86_64-unknown-linux-gnu;        sha=6aeece6993e902708983b209d04c0d1dbb14ebb405ddb87def578d41f920f56d ;; \
        arm64)   target=aarch64-unknown-linux-gnu;      sha=1cffbf51e63e634c746f741de50649bbbcbd9dbe1de363c9ecef64e278dba2b2 ;; \
        armhf)   target=armv7-unknown-linux-gnueabihf;  sha=3c4114923305f1cd3b96ce3454e9e549ad4aa7c07c03aec73d1a785e98388bed ;; \
        i386)    target=i686-unknown-linux-gnu;         sha=0a6bed6e9f21192a51f83977716466895706059afb880500ff1d0e751ada5237 ;; \
        ppc64el) target=powerpc64le-unknown-linux-gnu;  sha=079430f58ad4da1d1f4f5f2f0bd321422373213246a93b3ddb53dad627f5aa38 ;; \
        s390x)   target=s390x-unknown-linux-gnu;        sha=e7f89da453c8ce5771c28279d1a01d5e83541d420695c74ec81a7ec5d287c51c ;; \
        riscv64) target=riscv64gc-unknown-linux-gnu;     sha=98034d7a478de8cdb9ecacb0330c943205dae867765287bc334abe3959913f21 ;; \
        *) echo "unsupported architecture: $arch" >&2; exit 1 ;; \
    esac; \
    curl --proto '=https' --tlsv1.2 -fsSL -o /tmp/rustup-init \
        "https://static.rust-lang.org/rustup/archive/1.27.1/${target}/rustup-init"; \
    echo "${sha}  /tmp/rustup-init" | sha256sum -c -; \
    chmod +x /tmp/rustup-init; \
    /tmp/rustup-init -y --no-modify-path --profile minimal; \
    rm -f /tmp/rustup-init; \
    echo 'source /opt/rust/env' >>/etc/profile; \
    echo 'source /opt/rust/env' >>/etc/bash.bashrc

## Allow access to database, trust local connections
# RUN sed -i /etc/postgresql/16/main/pg_hba.conf -Ee's,(^local\s+all\s+postgres\s+)\w+,\1trust,'
RUN echo " \n\
# TYPE  DATABASE        USER            ADDRESS                 METHOD \n\
local   all             all                                     trust \n\
host    all             all             127.0.0.1/32            trust \n\
host    all             all             ::1/128                 trust \n\
local   replication     all                                     trust \n\
host    replication     all             127.0.0.1/32            trust \n\
host    replication     all             ::1/128                 trust \n\
" > /etc/postgresql/16/main/pg_hba.conf

ENV PATH=${PATH}:/usr/lib/postgresql/16/bin/

## non-interactive adduser
##   -m = create home dir
##   -s = set default shell
##   iroha-ci = username
##   -U = create a group same as username
##   no password
##   (no -u 1000: the 'ubuntu' user already owns UID 1000 on ubuntu 24.04)
RUN useradd -ms /bin/bash iroha-ci -U && \
    usermod -aG postgres iroha-ci

WORKDIR /opt/iroha
RUN set -e; \
    chown -R iroha-ci:iroha-ci /opt/iroha; \
    chmod -R 777 /opt/iroha; \
    mkdir -p /tmp/ccache -m 777; \
    ccache --clear

USER iroha-ci
# CMD /bin/bash
