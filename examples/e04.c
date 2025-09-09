#define MA_PARSE_IMPLEMENTATION
#include "../ma_parse.h"
#include <stdio.h>

int main() {
    ma_scanner_t sc = ma_scanner_create_default();

    int n;
    while (1) {
        n = ma_scanner_get_i32(&sc);
        ma_scanner_clear_input(&sc);
        if (ma_scanner_get_status(&sc) == MA_STATUS_OK) {
            break;
        }
        printf("ERROR: %s\n", ma_status_get_str(ma_scanner_get_status(&sc)));
        ma_scanner_clear_status(&sc);
    }

    printf("%d\n", n);
    return 0;
}