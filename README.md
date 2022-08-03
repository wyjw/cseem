# CSEEM

The Trace-Based Cache Simulator

Loosely based on: https://coffeebeforearch.github.io/2020/12/16/cache-simulator.html

How to Build:
```
mkdir build
cd build
cmake ..
make -j4
cd examples
python3 get_trace.py
```

Features:
```
Extensible parser
Different policies (LRU, FIFO, random)
Table printer
```

Example Usage
```
./main -f traces/art.trace -a 8 -b 32 -c 65536 -t simple -s
```
