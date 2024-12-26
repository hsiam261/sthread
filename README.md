# sthread
simple threads - simple thread library for linux. The library current only has support for amd64 machines.

# build steps
```
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -B build/
cd build/
make
```

# running examples
First build the library and then build the examples with the following command:
```
cd example
make
```

After that simply run the executable you want to test. For example to run the `hello-world.c` example simply just use `./hello-world.out` command on your shell.

# Setting Up DevShell
If nix is installed and flakes are enabled, you can jump directly into a dev shell using `nix develop` command.
