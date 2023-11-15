# Toolchains for riskheiv

## how to build

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


## run all unit tests

```sh
ninja -Cbuild unittests
```

# Performance of each branch predictor on dhrystone

## without branch predictor (static false)
```
Total stages: 124791
```


## 1-bit branch predictor
```
Total stages: 123650
 BP accuracy: 0.703736 (Hit :11150, Miss :4694)
```
## 2-bit (Bimodal) branch predictor
```
Total stages: 122294
 BP accuracy: 0.76054 (Hit :12050, Miss :3794)
```

## Gshare branch predictor
```
Total stages: 118650
 BP accuracy: 0.895986 (Hit :14196, Miss :1648)
```
