#include <stdio.h>
#define MA_PARSE_IMPLEMENTATION
#include "../ma_parse.h"

int main() {
    ma_scanner_t sc = ma_scanner_create(stdin, ",");
    printf("Please enter height, width, depth and density separated by commas.\n");
    printf("Example: 100,200,300,400\n");
    while (1) {
        int height, width, depth, density;
        printf(">> ");
        height  = ma_scanner_get_i32(&sc);
        width   = ma_scanner_get_i32(&sc);
        depth   = ma_scanner_get_i32(&sc);
        density = ma_scanner_get_i32(&sc);
        ma_scanner_clear_input(&sc);
        if (ma_scanner_get_status(&sc) == MA_STATUS_STREAM_EOF) {
            break;
        }
        if (ma_scanner_get_status(&sc) != MA_STATUS_OK) {
            printf("ERROR: %s\n", ma_status_get_str(sc.status));
            ma_scanner_clear_status(&sc);
            continue;
        }
        printf("Height: %d, width: %d, depth: %d, density: %d\n", height, width, depth, density);
    }
    printf("Bye!\n");
    return 0;
}