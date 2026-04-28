#include <stdint.h>

#ifndef MAIN_H
#define MAIN_H

#define PUBLIC_KEY_SIZE 32
#define HEX_COLS 5
#define LABEL_WIDTH 21
#define DECIMAL_BASE 10
#define MAGIC_SIZE KURO_MAGIC_LEN

// clang-format off
#define KURO_ASCII                                                                                                     \
    "                            ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n"                                                                         \
    "                            ⠀⠀⠀⣄⠀⣤⡤⠀⠀⠀\n"                                                                         \
    "                            ⠀⠀⠀⣿⣷⣏⠀⠀⠀⠀\n"                                                                         \
    "                            ⠀⠀⠀⠛⠛⠛⠓⠀C⠀\n"                                                                         \
    "                            ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀"
// clang-format on

#define DEFAULT_CONFIG_CONTENT                                                                                         \
    "VERSION=1\nSECURE_MODE=0\nASLR_ENABLED=1\nLOG_LEVEL=3\nCONSOLE_LOG_LEVEL=3\nPUBLIC_KEY=kernel.pub\n"

#endif // MAIN_H
