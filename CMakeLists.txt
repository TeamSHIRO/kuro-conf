#
# KURO - A minimal monolithic UEFI bootloader
# =========================================
# Configure the KURO bootloader
#
# Build options:
# Name - Description

cmake_minimum_required(VERSION 3.26 FATAL_ERROR)

project(KURO_CONFIGURATOR
        VERSION 0.1.0
        DESCRIPTION "An configurator for the KURO UEFI bootloader"
        HOMEPAGE_URL "https://oshub.org/projects/kuro"
        LANGUAGES C
)

set(CMAKE_C_STANDARD 17)

set(KURO_CONFIGURATOR_SOURCES
        src/main.c
        src/file.c
        src/logger.c
        src/gh.c
)

add_executable(kuro-conf
        ${KURO_CONFIGURATOR_SOURCES}
)

target_compile_definitions(kuro-conf PRIVATE PROJECT_VERSION="${CMAKE_PROJECT_VERSION}")

find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)
target_link_libraries(kuro-conf PRIVATE OpenSSL::Crypto CURL::libcurl)