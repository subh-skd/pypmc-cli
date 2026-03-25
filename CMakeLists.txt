cmake_minimum_required(VERSION 3.10)
project(pypmc-cli VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(pypmc
    src/main.c
    src/config.c
    src/venv.c
    src/core.c
)

# Platform-specific settings
if(WIN32)
    target_compile_definitions(pypmc PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(pypmc PRIVATE -Wall -Wextra -O2)
endif()

install(TARGETS pypmc RUNTIME DESTINATION bin)
