# immutable, persistent red-black tree

```c++
Tree<int, int> t;            // t is empty
auto t0 = t.insert(0, 0);    // t0 contains {0,0}
auto t1 = t0.insert(1, 1);   // t1 contains {0,0}, {1,1}
```

This is a C++ port of the excellent Rust-based implementation
from https://github.com/orium/rpds.

# Performance (20 March 2018)

All benchmarks run on the following hardware:

```
CPU Two Intel E5-2660 v3 10-core CPUs at 2.60 GHz (Haswell EP)
RAM 160GB ECC Memory (10x 16 GB DDR4 2133 MHz dual rank RDIMMs)
```

The insert workload measures the insertion performance of 1..20 threads
inserting 50,000 random keys to an existing tree containing 1..100M keys. The
keys are uniformly distributed 64-bit integers. Every insertion forms a new
tree that is discarded by the thread, so the base tree never changes.
