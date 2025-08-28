/*
==================================================================
Ma Tools - Scanning and string parsing library for all basic types
==================================================================

Library offers string parsing functions and stream scanning functions for
every basic data type.

Usage:
    In one of your files, do:
    #define MATOOLS_IMPLEMENTATION
    #include "matools.h"
    ...

    Then just include in other files:
    #include "matools.h"
    ...
*/

#ifndef MATOOLS_H
#define MATOOLS_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

typedef enum {
    MA_OK = 0,        // Everything okay
    MA_STREAM_EOF,    // EOF encountered on a stream
    MA_STREAM_FERROR, // Error occurred on a stream
    MA_INVALID_INPUT, // General invalid input error
    MA_OUT_OF_RANGE   // General out of range error
} ma_status_t;

typedef struct {
    FILE * istream;
	const char * delim;
	ma_status_t status;
	bool newline_found;
} ma_scanner_t;

// ---------------------
// FUNCTION DECLARATIONS
// ---------------------

const char * ma_status_get_str(ma_status_t status);

int64_t      ma_parse_i64(const char * string, ma_status_t * status);
int32_t      ma_parse_i32(const char * string, ma_status_t * status);
int16_t      ma_parse_i16(const char * string, ma_status_t * status);
int8_t       ma_parse_i8(const char * string, ma_status_t * status);
uint64_t     ma_parse_u64(const char * string, ma_status_t * status);
uint32_t     ma_parse_u32(const char * string, ma_status_t * status);
uint16_t     ma_parse_u16(const char * string, ma_status_t * status);
uint8_t      ma_parse_u8(const char * string, ma_status_t * status);
long double  ma_parse_ldbl(const char * string, ma_status_t * status);
double       ma_parse_dbl(const char * string, ma_status_t * status);
float        ma_parse_flt(const char * string, ma_status_t * status);

ma_scanner_t ma_scanner_create(FILE * istream, const char * delim);
ma_scanner_t ma_scanner_create_default();

void         ma_scanner_clear_input(ma_scanner_t * scanner);
void         ma_scanner_clear_status(ma_scanner_t * scanner);

size_t       ma_scanner_get_str(ma_scanner_t * scanner, char * buffer, size_t buffer_length); // returns length of the token
int64_t      ma_scanner_get_i64(ma_scanner_t * scanner);
int32_t      ma_scanner_get_i32(ma_scanner_t * scanner);
int16_t      ma_scanner_get_i16(ma_scanner_t * scanner);
int8_t       ma_scanner_get_i8(ma_scanner_t * scanner);
uint64_t     ma_scanner_get_u64(ma_scanner_t * scanner);
uint32_t     ma_scanner_get_u32(ma_scanner_t * scanner);
uint16_t     ma_scanner_get_u16(ma_scanner_t * scanner);
uint8_t      ma_scanner_get_u8(ma_scanner_t * scanner);
long double  ma_scanner_get_ldbl(ma_scanner_t * scanner);
double       ma_scanner_get_dbl(ma_scanner_t * scanner);
float        ma_scanner_get_flt(ma_scanner_t * scanner);

void         ma_scanner_set_delim(ma_scanner_t * scanner, const char * delim);
void         ma_scanner_set_delim_default(ma_scanner_t * scanner);

ma_status_t  ma_scanner_status(ma_scanner_t * scanner);
bool         ma_scanner_status_ok(ma_scanner_t * scanner);
bool         ma_scanner_status_eof(ma_scanner_t * scanner);
bool         ma_scanner_status_ferror(ma_scanner_t * scanner);
bool         ma_scanner_status_invalid(ma_scanner_t * scanner);
bool         ma_scanner_status_range(ma_scanner_t * scanner);

// --------------
// IMPLEMENTATION
// --------------

#ifdef MATOOLS_IMPLEMENTATION

// includes

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <float.h>
#include <errno.h>

// defines

#define TYPES_LIST\
    X(i64,   signed,   int64_t,      INT64_MIN, INT64_MAX)\
    X(i32,   signed,   int32_t,      INT32_MIN, INT32_MAX)\
    X(i16,   signed,   int16_t,      INT16_MIN, INT16_MAX)\
    X(i8,    signed,   int8_t,       INT8_MIN,  INT8_MAX)\
    X(u64,   unsigned, uint64_t,     0,         UINT64_MAX)\
    X(u32,   unsigned, uint32_t,     0,         UINT32_MAX)\
    X(u16,   unsigned, uint16_t,     0,         UINT16_MAX)\
    X(u8,    unsigned, uint8_t,      0,         UINT8_MAX)\
    X(ldbl,  floating, long double, -LDBL_MAX,  LDBL_MAX)\
    X(dbl,   floating, double,      -DBL_MAX,   DBL_MAX)\
    X(flt,   floating, float,       -FLT_MAX,   FLT_MAX)

#define STATUS_CODES\
    X(MA_OK,             ok,      "OK")\
    X(MA_STREAM_EOF,     eof,     "End of stream")\
    X(MA_STREAM_FERROR,  ferror,  "Stream error")\
    X(MA_INVALID_INPUT,  invalid, "Invalid input")\
    X(MA_OUT_OF_RANGE,   range,   "Out of range")

#define signed_t   int64_t
#define unsigned_t uint64_t
#define floating_t long double

// macro-functions

#define SET_STATUS(status, code)\
	do {\
		if (status && *status == MA_OK) {*status = code;}\
	} while (0)

// -----------------
// UTILITY FUNCTIONS
// -----------------

const char * ma_status_get_str(ma_status_t status) {
    switch (status) {
        #define X(code, name, msg) case code: return msg;
        STATUS_CODES
        #undef X
        default: return "Unknown status code";
    }
}

// ---------------------
// PARSING FUNCTIONALITY
// ---------------------

#define PARSE_signed(str, endptr)   strtoll(str, endptr, 10)
#define PARSE_unsigned(str, endptr) strtoull(str, endptr, 10)
#define PARSE_floating(str, endptr) strtold(str, endptr)
#define PARSE(variant, str, endptr) PARSE_##variant(str, endptr)

#define CHECK_BAD_RANGE_signed(value, min, max)   ((value) < (min) || (value) > (max))
#define CHECK_BAD_RANGE_unsigned(value, min, max) ((value) > (max))
#define CHECK_BAD_RANGE_floating(value, min, max) CHECK_BAD_RANGE_signed(value, min, max)
#define CHECK_BAD_RANGE(variant, value, min, max) CHECK_BAD_RANGE_##variant(value, min, max)

#define X(name, variant, type, min, max)\
    type ma_parse_##name (const char * str, ma_status_t * status) {\
        const char * tkn_start = NULL;\
        const char * tkn_end   = NULL;\
        \
        for (size_t i = 0; *(str+i) != '\0'; i++) {\
            if (!isspace((unsigned char)str[i])) {\
                if (!tkn_start) {tkn_start = str + i;}\
                tkn_end = str + i;\
            }\
        }\
        if (tkn_start == NULL) {\
            goto INVALID_TOKEN;\
        }\
        \
        char * parse_end;\
        errno = 0;\
        const variant##_t value = PARSE(variant, tkn_start, &parse_end);\
        if (parse_end <= tkn_end) {\
            goto INVALID_TOKEN;\
        }\
        if (errno == ERANGE || CHECK_BAD_RANGE(variant, value, min, max)) {\
            goto OUT_OF_RANGE;\
        }\
        \
        return (type) value;\
        \
        INVALID_TOKEN:\
        SET_STATUS(status, MA_INVALID_INPUT);\
        return 0;\
        \
        OUT_OF_RANGE:\
        SET_STATUS(status, MA_OUT_OF_RANGE);\
        return 0;\
    }
TYPES_LIST
#undef X

// ----------------------
// SCANNING FUNCTIONALITY
// ----------------------

#define DEFAULT_DELIM " \t\v\f\r"

ma_scanner_t ma_scanner_create(FILE * istream, const char * delim) {
    ma_scanner_t out;
    out.istream = istream;
    out.status = MA_OK;
    out.newline_found = false;
    if (delim == NULL) {out.delim = DEFAULT_DELIM;}
    else {out.delim = delim;}
    return out;
}

ma_scanner_t ma_scanner_create_default() {
    return ma_scanner_create(stdin, NULL);
}

ma_status_t ma_scanner_status(ma_scanner_t * scanner) {
    return scanner->status;
}

// Generate status code getters
#define X(code, name, msg)\
    bool ma_scanner_status_##name(ma_scanner_t * scanner) {\
        return scanner->status == code;\
    }
STATUS_CODES
#undef X

void ma_scanner_clear_status(ma_scanner_t * scanner) {
    scanner->status = MA_OK;
}

void ma_scanner_set_delim(ma_scanner_t * scanner, const char * delim) {
    scanner->delim = delim;
}

void ma_scanner_set_delim_default(ma_scanner_t * scanner) {
    scanner->delim = DEFAULT_DELIM;
}

void ma_scanner_clear_input(ma_scanner_t * scanner) {
    if (scanner->newline_found) {return;}
    int c;
    do {
        c = fgetc(scanner->istream);
    } while (c != '\n' && c != EOF);
    scanner->newline_found = true;
}

size_t ma_scanner_get_str(ma_scanner_t * scanner, char * buffer, const size_t buffer_length) {
    if (scanner->status != MA_OK) {return 0;}
    if (buffer_length == 0) {
        scanner->status = MA_OUT_OF_RANGE;
        return 0;
    }

    if (scanner->newline_found) {scanner->newline_found = false;}

    size_t token_length = 0;
    while (true) {
        const int c = fgetc(scanner->istream);
        if (c == EOF) {
            if (ferror(scanner->istream)) {goto ERR_FERROR;}
            if (token_length == 0)   {goto ERR_EOF_NO_CHAR;}
            break;
        }
        if (c == '\n') {
            scanner->newline_found = true;
            break;
        }
        // Check for delimiter
        if (strchr(scanner->delim, c) != NULL) {
            if (token_length == 0) {continue;} // Skip leading delimiters
            break;                             // Break when end of token found
        }

        // Write char into the string
        // If the token is too long, then set the status code, but keep consuming
        if (token_length < buffer_length - 1 || (token_length == 0 && buffer_length == 1)) {
        	buffer[token_length++] = (char) c;
        }
        else {
            scanner->status = MA_OUT_OF_RANGE;
        }
    }

    // Terminate string if not single char
    if (buffer_length != 1) {buffer[token_length] = '\0';}
    return token_length;

    ERR_EOF_NO_CHAR:
    scanner->status = MA_STREAM_EOF;
    return 0;

    ERR_FERROR:
    scanner->status = MA_STREAM_FERROR;
    return 0;
}

// Generate scanning functions definitions for each type
#define X(name, variant, type, min, max)\
    type ma_scanner_get_##name (ma_scanner_t * sc) {\
        if (sc->status != MA_OK) {return (type)0;}\
        \
        char buffer[512] = {0}; /*Did you know DBL_MAX has more than 300 digits?*/\
        const size_t tkn_len = ma_scanner_get_str(sc, buffer, sizeof(buffer));\
        if (sc->status != MA_OK) {\
            goto TOKENIZATION_ERROR;\
        }\
        if (tkn_len == 0) {\
            goto INVALID_TOKEN;\
        }\
        \
        const type value = ma_parse_##name(buffer, &sc->status);\
        if (sc->status != MA_OK) {\
            goto PARSING_ERROR;\
        }\
        \
        return value;\
        \
        INVALID_TOKEN:\
        sc->status = MA_INVALID_INPUT;\
        PARSING_ERROR:\
        TOKENIZATION_ERROR:\
        return 0;\
    }
TYPES_LIST
#undef X

// ----------------
// UNDEF ALL MACROS
// ----------------

#undef PARSE_signed
#undef PARSE_unsigned
#undef PARSE_floating
#undef PARSE

#undef CHECK_BAD_RANGE_signed
#undef CHECK_BAD_RANGE_unsigned
#undef CHECK_BAD_RANGE_floating
#undef CHECK_BAD_RANGE

#undef signed_t
#undef unsigned_t
#undef floating_t

#undef SET_STATUS
#undef DEFAULT_DELIM

#endif // MATOOLS_IMPLEMENTATION

#undef STATUS_CODES
#undef TYPES_LIST

#endif // MATOOLS_H
