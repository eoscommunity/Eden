# Eden

## Eden Community Web App

### Running Eden Community Web App locally

```sh
yarn
yarn dev --stream
open http://localhost:3000
```

For more details, refer to the [Web App README](./packages/webapp/README.md).

## Eden Contracts

Contracts can be built manually (see below) but the latest contracts are also available from our Github CI/CD in this repo. For contract deployment/update instructions and considerations, see the [Contracts README](./contracts/README.md).

### Build

Set the `WASI_SDK_PREFIX` environment variable before building (see architecture-specific instructions below). Alternatively, use cmake's `-DWASI_SDK_PREFIX=....` option. Also make sure `nodejs 14`, `npm 6.14`, and `yarn 1.22` are in your path.

```sh
git submodule update --init --recursive
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j
ctest -j
```

To speed up builds, use `-DCMAKE_CXX_COMPILER_LAUNCHER=ccache -DCMAKE_C_COMPILER_LAUNCHER=ccache`

#### Ubuntu 20.04

```sh
sudo apt-get update
sudo apt-get install -yq    \
    binaryen                \
    build-essential         \
    cmake                   \
    git                     \
    libcurl4-openssl-dev    \
    libgbm-dev              \
    libgmp-dev              \
    libnss3-dev             \
    libssl-dev              \
    libusb-1.0-0-dev        \
    libz-dev                \
    pkg-config              \
    python                  \
    wget

export WASI_SDK_PREFIX=~/work/wasi-sdk-12.0
export PATH=~/work/node-v14.16.0-linux-x64/bin:$PATH

cd ~/work
wget https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-12/wasi-sdk-12.0-linux.tar.gz
tar xf wasi-sdk-12.0-linux.tar.gz

wget https://nodejs.org/dist/v14.16.0/node-v14.16.0-linux-x64.tar.xz
tar xf node-v14.16.0-linux-x64.tar.xz
npm i -g yarn

# Install boost 1.75 or later. e.g. this installs it in /usr/local:
curl -LO https://github.com/eoscommunity/Eden/releases/download/deps/boost_1_75_0.tar.bz2
tar xf boost_1_75_0.tar.bz2
cd boost_1_75_0
./bootstrap.sh
./b2
sudo ./b2 --without-python install
```
