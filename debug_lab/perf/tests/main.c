#include "tests.h"
#include "random.h"
#include "time.h"


int main() {

    xorshift32_setSeed(time(0));

    TEST_RMQ();

    return 0;
}