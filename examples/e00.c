#define MA_PARSE_IMPLEMENTATION
#include "../ma_parse.h"
#include <stdio.h>

int main() {
    ma_scanner_t sc = ma_scanner_create_default();

    printf("Please enter a number: ");
    int number = ma_scanner_get_i32(&sc);
    if (!ma_scanner_status_ok(&sc)) {
        printf("ERROR: %s\n", ma_status_get_str(ma_scanner_get_status(&sc)));
    }
    else {
        printf("Your number: %d\n", number);
    }

    return 0;
}