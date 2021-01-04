#
# cc - C Containers library
#
# cc is, per 17 USC § 101, a work of the U.S. Government and is not subject to
# copyright protection in the United States.
#
# DISTRIBUTION STATEMENT A.  Approved for public release; distribution is
# unlimited.  Granted clearance per 88ABW-2020-3430.
#

FROM alpine:edge AS builder
LABEL name="cc"

RUN apk update && apk add --no-cache \
    cmake \
    gcc \
    g++ \
    make \
    valgrind

# Install the grun library

COPY . /source/
WORKDIR /build
RUN cmake -DCMAKE_BUILD_TYPE=Debug /source && make

# Set up the execution environment

WORKDIR /build/test
ENTRYPOINT ["/usr/bin/valgrind"]
