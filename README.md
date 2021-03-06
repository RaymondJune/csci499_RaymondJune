<p align="center">
  <a href="" rel="noopener">
 <img width=200px height=200px src="https://i.imgur.com/6wj0hh6.jpg" alt="Project logo"></a>
</p>

<h3 align="center">CSCI499_RaymondJune</h3>

<div align="center">

[![Status](https://img.shields.io/badge/status-active-success.svg)]()
[![GitHub Issues](https://img.shields.io/github/issues/RaymondJune/Emotion_Pretend_Game.svg)](https://github.com/RaymondJune/csci499_RaymondJune/issues)
[![GitHub Pull Requests](https://img.shields.io/github/issues-pr/RaymondJune/Emotion_Pretend_Game.svg)](https://github.com/RaymondJune/csci499_RaymondJune/pulls)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](/LICENSE)

</div>

---

<p align="center">  First two milestone of USC CS499 2020Spring Project. <a href = "https://docs.google.com/document/d/1TCxCbmyiO5mpiuvH8q9FZMXzt7nT2Drwecho8i3bW6g/edit#">Project Description</a>.
    <br> 
</p>

## 📝 Table of Contents

- [Virtual Box VM Setup](#Setup)
- [Build](#built_using)
- [Test](#tests)
- [Usage](#usage)
- [Phase 2](#phase)
- [Authors](#authors)
- [Acknowledgments](#acknowledgement)

## 🏁 Set up VM Environment <a name = "Setup"></a>

### Prerequisites

(1) Ubuntu 18.04 LTS <br />
(2) As followings: 
```
sudo apt-get install build-essential autoconf libtool pkg-config 
sudo apt install curl 
install bazel
  download "bazel-2.2.0-installer-linux-x86_64.sh"
  chmod +x bazel-2.2.0-installer-linux-x86_64.sh
  ./bazel-2.2.0-installer-linux-x86_64.sh --user
  export PATH="$PATH:$HOME/bin"
install cmake
  download "cmake-3.16.5-Linux-x86_64.sh"
  chmod +x cmake-3.16.5-Linux-x86_64.sh
  sudo ./cmake-3.16.5-Linux-x86_64.sh --prefix=/usr/local/ --exclude-subdir
  export PATH="$PATH:/usr/local/bin"
install python and pip
  apt install python
  apt install python-pip
install g++ and build-essential
  sudo apt install g++
  sudo apt install build-essential
```

### Installing

A step by step series of examples that tell you how to get a development env running.

#### GRPC
```
git clone -b $(curl -L https://grpc.io/release) https://github.com/grpc/grpc 
cd grpc 
git submodule update --init 
bazel build :all 
make 
make install 
```
#### Protobuf 
```
cd ./third_party/protobuf 
sudo make install
```
#### Packages
(1) googletest
```
cd third_party/googletest 
cmake . 
make 
sudo make install 
```
(2) gflags
```
cd ../gflags 
cmake . 
make 
sudo make install 
```

(3) glog
- change working directory to a directory which is in the same level as grpc directory
```
git clone https://github.com/google/glog.git
cd glog
cmake .
make
sudo make install
```
## ⛏️ Build <a name = "built_using"></a>
- Use cmake to build the project
```
mkdir build && cd build 
cmake .. 
make 
```

## 🔧 Running the tests <a name = "tests"></a>

```
./store_test 
```

## 🎈 Usage <a name="usage"></a>

- After building the project, launch servers in the following order:
```
./kvstore_server [--store <file>]
./func
```
- Then hook events, by default, hook all five events
```
./bootstrap --default
```
- Then open multiple terminals resembling multiple warble users to test user commands
```
./warble --registeruser Bob
./warble --registeruser Alice
./warble --user alice --warble 'hello, Bob!'
```
- If you want to reply a warble, please keep a note of that warble's warble id
```
./warble --user Bob --warble 'hello, alice!' --reply warble_id_1583455264974665
./warble --user Bob --read warble_id_1583455264974665
./warble --user alice --follow Bob
./warble --user Bob --profile
```

## 🎈 Phase 2 <a name="phase"></a>
- Ephemeral warble code: My code in phase 1 already adheres to this and my partner has verified it in code review.
- Key-value store disk persistence: Update code and unit tests

## ✍️ Authors <a name = "authors"></a>

- [@Ruimin Zeng](https://github.com/RaymondJune) - Idea & Initial work


## 🎉 Acknowledgements <a name = "acknowledgement"></a>

- [@tianhanl](https://github.com/tianhanl)
- [@barath](https://github.com/barath)
- [@apetranik](https://github.com/apetranik)
- [@priyankaranke](https://github.com/priyankaranke)
