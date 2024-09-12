FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get -q update \
&& apt-get -qy install --no-install-recommends --no-install-suggests \
gcc \
binutils \
python3 \
python3-pip

RUN pip install conan
