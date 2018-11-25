FROM alpine:edge AS otbuild
RUN apk add --no-cache --repository http://dl-3.alpinelinux.org/alpine/v3.8/main/ \
      boost-dev=1.66.0-r0 && \
    apk add --no-cache --repository http://dl-3.alpinelinux.org/alpine/v3.3/main/ \
      lua-dev=5.1.5-r4 && \
    apk add --no-cache --repository http://dl-3.alpinelinux.org/alpine/edge/main/ \
      binutils \
      build-base \
      make \
      cmake \
      clang \
      libxml2-dev \
      gcc \
      gmp-dev \
      lua5.1-dev \
      luajit-dev \
      make \
      mariadb-connector-c-dev

COPY cmake /usr/src/rojtertibia/cmake/
COPY src /usr/src/rojtertibia/src/
COPY CMakeLists.txt /usr/src/rojtertibia/
WORKDIR /usr/src/rojtertibia/build
RUN cmake .. && make -j16

FROM alpine:edge
RUN apk add --no-cache --repository http://dl-3.alpinelinux.org/alpine/v3.8/main/ \
      boost=1.66.0-r0 \
      lua5.1 && \
    apk add --no-cache --repository http://dl-3.alpinelinux.org/alpine/edge/main/ \
      gmp \
      luajit \
      mariadb-connector-c \
      libxml2

COPY --from=otbuild /usr/src/rojtertibia/build/src/otserv /bin/otserv
COPY data /srv/data/
COPY LICENSE README.md /srv/
COPY config.lua.dist /srv/config.lua

EXPOSE 7171 7172
WORKDIR /srv
VOLUME /srv
ENTRYPOINT ["/bin/otserv"]