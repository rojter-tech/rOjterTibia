FROM ubuntu:latest AS build

RUN apt-get update -y && apt-get install -y \
  git \
  cmake \
  build-essential \
  liblua5.2-dev \
  libgmp3-dev \
  libmysqlclient-dev \
  libboost-system-dev \
  libboost-iostreams-dev \
  libpugixml-dev \
  libcrypto++-dev \
  libluajit-5.1-dev

COPY cmake /usr/src/rojtertibia/cmake/
COPY src /usr/src/rojtertibia/src/
COPY CMakeLists.txt /usr/src/rojtertibia/
WORKDIR /usr/src/rojtertibia/build
RUN cmake .. && make -j8

FROM ubuntu:latest

RUN apt-get update -y && apt-get install -y \
  libboost-system \
  libboost-iostreams \
  libcrypto++ \
  libluajit-5.1-common \
  libmysqlclient \
  libpugixml

RUN ln -s /usr/lib/libcryptopp.so /usr/lib/libcryptopp.so.5.6
COPY --from=build /usr/src/rojtertibia/build/tfs /bin/tfs
COPY data /srv/data/
COPY LICENSE README.md *.dist *.sql key.pem /srv/
COPY config.lua.dist /srv/config.lua

EXPOSE 7171 7172
WORKDIR /srv
VOLUME /srv
ENTRYPOINT ["/bin/tfs"]
