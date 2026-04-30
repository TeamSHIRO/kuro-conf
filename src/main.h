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
    "VERSION=1\n"             \
    "SECURE_MODE=1\n"         \
    "ASLR_ENABLED=1\n"        \
    "LOG_LEVEL=1\n"           \
    "CONSOLE_LOG_LEVEL=0\n"   \
    "PUBLIC_KEY=\n" \
    "EXEC_PATH=\n"     \
    "MODULE_PATH=\n"          \
    "ARGS=\n"

#endif // MAIN_H
