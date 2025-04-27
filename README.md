# libevent-example

- bufferevent server, client example study

## build
```sh
$ git submodule upddate --init
$ rm -rf build
$ cmake -B build && cmake --build build
```

## run
```sh
$ cd build
$ ./server
$ ./client
```
