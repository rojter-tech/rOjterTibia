FROM debian:wheezy AS build
RUN apt-get update && apt-get install -y \
	git \
	cmake \
	build-essential \
	liblua5.2-dev \
	libgmp3-dev \
	libmysqlclient-dev \
	libboost-system-dev \
	libboost-iostreams-dev \
	libpugixml-dev \
	libcrypto++-dev

COPY cmake /usr/src/rojtertibia/cmake/
COPY src /usr/src/rojtertibia/src/
COPY CMakeLists.txt /usr/src/rojtertibia/
WORKDIR /usr/src/rojtertibia/build
RUN cmake .. && make -j8

FROM debian:wheezy
RUN apk add --no-cache --repository http://dl-3.alpinelinux.org/alpine/edge/testing/ \
  libboost-iostreams \
  libboost-system \
  libcrypto++ \
  libgmp3 \
  liblua5.2 \
  libmysqlclient \
  libpugixml

RUN ln -s /usr/lib/libcryptopp.so /usr/lib/libcryptopp.so.5.6
COPY --from=build /usr/src/rojtertibia/build/tfs /bin/tfs
COPY data /srv/data/
COPY LICENSE README.md *.dist *.sql key.pem /srv/

EXPOSE 7173 7174
WORKDIR /srv
VOLUME /srv
ENTRYPOINT ["/bin/tfs"]
