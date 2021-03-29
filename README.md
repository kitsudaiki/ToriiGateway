# ToriiGateway

## Description

Proxy-service in context of the Kitsunemimi-Project.

## Build

I write my projects with the Qt-creator, but without Qt itself.

### Requirements

#### required tools to build

name | repository | version | task
--- | --- | --- | ---
clang++ | clang++ | >= 4.0 | Compiler for the C++ code. (compared to the other Kitsunemimi-repositories this project requires clang++ insteadk of g++ because of some special stuff in the boost beast library)
make | make | >= 4.0 | process the make-file, which is created by qmake to build the programm with g++
qmake | qt5-qmake | >= 5.0 | This package provides the tool qmake, which is similar to cmake and create$
FLEX | flex | >= 2.6 | Build the lexer-code for all used parser.
GNU Bison | bison | >= 3.0 | Build the parser-code together with the lexer-code.

Installation on Ubuntu/Debian:

```bash
sudo apt-get install clang++ make qt5-qmake bison flex
```

#### required official libraries

repository-name | version | task
--- | --- | ---
libboost-filesystem-dev | >= 1.60 | Use for file-interactions like for example listing files in a direc$
boost-beast library | (compiled form source) | 1.7x | provides http-server and websocket-server
ssl library | libssl-dev | 1.1.x | encryption for tls connections

Installation on Ubuntu/Debian:

```bash
sudo apt-get install libboost-filesystem-dev libssl-dev
```

This are the version I have installed under Debian Stable via apt. Some older or newer version should w$

IMPORTANT: All my projects are only tested on Linux.

#### Kitsunemimi-repositories

These repositories will be downloaded automatically by the build script of the tool itself (see next section). This list here is only as information to give an overview of all used Kitsunemimi libraries in this project.

Repository-Name | Version-Tag | Download-Path
--- | --- | ---
libKitsunemimiCommon | v0.18.0 |  https://github.com/kitsudaiki/libKitsunemimiCommon.git
libKitsunemimiPersistence | v0.10.1 | https://github.com/kitsudaiki/libKitsunemimiPersistence.git
libKitsunemimiJson | v0.10.5 | https://github.com/kitsudaiki/libKitsunemimiJson.git
libKitsunemimiJinja2 | v0.8.0 | https://github.com/kitsudaiki/libKitsunemimiJinja2.git
libKitsunemimiIni | v0.4.6 | https://github.com/kitsudaiki/libKitsunemimiIni.git
libKitsunemimiArgs | v0.2.1 | https://github.com/kitsudaiki/libKitsunemimiArgs.git
libKitsunemimiConfig | v0.2.3 | https://github.com/kitsudaiki/libKitsunemimiConfig.git
libKitsunemimiSakuraLang | v0.7.2 | https://github.com/kitsudaiki/libKitsunemimiSakuraLang.git
libKitsunemimiNetwork | v0.6.5 | https://github.com/kitsudaiki/libKitsunemimiNetwork.git
libKitsunemimiSakuraNetwork | v0.5.0 | https://github.com/kitsudaiki/libKitsunemimiSakuraNetwork.git
libKitsunemimiSakuraMessaging | v0.4.0 | -


### build programm

In all of my repositories you will find a `build.sh`. You only have to run this script. It doesn't require sudo, because you have to install the required tools manually beforehand, for example via apt. But if other projects of mine are required, it downloads them from GitHub and builds them in the correct version too. This script is also used by the CI pipeline, so it's tested with every commit.

Before running the build-script:

```bash
.
└── ToriiGateway
    ├── build.sh
    └── ...
```

After running the build-script:

```bash
.
├── build
│   ├── libKitsunemimiCommon
│   │   └── ...
│   ├── libKitsunemimiPersistence
│   │   └── ...
│   └── ...
│
├── libKitsunemimiCommon
│   └── ...
├── libKitsunemimiPersistence
│   └── ...
├── ...
│
├── ToriiGateway
│   ├── build.sh
│   └── ...
│
└── result
    └─── ToriiGateway
```

It automatically creates a `build` and `result` directory in the directory where you have cloned the project. At first it builds all into the build directory and after all build steps are finished, it copies the final binary into the result directory.

The build script links all Kitsunemimi libraries statically into the final binary.

Tested on Debian and Ubuntu. If you use CentOS, Arch, etc and the build script fails on your machine, then please write me a message or file a GitHub issue and I will try to fix the script.


## Contributing

Please give me as many inputs as possible: feature suggestions, bugs, bad code style, bad documentation, bad spelling and so on.

I know that my code is far away from being perfect. Its fast, but has only a little amount of functionality and, I'm sure, that it still has a huge amount of unhandled error-cases and bugs. This is mostly the result of working alone for this project. Beside this, I have unfortunately nearly no knowledge, how to work in an open-source project. So I would be grateful for any help. :)

Feel free to contact me under tobias.anker@kitsunemimi.moe

## License

This project is licensed under the Apache License Version 2.0 - see the [LICENSE](LICENSE) file for details

