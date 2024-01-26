# blackjack

Blackjack in the CLI. Play against the computer and test your luck!

## Compilation instructions

Linux or MacOS (host system):

```sh
$ mkdir build && cd build
$ cmake -DCMAKE_BUILD_TYPE=Release ..
$ cmake --build .
```

Windows (host or cross-compile):

```sh
$ mkdir build-windows && cd build-windows
$ cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../windows.cmake ..
$ cmake --build .
```
