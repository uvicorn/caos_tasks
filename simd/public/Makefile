CC = gcc-15
CFLAGS = -Wall -Wextra -O3 -mavx2 -march=native 

all: bench_simd

bench_simd: main.c filter_simd.c
	$(CC) $(CFLAGS) $^ -o $@

test: bench_simd
	./bench_simd

clean:
	rm -f bench_simd
