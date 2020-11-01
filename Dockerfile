FROM ubuntu:18.04

WORKDIR /advanced-compiler
RUN mkdir downloads && cd downloads

RUN  apt-get update \
  && apt-get install -y wget \
  && apt-get install -y xz-utils \
  && apt-get install -y cmake \
  && apt-get install -y ninja-build \
  && apt-get install -y clang-9 \
  && apt-get install -y build-essential \
  && apt-get install -y python \
  && apt-get clean -y \
  && apt-get autoremove -y \
  && rm -rf /var/lib/apt/lists/*

ENV CXX=clang++
ENV CC=clang

WORKDIR /advanced-compiler/downloads

RUN wget http://releases.llvm.org/9.0.0/llvm-9.0.0.src.tar.xz
RUN tar -xf llvm-9.0.0.src.tar.xz && rm -f llvm-9.0.0.src.tar.xz
RUN wget http://releases.llvm.org/9.0.0/cfe-9.0.0.src.tar.xz 
RUN tar -xf cfe-9.0.0.src.tar.xz && mv cfe-9.0.0.src llvm-9.0.0.src/tools/clang && rm -f cfe-9.0.0.src.tar.xz
RUN wget http://releases.llvm.org/9.0.0/compiler-rt-9.0.0.src.tar.xz 
RUN tar -xf compiler-rt-9.0.0.src.tar.xz && mv compiler-rt-9.0.0.src llvm-9.0.0.src/projects/compiler-rt && rm -f compiler-rt-9.0.0.src.tar.xz

WORKDIR /advanced-compiler

RUN mkdir llvm_build

RUN mv /usr/bin/clang-9 /usr/bin/clang && mv /usr/bin/clang++-9 /usr/bin/clang++ && mv /usr/bin/clang-cpp-9 /usr/bin/clang-cpp

WORKDIR /advanced-compiler/llvm_build
RUN cmake -G Ninja -DLLVM_TARGETS_TO_BUILD="X86" /advanced-compiler/downloads/llvm-9.0.0.src
RUN ninja

WORKDIR /advanced-compiler/hw1
COPY hw1 .
WORKDIR /advanced-compiler/demo
COPY demo .

WORKDIR /advanced-compiler
ENV PATH=/advanced-compiler/llvm_build/bin:$PATH

CMD ["bash"]



