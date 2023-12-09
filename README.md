# Reservoir in Processor Simulator

## how to build

The project requires cmake, ninja-build as dependencies. On Ubuntu/Debian, use

```sh
sudo apt-get install cmake ninja-build
```

or on mac, use

```sh
brew install cmake ninja-build
```

to install dependencies.

### before build

```sh
git submodule update --init
```

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

## Performance of each branch predictor on dhrystone

## without branch predictor (static false)

```sh
Total stages: 124791
```

## 1-bit branch predictor

```sh
Total stages: 123650
 BP accuracy: 0.703736 (Hit :11150, Miss :4694)
```

## 2-bit (Bimodal) branch predictor

```sh
Total stages: 122294
 BP accuracy: 0.76054 (Hit :12050, Miss :3794)
```

## Gshare branch predictor

```sh
Total stages: 118650
 BP accuracy: 0.895986 (Hit :14196, Miss :1648)
```
