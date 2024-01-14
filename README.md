# Hype & HDVR
> This repository is part of my individual project for the L46 module (Principles of Machine Learning Systems) in my MPhil in Advanced Computer Science at the University of Cambridge.

This repository contains **Hype**, a hyperdimensional computing framework built on the standard library, and **HDVR**, 
an application of Hype on the [ISOLET dataset](https://archive.ics.uci.edu/dataset/54/isolet) for speech recognition.

This implementation is based largely on work by Imani et al. (2017):
>Imani, M., Kong, D., Rahimi, A. & Simunic, T. (2017), ‘VoiceHD: Hyperdimensional Computing
for Efficient Speech Recognition’, 2017 IEEE International Conference on Rebooting Computing
(ICRC) pp. 1–8.

The paper can be found on [IEEE Xplore](https://ieeexplore.ieee.org/document/8123650).

## What is it?
The core library, Hype, contains general constructions which are commonly used in hyperdimensional computing. These 
include interfaces and implementations for hypervectors (see `hype::IVector` and `hype::Vector`) and memory (see 
`hype::Memory` and `hype::AssociativeMemory`).

To demonstrate the core library, it has been used in HDVR. This is a concrete application on the ISOLET dataset for
speech recognition.

More information on the project and its performance can be found in the report in this repository.

## How do I run it?
To compile the example application, run the following commands in a terminal in the root directory:
```
mkdir build
cd build
cmake ..
make
```

You can then run the example application by running:
```
./HDVR

Loading raw datasets... DONE
Encoding training data...  9%  19%  29%  39%  49%  59%  69%  79%  89%  99% DONE
Encoding testing data...  9%  19%  29%  39%  49%  59%  69%  79%  89%  99% DONE
Loaded 6238 training samples, and 1559 testing samples.
Running test... 
Accuracy before training: 88.5183%
[Epoch: 1]: error: 8.75281% – accuracy: 92.0462%
[Epoch: 2]: error: 5.96345% – accuracy: 93.2008%
[Epoch: 3]: error: 4.21609% – accuracy: 93.7139%
[Epoch: 4]: error: 2.99776% – accuracy: 93.7139%
[Epoch: 5]: error: 2.35652% – accuracy: 93.9705%
[Epoch: 6]: error: 1.69926% – accuracy: 94.0988%
[Epoch: 7]: error: 1.47483% – accuracy: 94.2912%
[Epoch: 8]: error: 1.042% – accuracy: 94.0346%
[Epoch: 9]: error: 1.00994% – accuracy: 94.0346%
[Epoch: 10]: error: 0.81757% – accuracy: 94.0988%
=== SUCCESS ===
```