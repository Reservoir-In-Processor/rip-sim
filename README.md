# Reservoir in Processor Simulator (RiP-Sim) - Work in Progress

[![Build and Test](https://github.com/Reservoir-In-Processor/rip-sim/actions/workflows/main.yaml/badge.svg)](https://github.com/Reservoir-In-Processor/rip-sim/actions/workflows/main.yaml) [![Dhrystone 32-bit](https://github.com/Reservoir-In-Processor/rip-sim/actions/workflows/dhrystone.yaml/badge.svg)](https://github.com/Reservoir-In-Processor/rip-sim/actions/workflows/dhrystone.yaml) [![Python wrapper](https://github.com/Reservoir-In-Processor/rip-sim/actions/workflows/python-wrapper.yaml/badge.svg)](https://github.com/Reservoir-In-Processor/rip-sim/actions/workflows/python-wrapper.yaml) [![riscv-tests rv32im](https://github.com/Reservoir-In-Processor/rip-sim/actions/workflows/riscv-tests.yaml/badge.svg)](https://github.com/Reservoir-In-Processor/rip-sim/actions/workflows/riscv-tests.yaml) [![License](https://img.shields.io/badge/License-Apache_2.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)

## Overview

RiP-Sim is an innovative 32-bit RISC-V (RV32IM) Simulator, currently under active development. This project stands out by integrating Reservoir Computing into branch prediction, a novel approach in the realm of processor simulation. As a Work in Progress, RiP-Sim invites contributions and feedback from the community, particularly from those interested in processor architecture or reservoir computing.

### Python Wrapper

A notable feature of RiP-Sim is its Python wrapper, which allows users to test and develop their own branch predictor models. This interface provides good accessibility and customization. Users can implement and evaluate complex branch prediction algorithms, like reservoir computing models, directly within the simulation environment.

```python

rbp = ESN_RLS(reservoir_dim=100, input_dim=input_dim, output_dim=1)
while True:
   trap = rsim.proceed()
   if trap == RIPSimulator.Trap.EBREAK:
       break
   elif trap == RIPSimulator.Trap.BRANCH_PRED:
       rbp.train(rsim.previous_branch_result)
       inputs = create_inputs(rsim)
       rsim.predict(rbp.predict(inputs))
   else:
       assert False, "unreachable!"

```

This aspect of RiP-Sim allows for the exploration of complex algorithms within the simulator, offering an extra layer of experimentation and customization for enthusiasts and researchers.

## Requirements

The project requires cmake, ninja-build as dependencies. On Ubuntu/Debian, use

```sh
sudo apt-get install cmake ninja-build
```

or on mac, use

```sh
brew install cmake ninja-build
```

to install dependencies.

## Build

```sh
git submodule update --init
cmake -GNinja -Bbuild -DCMAKE_BUILD_TYPE=Release
ninja -Cbuild
```

or you can use debug build with/without sanitizers to see a tons of simulation dumps.

```sh
cmake -GNinja -Bbuild -DSANITIZE=ON -DCMAKE_BUILD_TYPE=Debug
ninja -Cbuild
```

### Unit Tests

#### All

```sh
ninja -Cbuild unittests
```

#### Instruction Level Simulator

```sh
ninja -Cbuild sim-unittests
```

#### RiP Simulator

```sh
ninja -Cbuild rip-unittests
```

#### Python wrapper for RiP simulator

```sh
ninja -Cbuild pytest
```

## Performance of each branch predictor on Dhrystone

### No branch predictor (predicted UnTaken for all branches)

```sh
$ ./build/bin/rip-sim rip-tests/dhry.bin --dram-size=268435456 --stats 
break happens
========== BEGIN STATS ============
Total stages: 122543
...
```

### 1-bit branch predictor

```sh
$ ./build/bin/rip-sim rip-tests/dhry.bin --dram-size=268435456 --stats -b=onebit
break happens
========== BEGIN STATS ============
Total stages: 121692
...
 BP accuracy: 0.692761 (Hit :10938, Miss :4851)
=========== END STATS =============
```

### 2-bit (Bimodal) branch predictor

```sh
$ ./build/bin/rip-sim rip-tests/dhry.bin --dram-size=268435456 --stats -b=twobit
break happens
========== BEGIN STATS ============
Total stages: 119889
...
 BP accuracy: 0.754893 (Hit :11919, Miss :3870)
=========== END STATS =============

```

### Gshare branch predictor

```sh
$ ./build/bin/rip-sim rip-tests/dhry.bin --dram-size=268435456 --stats -b=gshare
break happens
========== BEGIN STATS ============
Total stages: 116617
...
 BP accuracy: 0.88986 (Hit :14050, Miss :1739)
=========== END STATS =============
```

### Perceptron branch predictor

```sh
$ ./build/bin/rip-sim rip-tests/dhry.bin --dram-size=268435456 --stats -b=perceptron
break happens
========== BEGIN STATS ============
Num Stages=115801
...
 BP accuracy: 0.941225 (Hit :14861, Miss :928)
=========== END STATS =============
```
