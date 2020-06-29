FROM ubuntu:18.04

RUN apt-get update

RUN apt-get install -y apt-utils

RUN apt-get install -y build-essential \
  clang \
  cmake \
  git \
  net-tools \
  ruby \
  vim \
  wget \
  zsh

RUN apt-get install -y curl

RUN apt-get install -y python3 python-pip python3-pip

RUN apt-get install -y flex bison gperf python-setuptools ninja-build ccache libffi-dev libssl-dev dfu-util

RUN mkdir ~/esp

WORKDIR /root/esp

RUN git clone --recursive https://github.com/espressif/esp-idf.git

WORKDIR /root/esp/esp-idf

RUN ./install.sh

RUN . ./export.sh

COPY .zshrc /root
COPY .vimrc /root

WORKDIR /work

CMD ["/bin/zsh"]
