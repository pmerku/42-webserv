# 42-webserv
Webserver made from scratch in c++

## Why use this?
Nginx is too good, we solve this major flaw by making a worse version

### Installation

Make sure you have `clang` installed, if not change to your preferred c++ compiler in the `makefile`.

If you have `make` installed you can just clone the repo and run `make exec -j`. The program
will provide all the instructions once its run.

Otherwise, you can use the provided `cmake` configuration in your favorite IDE or use `cmake`
compilation directly in the terminal.

If you use `cmake` from the terminal create a directory inside the repo to not clutter it with generated files
```
mkdir cmake
cd cmake
cmake .. && make
```

Both ways will put the binary inside the `build` folder at the root of the repo.

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.