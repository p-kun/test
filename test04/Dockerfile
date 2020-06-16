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

RUN apt-get install -y python3

COPY .zshrc /root
COPY .vimrc /root

WORKDIR /work

CMD ["/bin/zsh"]
