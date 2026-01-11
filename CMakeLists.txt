cmake_minimum_required(VERSION 3.14)
project(Lab2 C CXX)

# Стандарты
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Предупреждения
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Тестирование
enable_testing()

add_library(lab2_lib STATIC
    Lab2.c
)

target_include_directories(lab2_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Тесты
add_subdirectory(tests)
