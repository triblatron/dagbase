# dagbase

## Synopsis

Base utility library for [dag](https://github.com/triblatron/dag) and [dagui](https://github.com/triblatron/dagui)

Supports the following functionality:

* Lua interface
* Configuration interface
* OutputStream to serialise a Graph
* InputStream to read previously serialised objects

## Supported Platforms

* Windows 10,11
* Ubuntu Linux 22.04,24.04
* macOS >= 10.15 Intel
* macOS >= 15.1 Apple Silicon
* Raspberry Pi OS 12 on Pi4, Pi5

## Dependencies

* C++ 17 as found in Visual Studio 2017, 2019, 2022, gcc 11, clang 16
* cmake >= 3.15
* Lua >= 5.4
* googletest including googlemock
* googlebenchmark

## Building

### MacOS 15.1 Sequoia Apple Silicon

* Install brew
* Install Apple clang(using gcc as the name for some reason) and cmake
```bash
brew install gcc cmake
```
* Clone the repo
```bash
git clone https://github.com/triblatron/dagbase
```
* Configure and generate the build system
```bash
mkdir dagbase_build && cd dagbase_build

cmake -C ../dagbase/Automation/Build/InitialCacheLinuxGitHub.txt -B . -S ../dagbase
```
* Build
```bash
cmake --build . --target install -j <number_of_cores> --config Release
```
* Test
```bash
DagBaseTest
```
* Note that the repo used to be called nodebackend and the code.
