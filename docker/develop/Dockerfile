FROM ubuntu:24.04@sha256:562111b88caa79c71391c0c4d3c2114a7d5c35915c20561fd893cb1e07929e6b

# number of concurrent threads during build
# usage: docker build --build-arg PARALLELISM=8 -t name/name .
ARG PARALLELISM=1

ENV IROHA_HOME /opt/iroha
ENV IROHA_BUILD /opt/iroha/build

RUN apt-get update && \
    apt-get -y --no-install-recommends install apt-utils software-properties-common wget gpg-agent; \
    apt-get -y clean && \
    rm -rf /var/lib/apt/lists/*

RUN set -e; \
    apt-get update && \
    apt-get -y --no-install-recommends install libtool \
        # compilers (gcc-13 is the default gcc on ubuntu 24.04; keep both for CI matrix)
        build-essential g++-13 g++-14 cmake ninja-build \
        # CI dependencies
        git ssh tar gzip ca-certificates gnupg \
        # code coverage
        lcov \
        # Python3
        python3-pip python-is-python3 \
        # other
        curl file gdb gdbserver ccache python3-dev libssl-dev bison flex \
        gcovr cppcheck doxygen rsync graphviz graphviz-dev unzip vim zip pkg-config; \
    apt-get -y clean && \
    rm -rf /var/lib/apt/lists/*

# compiler clang and libc++ only on x86_64, for debug purpose
RUN set -e; \
    if [ `uname -m` = "x86_64" ]; then \
      apt-get update && \
      apt-get -y --no-install-recommends install \
        clang lldb lld libc++-dev libc++abi-dev clang-format; \
      apt-get -y clean && \
      rm -rf /var/lib/apt/lists/*; \
    fi

# golang stuff
RUN curl https://dl.google.com/go/go1.17.2.linux-$(dpkg --print-architecture).tar.gz | tar -C /opt -xz
ENV GOPATH=/opt/gopath
RUN mkdir ${GOPATH}
ENV PATH=${PATH}:/opt/go/bin:${GOPATH}/bin
RUN go install github.com/golang/protobuf/protoc-gen-go@v1.5.4
RUN export PATH="$PATH:$(go env GOPATH)/bin" # according to: https://grpc.io/docs/languages/go/quickstart/
# RUN go mod init $(go env GOPATH) # according to: https://golangdocs.com/create-go-modules

## pip3 contains fresher versions of packages than apt
## hash-locked requirements (OpenSSF Scorecard Pinned-Dependencies)
COPY requirements-hashes.txt /tmp/requirements-hashes.txt
RUN pip3 install --no-cache-dir --require-hashes -r /tmp/requirements-hashes.txt

# install dependencies
COPY vcpkg /tmp/vcpkg-vars
RUN set -xe; \
    case "$(uname -m)" in  arm*|s390*|ppc64*)  export VCPKG_FORCE_SYSTEM_BINARIES=1 ;; esac; \
    /tmp/vcpkg-vars/build_iroha_deps.sh /tmp/vcpkg /tmp/vcpkg-vars; \
    /tmp/vcpkg/vcpkg export $(/tmp/vcpkg/vcpkg list | cut -d':' -f1 | tr '\n' ' ') --raw --output=dependencies; \
    mv /tmp/vcpkg/dependencies /opt/dependencies; \
    chmod +x /opt/dependencies/installed/*/tools/protobuf/protoc*; \
    unset VCPKG_FORCE_SYSTEM_BINARIES; \
    rm -rf /tmp/vcpkg*
    ##NOTE Newer packages like libpq may require newer cmake version, and will fail with no sanity error message when VCPKG_FORCE_SYSTEM_BINARIES=1;
    ##NOTE But vcpkg on platforms arm,s390,ppc64 require VCPKG_FORCE_SYSTEM_BINARIES=1

# install sonar cli
ENV SONAR_CLI_VERSION=3.3.0.1492
RUN set -e; \
    mkdir -p /opt/sonar; \
    curl -L -o /tmp/sonar.zip https://binaries.sonarsource.com/Distribution/sonar-scanner-cli/sonar-scanner-cli-${SONAR_CLI_VERSION}-linux.zip; \
    unzip -o -d /tmp/sonar-scanner /tmp/sonar.zip; \
    mv /tmp/sonar-scanner/sonar-scanner-${SONAR_CLI_VERSION}-linux /opt/sonar/scanner; \
    ln -s -f /opt/sonar/scanner/bin/sonar-scanner /usr/local/bin/sonar-scanner; \
    rm -rf /tmp/sonar*

# fetch lcov reports converter
RUN set -e; \
    curl -L -o /tmp/lcov_cobertura.py https://raw.githubusercontent.com/eriwen/lcov-to-cobertura-xml/8c55cd11f80a21e7e46f20f8c81fcde0bf11f5e5/lcov_cobertura/lcov_cobertura.py

# OpenJRE 8
RUN set -e; \
    apt-get update; \
    apt-get -y install openjdk-17-jre-headless; \
    apt-get -y clean && \
    rm -rf /var/lib/apt/lists/*; \
    java -version


# install rust (latest stable via rustup, needed e.g. by ursa)
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
ENV PATH=${PATH}:/opt/rust/bin

# non-interactive adduser
#   -m = create home dir
#   -s = set default shell
#   iroha-ci = username
#   -u = userid, default for Ubuntu is 1000
#   -U = create a group same as username
#   no password
RUN useradd -ms /bin/bash iroha-ci -U  ## no -u 1000: taken by "ubuntu" user on 24.04

WORKDIR /opt/iroha
RUN set -e; \
    chmod -R 777 /opt/iroha; \
    mkdir -p /tmp/ccache -m 777; \
    ccache --clear


USER iroha-ci
ENV CMAKE_TOOLCHAIN_FILE /opt/dependencies/scripts/buildsystems/vcpkg.cmake
CMD ["/bin/bash"]
