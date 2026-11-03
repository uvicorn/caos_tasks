## 1. SparseTable perf

Субо братик засрал мою версию SparseTable. Померьте с помощью perf кеши, **осознайте** проблему и зафиксите ее.


```
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -v
./bin/test_runner
```

## 2. AVX2 experience

Если все остально сделано, то в include/random.h ускорьте xorshift_array через SIMD.

Ман интела откуда брать инструкции: https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html

У кого нет впн: https://web.archive.org/web/20261025134223/https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html

