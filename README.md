# boinc-rpc-cpp

C++ RPC API for BOINC client.

## Dependencies

- [CMake](https://cmake.org) (3.5+)
- [Boost](http://boost.org) (1.58+)
- [libxml++](https://developer.gnome.org/libxml++/stable/) (2.40+)

## Installation

```
$ mkdir build
$ cd build
$ cmake -DCMAKE_INSTALL_PREFIX=/usr ..
$ make
# make DESTDIR=/usr install
```

## Usage example

```
#include <boinc-rpc-cpp/boinc-rpc-cpp.hpp>

int main() {
    Boinc::Client c {.addr = "127.0.0.1", .port = 31416, .password = "my-pass-in-gui_rpc_auth.cfg"};
    for (auto msg : c.get_messages()) {
        std::cout << msg.body.value_or("") << std::endl;
    };

    return 0;
}
```
