FROM alpine:3.18

RUN apk update && \
    apk add git \
            cmake \
            python3 \
            build-base \
            bsd-compat-headers \
            newlib-arm-none-eabi \
            g++-arm-none-eabi \
            su-exec && \
    chmod +s /sbin/su-exec && \
    adduser builder -u 1000 -D

ARG SDK_PATH=/usr/share/pico_sdk
RUN git clone --depth 1 --branch 1.5.1 https://github.com/raspberrypi/pico-sdk $SDK_PATH && \
    cd $SDK_PATH && \
    git submodule update --init

ENV PICO_SDK_PATH=$SDK_PATH

USER builder

