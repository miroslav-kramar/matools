# Ma Tools

Single-header scanning and string parsing library for all basic types.

A decent documentation will be provided when everything settles down enough. For now, please refer to the source code :-)

> [!WARNING]
> Current implementation requires testing. For now, use at your own risk!

## How to work with the library?

Pick a file which will contain the implementation of all Ma Tools functions. Then define the `MATOOLS_IMPLEMENTATION` flag before including the `matools.h` library:

```c
#define MATOOLS_IMPLEMENTATION
#include "matools.h"
...
```

Then, in all other files using Ma Tools functions:

```c
#include "matools.h"
...
```

## Example usage

Other examples can be found in `examples/`.

```c
#define MATOOLS_IMPLEMENTATION
#include "matools.h"
#include <stdio.h>

int main() {
    // Create a scanner struct to hold all necessary context information.
    // Pass a pointer to a stream you want to scan. Second argument is delimiter,
    // pass NULL if you want to split on whitespace.
    ma_scanner_t sc = ma_scanner_create(stdin, NULL);

    int n;
    while (1) {
        // Pick a scanning function corresponding to the data type.
        n = ma_scanner_get_i32(&sc);
        // Consume all remaining characters until new line or EOF.
        ma_scanner_clear_input(&sc);
        // Check for errors.
        if (ma_scanner_status_ok(&sc)) {
            break;
        }
        // Get textual representation of a status code
        printf("ERROR: %s\n", ma_status_str(ma_scanner_status(&sc)));
        // Clear status before further use
        ma_scanner_clear_status(&sc);
    }

    printf("%d\n", n);
    return 0;
}
```