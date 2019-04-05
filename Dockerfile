FROM nvidia/cuda:8.0-cudnn5-devel

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update -y \
    && apt-get install -y \
    build-essential \
    apt-utils \
    openssl \
    git \
    cmake \
    git \
    curl \
    vim \
    ca-certificates \
    libjpeg-dev \
    libfreetype6-dev \
    zlib1g-dev \
    openssh-client \
    bash \
    g++ \
    tzdata \
    mariadb-client \
    gcc \
    xz-utils \
    file \
    sudo \
    tmux \
    libmecab-dev \
    wget \
    unzip \
    lsof \
    lsb-core \
    libatlas-base-dev \
    libopencv-dev \
    python-opencv \
    python-pip \
    python3-pip \
    ffmpeg \
    zip \
    cython \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

ENV PYTHONPATH /usr/local/python/openpose

WORKDIR /root

# openpose
RUN git clone https://github.com/CMU-Perceptual-Computing-Lab/openpose.git
WORKDIR /root/openpose
RUN git checkout tags/v1.4.0 \
    && git submodule update --init --recursive \
    && sync \
    && sleep 1 \
    && bash ./ubuntu/install_caffe_and_openpose_JetsonTX2_JetPack3.1.sh

# python API
RUN cd build && cmake -DBUILD_PYTHON=true ..
RUN cd build && make -j `nproc`
RUN cd build && sudo make install