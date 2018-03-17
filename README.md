# immutable, persistent red-black tree

```c++
Tree<int, int> t;            // t is empty
auto t0 = t.insert(0, 0);    // t0 contains {0,0}
auto t1 = t0.insert(1, 1);   // t1 contains {0,0}, {1,1}
```

This is a port of the excellent Rust-based implementation
from https://github.com/orium/rpds.
