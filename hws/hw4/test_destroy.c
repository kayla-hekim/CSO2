#include <assert.h>
#include "mlpt.h"

int main(void) {
    assert(ptbr == 0);
    assert(allocate_page(0x0) == 1);
    assert(ptbr != 0);
    assert(mlpt_destroy() == 0);
    assert(ptbr == 0);
    assert(mlpt_destroy() == -1);
    return 0;
}