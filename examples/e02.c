#include <stdio.h>
#define MA_PARSE_IMPLEMENTATION
#include "../ma_parse.h"

int main() {
    ma_scanner_t sc = ma_scanner_create(stdin, ",");
    while (1) {
        int height, width, depth, density;
        height  = ma_scanner_get_i32(&sc);
        width   = ma_scanner_get_i32(&sc);
        depth   = ma_scanner_get_i32(&sc);
        density = ma_scanner_get_i32(&sc);
        ma_scanner_clear_input(&sc);
        if (ma_scanner_status_eof(&sc)) {
            break;
        }
        if (!ma_scanner_status_ok(&sc)) {
            printf("ERROR: %s\n", ma_status_get_str(sc.status));
            ma_scanner_clear_status(&sc);
        }
        printf("Height: %d, width: %d, depth: %d, density: %d\n", height, width, depth, density);
    }
    printf("Bye!\n");
    return 0;
}