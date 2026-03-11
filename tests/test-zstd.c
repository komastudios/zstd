#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "zstd.h"

#define ASSERT(expr, msg) do { \
    if(!(expr)) { \
        fprintf(stderr, "%s:%d: Assertion failed: %s\n", __FILE__, __LINE__, msg); \
        exit(EXIT_FAILURE); \
    } \
} while(0)

#define ASSERT_EQ(val1, val2) do { \
    if((val1) != (val2)) { \
        fprintf(stderr, "%s:%d: Expected: %s == %s\n", \
                __FILE__, __LINE__, #val1, #val2); \
        exit(EXIT_FAILURE); \
    } \
} while(0)

void test_version(void) {
    ASSERT_EQ(ZSTD_versionNumber(), ZSTD_VERSION_NUMBER);
    fprintf(stderr, "  test_version passed\n");
}

void test_compress_bound(void) {
    size_t bound = ZSTD_compressBound(1024);
    ASSERT(bound > 0, "compressBound should be positive for valid input");

    size_t overflow_bound = ZSTD_compressBound((size_t)-1);
    ASSERT(ZSTD_isError(overflow_bound), "compressBound should return error for overflow");

    fprintf(stderr, "  test_compress_bound passed\n");
}

void test_compress_decompress(void) {
    const char src[] = "Hello, Zstd compression! This is a test of round-trip compression and decompression.";
    size_t src_size = sizeof(src);

    size_t max_dst_size = ZSTD_compressBound(src_size);
    ASSERT(!ZSTD_isError(max_dst_size), "compressBound failed");

    char* compressed = (char*)malloc(max_dst_size);
    ASSERT(compressed != NULL, "malloc failed");

    size_t compressed_size = ZSTD_compress(compressed, max_dst_size, src, src_size, 1);
    ASSERT(!ZSTD_isError(compressed_size), ZSTD_getErrorName(compressed_size));

    char* decompressed = (char*)malloc(src_size);
    ASSERT(decompressed != NULL, "malloc failed");

    size_t decompressed_size = ZSTD_decompress(decompressed, src_size, compressed, compressed_size);
    ASSERT_EQ(decompressed_size, src_size);
    ASSERT(memcmp(src, decompressed, src_size) == 0, "decompressed data does not match original");

    free(compressed);
    free(decompressed);
    fprintf(stderr, "  test_compress_decompress passed\n");
}

void test_decompress_failure(void) {
    const char src[] = "Test data for decompression failure.";
    size_t src_size = sizeof(src);

    size_t max_dst_size = ZSTD_compressBound(src_size);
    char* compressed = (char*)malloc(max_dst_size);
    ASSERT(compressed != NULL, "malloc failed");

    size_t compressed_size = ZSTD_compress(compressed, max_dst_size, src, src_size, 1);
    ASSERT(!ZSTD_isError(compressed_size), ZSTD_getErrorName(compressed_size));

    /* corrupt the compressed data */
    compressed[0] ^= 0xFF;

    char* decompressed = (char*)malloc(src_size);
    ASSERT(decompressed != NULL, "malloc failed");

    size_t result = ZSTD_decompress(decompressed, src_size, compressed, compressed_size);
    ASSERT(ZSTD_isError(result), "decompress should fail on corrupt data");

    /* truncated data */
    compressed[0] ^= 0xFF; /* restore first byte */
    result = ZSTD_decompress(decompressed, src_size, compressed, 1);
    ASSERT(ZSTD_isError(result), "decompress should fail on truncated data");

    free(compressed);
    free(decompressed);
    fprintf(stderr, "  test_decompress_failure passed\n");
}

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

    fprintf(stderr, "Running Zstd tests...\n");
    fprintf(stderr, "Zstd version: %d\n", ZSTD_versionNumber());

    test_version();
    test_compress_bound();
    test_compress_decompress();
    test_decompress_failure();

    fprintf(stderr, "All tests passed.\n");
    return 0;
}
