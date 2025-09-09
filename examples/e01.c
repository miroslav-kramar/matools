#define MA_PARSE_IMPLEMENTATION
#include "../ma_parse.h"
#include <stdio.h>

int main() {
    ma_scanner_t sc = ma_scanner_create_default();

    while (1) {
        printf(">> ");
        const int n = ma_scanner_get_i32(&sc);
        ma_scanner_clear_input(&sc);

        if (ma_scanner_get_status(&sc) == MA_STATUS_STREAM_EOF) {
            break;
        }
        if (ma_scanner_get_status(&sc) != MA_STATUS_OK) {
            printf("ERROR: %s\n", ma_status_get_str(ma_scanner_get_status(&sc)));
            ma_scanner_clear_status(&sc);
            continue;
        }
        printf("%d\n", n);
    }
    printf("\nBye!\n");

    return 0;
}