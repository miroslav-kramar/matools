#define MA_PARSE_IMPLEMENTATION
#include "../ma_parse.h"
#include <stdio.h>

// The library expects there is only one scanner per stream.
// If you need to use a scanning functionality inside a function,
// do not create a new scanner inside of it.
// Pass a scanner pointer instead.

int sc_plus_one(ma_scanner_t * sc) {
    int out = ma_scanner_get_i32(sc) + 1;
    ma_scanner_clear_input(sc);
    return out;
}

int main() {
    ma_scanner_t sc = ma_scanner_create_default();
    int i = sc_plus_one(&sc);
    if (ma_scanner_get_status(&sc) != MA_STATUS_OK) {
        fprintf(stderr, "Error!\n");
    }
    else {
        printf("%d\n", i);
    }
    return 0;
}