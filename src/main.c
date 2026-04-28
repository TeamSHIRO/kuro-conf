#include "main.h"

#include <errno.h>
#include <getopt.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ansi.h"
#include "file.h"
#include "gh.h"
#include "logger.h"
#include "kuro_conf.h"

int mkconf(const char *config_path) {
    if (config_path == NULL) {
        if (write_file("kuro.conf", (const unsigned char *) DEFAULT_CONFIG_CONTENT, strlen(DEFAULT_CONFIG_CONTENT)) !=
            0) {
            k_error("Failed to create configuration file.");
            return 1;
        }
    } else {
        if (write_file(config_path, (const unsigned char *) DEFAULT_CONFIG_CONTENT, strlen(DEFAULT_CONFIG_CONTENT)) !=
            0) {
            k_error("Failed to create configuration file.");
            return 1;
        }
    }

    k_success("Configuration file created successfully.");

    return 0;
}

// NOLINTNEXTLINE
int decode_kuro_config_file(const char *config_path, KuroConfig *config) {
    size_t file_size = 0;
    char *file_buffer = read_whole_file(config_path, &file_size);
    if (file_buffer == NULL) {
        k_error("Failed to read config file \"%s\": %s (Error code: %d)", config_path, strerror(errno), errno);
        return 1;
    }

    char *line = strtok(file_buffer, "\n");
    while (line != NULL) {
        // Skip empty lines and comments
        if (line[0] == '\0' || line[0] == '#') {
            line = strtok(NULL, "\n");
            continue;
        }

        char *eq = strchr(line, '=');
        if (eq == NULL) {
            k_error("Invalid config line: %s", line);
            return 1;
        }

        *eq = '\0';
        char *key = line;
        char *value = eq + 1;

        // Trim leading/trailing whitespace from key
        while (*key == ' ' || *key == '\t') {
            key++;
        }
        char *key_end = key + strlen(key) - 1;
        while (key_end > key && (*key_end == ' ' || *key_end == '\t')) {
            *key_end = '\0';
            key_end--;
        }
        // Trim leading/trailing whitespace from value
        while (*value == ' ' || *value == '\t') {
            value++;
        }
        char *value_end = value + strlen(value) - 1;
        while (value_end > value && (*value_end == ' ' || *value_end == '\t')) {
            *value_end = '\0';
            value_end--;
        }

        if (strcmp(key, "VERSION") == 0) {
            char *endptr;
            long val = strtol(value, &endptr, DECIMAL_BASE);
            if (*endptr != '\0') {
                k_error("Invalid integer value for VERSION: %s", value);
                return 1;
            }
            config->identifier.k_version = (int) val;
        } else if (strcmp(key, "SECURE_MODE") == 0) {
            char *endptr;
            long val = strtol(value, &endptr, DECIMAL_BASE);
            if (*endptr != '\0') {
                k_error("Invalid integer value for SECURE_MODE: %s", value);
                return 1;
            }
            config->secure_mode = (uint8_t) val;
        } else if (strcmp(key, "ASLR_ENABLED") == 0) {
            char *endptr;
            long val = strtol(value, &endptr, DECIMAL_BASE);
            if (*endptr != '\0') {
                k_error("Invalid integer value for ASLR_ENABLED: %s", value);
                return 1;
            }
            config->aslr_enabled = (uint8_t) val;
        } else if (strcmp(key, "LOG_LEVEL") == 0) {
            char *endptr;
            long val = strtol(value, &endptr, DECIMAL_BASE);
            if (*endptr != '\0') {
                k_error("Invalid integer value for LOG_LEVEL: %s", value);
                return 1;
            }
            config->log_level = (uint8_t) val;
        } else if (strcmp(key, "CONSOLE_LOG_LEVEL") == 0) {
            char *endptr;
            long val = strtol(value, &endptr, DECIMAL_BASE);
            if (*endptr != '\0') {
                k_error("Invalid integer value for CONSOLE_LOG_LEVEL: %s", value);
                return 1;
            }
            config->console_log_level = (uint8_t) val;
        } else if (strcmp(key, "PUBLIC_KEY") == 0) {
            size_t pkey_file_size = 0;
            char *pkey_file_buffer = read_whole_file(value, &pkey_file_size);
            if (pkey_file_buffer == NULL) {
                k_error("Failed to read public key file \"%s\": %s (Error code: %d)", value, strerror(errno), errno);
                return 1;
            }
            size_t copy_len = pkey_file_size < PUBLIC_KEY_SIZE ? pkey_file_size : PUBLIC_KEY_SIZE;
            memcpy(config->public_key, pkey_file_buffer, copy_len);
            free(pkey_file_buffer);
        } else {
            k_error("kuro-conf.parsingError: Unknown config key: %s", key);
            return 1;
        }

        line = strtok(NULL, "\n");
    }

    free(file_buffer);

    return 0;
}

int get_config_footer(const char *config_path, KuroConfig *footer) {
    FILE *fptr = fopen(config_path, "rb");

    if (fptr == NULL) {
        k_error("Failed to open binary \"%s\": %s (Error code: %d)", config_path, strerror(errno), errno);
        return 1;
    }

    if (fseek(fptr, -(long) sizeof(KuroConfig), SEEK_END) != 0) {
        k_error("Failed to seek in binary \"%s\": %s (Error code: %d)", config_path, strerror(errno), errno);
        (void) fclose(fptr);
        return 1;
    }

    if (fread(footer, sizeof(KuroConfig), 1, fptr) != 1) {
        k_error("Failed to read config from binary \"%s\": %s (Error code: %d)", config_path, strerror(errno), errno);
        (void) fclose(fptr);
        return 1;
    }

    return 0;
}

void print_version() {
    int out_of_date = 0;
    char *latest = NULL;
    gh_get_latest_published_version(&out_of_date, &latest);

    printf("\n");
    printf("🭌🬿🭠🭗  kuro-conf version " A_BOLD PROJECT_VERSION A_RESET "\n");
    printf("██🭏🬼  ");

    if (out_of_date) {
        printf(T_YELLOW A_BOLD "⬤ new version available! (%s)" A_RESET "\n", latest);
    } else {
        printf(T_GREEN A_BOLD "⬤ up to date" A_RESET "\n");
    }

    printf("\n");
}

// NOLINTNEXTLINE
int parse_args(int argc, char *argv[], char **config_file, int *show_version) {
    int opt;
    static struct option long_options[] = {{"config", required_argument, 0, 'c'}, {"version", no_argument, 0, 'v'}};
    int option_index = 0;
    while ((opt = getopt_long(argc, argv, ":c:v", long_options, &option_index)) != -1) {
        switch (opt) { // NOLINT
            case 'c':
                *config_file = optarg;
                break;
            case 'v':
                *show_version = 1;
                break;
            case ':':
                k_error("Option -%c requires an argument", optopt);
                return 1;
            case '?':
            default:
                k_error("Unknown option -%c", optopt);
                return 1;
        }
    }
    return 0;
}

void print_usage() {
    printf(A_BOLD KURO_ASCII A_RESET "\n");
    printf(A_BOLD "                             KURO-CONF\n" A_RESET);
    printf("         A tool for configuring the KURO UEFI bootloader.\n");
    printf(A_DIM "More details on KURO can be found at: https://github.com/TeamSHIRO/KURO\n\n" A_RESET);
    printf(A_BOLD "Usage:\n" A_RESET);
    printf("  kuro-conf <command> [options]\n\n");
    printf(A_BOLD "Commands:\n" A_RESET);
    printf("  read    Read the configuration of the KURO UEFI bootloader\n");
    printf("  edit    Edit the configuration of the KURO UEFI bootloader with a .conf file\n");
    printf("  mkconf  Create a new template configuration file for the KURO UEFI bootloader\n");
    printf("  help    Display this help message\n");
    printf(A_BOLD "Options:\n" A_RESET);
    printf("  -c --config      <file>    Specify the configuration file to use\n");
    printf("  -v --version               Show the version of kuro-conf\n");
    printf("\n");
}

// NOLINTNEXTLINE
int edit_config(const char *bootloader_path, const char *config_file) {
    KuroConfig config;
    KuroConfig footer;

    if (bootloader_path == NULL) {
        k_error("Bootloader path is required");
        return 1;
    }

    if (config_file == NULL) {
        k_error("Configuration file is required!");
        printf(A_DIM "     > Tip! Retry again with `kuro-conf %s -c {config_file}`\n" A_RESET, bootloader_path);
        return 1;
    }

    if (decode_kuro_config_file(config_file, &config) != 0) {
        k_error("Failed to decode configuration file: %s", config_file);
        return 1;
    }

    static const uint8_t K_MAGIC[KURO_MAGIC_LEN] = KURO_MAGIC;

    int footer_exists = (get_config_footer(bootloader_path, &footer) == 0) &&
                        (memcmp(footer.identifier.k_magic, K_MAGIC, KURO_MAGIC_LEN) == 0);

    // Set magic values in config footer
    memcpy(config.identifier.k_magic, K_MAGIC, KURO_MAGIC_LEN);

    memset(config.identifier.k_reserved, 0, sizeof(config.identifier.k_reserved));

    FILE *fptr = NULL;
    if (footer_exists) {
        // Overwrite the existing footer
        fptr = fopen(bootloader_path, "rb+");
        if (fptr == NULL) {
            k_error("Failed to open bootloader binary \"%s\" for updating: %s (Error code: %d)", bootloader_path,
                    strerror(errno), errno);
            return 1;
        }
        if (fseek(fptr, -(long) sizeof(KuroConfig), SEEK_END) != 0) {
            k_error("Failed to seek to footer in bootloader binary \"%s\": %s (Error code: %d)", bootloader_path,
                    strerror(errno), errno);
            (void) fclose(fptr);
            return 1;
        }
        if (fwrite(&config, sizeof(KuroConfig), 1, fptr) != 1) {
            k_error("Failed to overwrite footer in bootloader binary \"%s\": %s (Error code: %d)", bootloader_path,
                    strerror(errno), errno);
            (void) fclose(fptr);
            return 1;
        }
        if (fclose(fptr) != 0) {
            k_error("Failed to close bootloader binary \"%s\": %s (Error code: %d)", bootloader_path, strerror(errno),
                    errno);
            return 1;
        }
        k_success("KURO Footer overwritten in \"%s\"", bootloader_path);
    } else {
        // Append new footer
        fptr = fopen(bootloader_path, "ab");
        if (fptr == NULL) {
            k_error("Failed to open bootloader binary \"%s\" for writing: %s (Error code: %d)", bootloader_path,
                    strerror(errno), errno);
            return 1;
        }
        if (fwrite(&config, sizeof(KuroConfig), 1, fptr) != 1) {
            k_error("Failed to write footer to bootloader binary \"%s\": %s (Error code: %d)", bootloader_path,
                    strerror(errno), errno);
            (void) fclose(fptr);
            return 1;
        }
        if (fclose(fptr) != 0) {
            k_error("Failed to close bootloader binary \"%s\": %s (Error code: %d)", bootloader_path, strerror(errno),
                    errno);
            return 1;
        }
        k_success("KURO Footer appended to \"%s\"", bootloader_path);
    }

    return 0;
}

int read_config(const char *bootloader_path) {
    KuroConfig config;
    if (get_config_footer(bootloader_path, &config) != 0) {
        k_error("Failed to read configuration from bootloader binary \"%s\"", bootloader_path);
        return 1;
    }
    static const uint8_t K_MAGIC[KURO_MAGIC_LEN] = KURO_MAGIC;

    printf("Bootloader identifier:\n");
    printf("  Magic:              ");
    for (int i = 0; i < MAGIC_SIZE; i++) {
        printf("%02X ", (uint8_t) config.identifier.k_magic[i]);
    }
    if (memcmp(config.identifier.k_magic, K_MAGIC, KURO_MAGIC_LEN) == 0) {
        printf(T_GREEN A_BOLD "  ⬤ valid" A_RESET);
    } else {
        printf(T_RED A_BOLD "  ⬤ invalid" A_RESET);
    }
    printf("\n");
    printf("  Version:            %d", config.identifier.k_version);
    if (config.identifier.k_version == KURO_CONFIG_VERSION_1) {
        printf(T_GREEN A_BOLD "                ⬤ stable" A_RESET);
    } else {
        printf(T_RED A_BOLD "                ⬤ invalid" A_RESET);
    }
    printf("\n");
    printf("  Reserved:           %02X %02X\n", (uint8_t) config.identifier.k_reserved[0],
           (uint8_t) config.identifier.k_reserved[1]);

    int has_public_key = 0;
    for (int i = 0; i < PUBLIC_KEY_SIZE; i++) {
        if (config.public_key[i] != 0) {
            has_public_key = 1;
            break;
        }
    }

    printf("Bootloader configuration:\n");
    if (has_public_key) {
        printf("  Public key:         ");

        for (int i = 0; i < PUBLIC_KEY_SIZE; i++) {
            if (i > 0 && i % HEX_COLS == 0) {
                printf("\n                      ");
            }

            printf("%02X ", config.public_key[i]);
            if (i == HEX_COLS - 1) {
                printf(T_GREEN A_BOLD "  ⬤ enabled" A_RESET);
            }
        }
    } else {
        printf("  Public key:         " T_YELLOW A_BOLD "                 ⬤ disabled" A_RESET);
    }
    printf("\n");
    printf("  Secure mode:                         ");
    if (config.secure_mode) {
        printf(T_GREEN A_BOLD "⬤ enabled" A_RESET);
    } else {
        printf(T_YELLOW A_BOLD "⬤ disabled" A_RESET);
    }
    printf("\n");
    printf("  ASLR:                                ");
    if (config.aslr_enabled) {
        printf(T_GREEN A_BOLD "⬤ enabled" A_RESET);
    } else {
        printf(T_YELLOW A_BOLD "⬤ disabled" A_RESET);
    }
    printf("\n");
    printf("  Log level:          %d\n", config.log_level);
    printf("  Console log level:  %d\n", config.console_log_level);
    printf("  String offset:      0x%08X\n", config.str_offset);

    return 0;
}

int main(int argc, char *argv[]) {
    char *config_file = NULL;
    int show_version = 0;

    if (parse_args(argc, argv, &config_file, &show_version) != 0) {
        print_usage();
        return 1;
    }

    if (show_version) {
        print_version();
        return 0;
    }

    if (optind < argc) {
        char *command = argv[optind];
        char *bootloader_path = (optind + 1 < argc) ? argv[optind + 1] : NULL;

        if (strcmp((const char *) command, "edit") == 0) {
            if (edit_config(bootloader_path, config_file) != 0) {
                return 1;
            }
        } else if (strcmp((const char *) command, "help") == 0) {
            print_usage();
        } else if (strcmp((const char *) command, "read") == 0) {
            if (read_config(bootloader_path) != 0) {
                return 1;
            }
        } else if (strcmp((const char *) command, "mkconf") == 0) {
            if (mkconf(config_file) != 0) {
                return 1;
            }
        } else {
            k_error("Unknown command: %s \n", command);
            print_usage();
        }
    } else {
        print_usage();
    }
    return 0;
}
