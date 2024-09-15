FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get -q update \
&& apt-get -qy install --no-install-recommends --no-install-suggests \
gcc \
g++ \
binutils \
make \
cmake \
git \
python3 \
python3-pip

RUN pip install conan

COPY .github/conan.profile /root/.conan2/profiles/default
COPY .github/conan.profile /github/home/.conan2/profiles/default
COPY conanfile.py /app/deps/conanfile.py

RUN conan install --version='0.0.1' --build=missing /app/deps/conanfile.py