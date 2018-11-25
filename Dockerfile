FROM alpine:edge AS build
RUN apk add --no-cache --repository http://dl-3.alpinelinux.org/alpine/v3.6/main/ \
      boost-dev=1.62.0-r5 && \
    apk add --no-cache --repository http://dl-3.alpinelinux.org/alpine/v3.3/main/ \
      lua-dev=5.1.5-r4 && \
    apk add --no-cache --repository http://dl-3.alpinelinux.org/alpine/edge/main/ \
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
RUN apk add --no-cache --repository http://dl-3.alpinelinux.org/alpine/v3.7/main/ \
  boost-iostreams=1.62.0-r5 \
  boost-system=1.62.0-r5 \
  lua5.1
RUN apk add --no-cache --repository http://dl-3.alpinelinux.org/alpine/edge/main/ \
  gmp \
  luajit \
  mariadb-connector-c \
  libxml2

COPY --from=build /usr/src/rojtertibia/build/tfs /bin/tfs
COPY data /srv/data/
COPY LICENSE README.md *.dist *.sql key.pem /srv/
COPY config.lua.dist /srv/config.lua

EXPOSE 7171 7172
WORKDIR /srv
VOLUME /srv
ENTRYPOINT ["/bin/tfs"]