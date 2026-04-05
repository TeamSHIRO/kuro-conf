#include <stdint.h>

#ifndef MAIN_H
#define MAIN_H

#define PUBLIC_KEY_SIZE 32
#define EXECUTABLE_PATH_SIZE 256
#define PATH_PREFIX "./"
#define HEX_COLS 5
#define LABEL_WIDTH 21

#define K_MAGIC0 0x7F
#define K_MAGIC1 0x4B // K
#define K_MAGIC2 0x55 // U
#define K_MAGIC3 0x52 // R
#define K_MAGIC4 0x4F // O

#define MAGIC_SIZE 5

#define K_VERSION_1 1
#define K_VERSION_UNSUPPORTED 0

// clang-format off
#define KURO_ASCII                                                                                                     \
    "                            ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n"                                                                         \
    "                            ⠀⠀⠀⣄⠀⣤⡤⠀⠀⠀\n"                                                                         \
    "                            ⠀⠀⠀⣿⣷⣏⠀⠀⠀⠀\n"                                                                         \
    "                            ⠀⠀⠀⠛⠛⠛⠓⠀C⠀\n"                                                                         \
    "                            ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀"
// clang-format on

#define DEFAULT_CONFIG_CONTENT                                                                                         \
    "VERSION=2\nHAS_PUBLIC_KEY=1\nASLR_ENABLED=1\nPUBLIC_KEY=kernel.pub\nEXECUTABLE_PATH=kernel.bin\n"

typedef struct {
    char k_magic0;
    char k_magic1;
    char k_magic2;
    char k_magic3;
    char k_magic4;
    uint8_t k_version;
    uint16_t k_reserved;
} KuroIdentifier;

typedef struct {
    KuroIdentifier identifier;
    uint8_t has_public_key;
    uint8_t aslr_enabled;
    char public_key[PUBLIC_KEY_SIZE];
    char executable_path[EXECUTABLE_PATH_SIZE];
} KuroConfig;

// Total size: 554 bytes.

#endif // MAIN_H
