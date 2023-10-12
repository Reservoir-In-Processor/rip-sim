# Toolchains for riskheiv

## how to build

### sanitizer build

```sh
cmake -GNinja -Bbuild -DSANITIZE=ON -DCMAKE_BUILD_TYPE=Debug
ninja -Cbuild
```

### release build

```sh
cmake -GNinja -Bbuild -DCMAKE_BUILD_TYPE=Release
ninja -Cbuild
```

## run unit tests

### All

```sh
ninja -Cbuild unittests
```

### Instruction Level Simulator

```sh
ninja -Cbuild sim-unittests
```

### Rip Simulator

```sh
ninja -Cbuild rip-unittests
```


## run all unit tests

```sh
ninja -Cbuild unittests
```
