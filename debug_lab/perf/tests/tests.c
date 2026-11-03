#include "tests.h"
#include "sparse_table.h"
#include "random.h"
#include "stdio.h"
#include "time.h"
#include "random.h"
#include <stdlib.h>


const unsigned TEST_SIZE = 1 << 20;
const unsigned TEST_ITERATIONS = 5;


#define ASSERT(x, msg)          \
    if (!(x)){                  \
        fprintf(stderr, msg);   \
        exit(1);                \
    }

void TEST_RMQ(){
    printf("START RMQ TEST\n");

    clock_t sum_fixed = 0;
    clock_t sum_default = 0;

    Element* test_data = aligned_alloc(32, sizeof(Element) * TEST_SIZE);
    Element* test_segments = aligned_alloc(32, sizeof(Element) * TEST_SIZE * 2);

    for (unsigned test_iter = 0; test_iter < TEST_ITERATIONS; test_iter++){
        xorshift_array(test_data, TEST_SIZE, xorshift32());
        xorshift_array_bounded(test_segments, TEST_SIZE * 2, xorshift32(), TEST_SIZE);

        SparseTable* default_ST    = SparseTable_ctor(test_data, TEST_SIZE);
        SparseTable* fixed_ST      = SparseTable_FIX_ctor(test_data, TEST_SIZE);
        for (unsigned query = 0; query < TEST_SIZE * 2; query += 2){
            unsigned l = test_segments[query];
            unsigned r = test_segments[query + 1];
            if (l == r)
                r++;
            else if (l > r)
                swap(l, r);

            clock_t start = clock();
            Element res_default = SparseTable_get_min_on_segment(default_ST, l, r);
            sum_default += clock() - start;

            start = clock();
            Element res_fixed = SparseTable_FIX_get_min_on_segment(fixed_ST, l, r);
            sum_fixed += clock() - start;

            ASSERT(res_fixed == res_default, "TEST SPARSE TABLE FAILED");
        }

        SparseTable_dtor(default_ST);
        SparseTable_dtor(fixed_ST);
    }
    sum_fixed /= TEST_ITERATIONS;
    sum_default    /= TEST_ITERATIONS;

    printf(
        "\n"
        "| Fixed     | Default |\n"
        "| --------- | ------- |\n"
        "| %ld       | %ld     |\n"
        "\n",
        sum_fixed, sum_default 
    );

    free(test_data);
    free(test_segments);
}