# Immutable, persistent red-black tree

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

The `items/s` column below shows the number of keys inserted per second, per
thread.

```
Run on (40 X 3300 MHz CPU s)
CPU Caches:
  L1 Data 32K (x20)
  L1 Instruction 32K (x20)
  L2 Unified 256K (x20)
  L3 Unified 25600K (x2)
----------------------------------------------------------------------------
Benchmark                                     Time           CPU Iterations
----------------------------------------------------------------------------
BM_Insert/1/50000/threads:1            14585112 ns   14583913 ns         48   3.26961M items/s
BM_Insert/1/50000/threads:2            23006072 ns   46008715 ns         16   1061.28k items/s
BM_Insert/1/50000/threads:4            15072922 ns   60286563 ns         12   809.934k items/s
BM_Insert/1/50000/threads:8            15310274 ns  122472125 ns          8   398.688k items/s
BM_Insert/1/50000/threads:16           16799798 ns  268767090 ns         16   181.674k items/s
BM_Insert/1/50000/threads:20           13710540 ns  274171678 ns         20   178.093k items/s
BM_Insert/10/50000/threads:1           43361104 ns   43357572 ns         16   1126.17k items/s
BM_Insert/10/50000/threads:2           61379752 ns  122750333 ns          6   397.784k items/s
BM_Insert/10/50000/threads:4           34766197 ns  139054203 ns          4   351.145k items/s
BM_Insert/10/50000/threads:8           24842416 ns  198724029 ns          8   245.708k items/s
BM_Insert/10/50000/threads:16          28086838 ns  449349336 ns         16   108.664k items/s
BM_Insert/10/50000/threads:20          22692047 ns  453792110 ns         20     107.6k items/s
BM_Insert/100/50000/threads:1          70612655 ns   70607468 ns         10   691.543k items/s
BM_Insert/100/50000/threads:2          95424727 ns  190834486 ns          4   255.866k items/s
BM_Insert/100/50000/threads:4          51846444 ns  207369635 ns          4   235.464k items/s
BM_Insert/100/50000/threads:8          40646863 ns  325141208 ns          8   150.175k items/s
BM_Insert/100/50000/threads:16         26222879 ns  419515123 ns         16   116.392k items/s
BM_Insert/100/50000/threads:20         26370949 ns  527365311 ns         20   92.5888k items/s
BM_Insert/1000/50000/threads:1        103091920 ns  103068004 ns          7   473.747k items/s
BM_Insert/1000/50000/threads:2        125407984 ns  250795451 ns          2   194.693k items/s
BM_Insert/1000/50000/threads:4         68696828 ns  274758007 ns          4   177.713k items/s
BM_Insert/1000/50000/threads:8         39250971 ns  313970169 ns          8   155.518k items/s
BM_Insert/1000/50000/threads:16        34350680 ns  549554322 ns         16   88.8504k items/s
BM_Insert/1000/50000/threads:20        33472593 ns  669382132 ns         20   72.9451k items/s
BM_Insert/10000/50000/threads:1       136638279 ns  136627041 ns          5   357.383k items/s
BM_Insert/10000/50000/threads:2       146108420 ns  292193495 ns          2   167.109k items/s
BM_Insert/10000/50000/threads:4        80790644 ns  323137977 ns          4   151.106k items/s
BM_Insert/10000/50000/threads:8        46486877 ns  371865944 ns          8   131.306k items/s
BM_Insert/10000/50000/threads:16       30153325 ns  482414772 ns         16   101.216k items/s
BM_Insert/10000/50000/threads:20       26817771 ns  536302848 ns         20   91.0458k items/s
BM_Insert/100000/50000/threads:1      177791213 ns  177777103 ns          4   274.659k items/s
BM_Insert/100000/50000/threads:2      187525013 ns  375018379 ns          2   130.202k items/s
BM_Insert/100000/50000/threads:4      101209401 ns  404806068 ns          4   120.621k items/s
BM_Insert/100000/50000/threads:8       57154975 ns  457184770 ns          8   106.802k items/s
BM_Insert/100000/50000/threads:16      36265690 ns  580188563 ns         16   84.1591k items/s
BM_Insert/100000/50000/threads:20      31374847 ns  627055902 ns         20   77.8689k items/s
BM_Insert/1000000/50000/threads:1     284157682 ns  284132513 ns          2    171.85k items/s
BM_Insert/1000000/50000/threads:2     246166604 ns  492293154 ns          2   99.1851k items/s
BM_Insert/1000000/50000/threads:4     136403581 ns  545572170 ns          4   89.4989k items/s
BM_Insert/1000000/50000/threads:8      76189559 ns  609448143 ns          8   80.1186k items/s
BM_Insert/1000000/50000/threads:16     45518500 ns  728188312 ns         16   67.0543k items/s
BM_Insert/1000000/50000/threads:20     38972236 ns  779353832 ns         20   62.6521k items/s
BM_Insert/10000000/50000/threads:1    388982227 ns  388877223 ns          2   125.562k items/s
BM_Insert/10000000/50000/threads:2    309269358 ns  618416331 ns          2   78.9567k items/s
BM_Insert/10000000/50000/threads:4    171764105 ns  686960578 ns          4   71.0785k items/s
BM_Insert/10000000/50000/threads:8     94066016 ns  752449714 ns          8   64.8922k items/s
BM_Insert/10000000/50000/threads:16    51168286 ns  814673587 ns         16   59.9358k items/s
BM_Insert/10000000/50000/threads:20    46079329 ns  918171791 ns         20   53.1797k items/s
BM_Insert/100000000/50000/threads:1   586921614 ns  586878163 ns          1   83.1998k items/s
BM_Insert/100000000/50000/threads:2   418418581 ns  836669039 ns          2   58.3601k items/s
BM_Insert/100000000/50000/threads:4   234051581 ns  936066969 ns          4   52.1631k items/s
BM_Insert/100000000/50000/threads:8   125203020 ns 1001516619 ns          8   48.7542k items/s
BM_Insert/100000000/50000/threads:16   64886922 ns 1037121993 ns         16   47.0804k items/s
BM_Insert/100000000/50000/threads:20   53904892 ns 1077886906 ns         20   45.2999k items/s
```
