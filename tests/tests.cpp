#include <catch2/catch_test_macros.hpp>

#include <cstdio>
#include <cstring>

extern "C" {
#include "Lab2.h"
}

/*------------------------------------------------------------
  Вспомогательная функция:
  создаёт FILE* из строки (MSVC-safe)
------------------------------------------------------------*/
static FILE* make_input_file(const char* text) {
    FILE* f = tmpfile();
    REQUIRE(f != nullptr);

    fputs(text, f);
    rewind(f);
    return f;
}

/*============================================================
  get_processed_string
============================================================*/

TEST_CASE("get_processed_string: nullptr") {
    REQUIRE(get_processed_string(nullptr) == nullptr);
}

TEST_CASE("get_processed_string: no digits") {
    char input[] = "Hello World!";
    char* result = get_processed_string(input);

    REQUIRE(result != nullptr);
    REQUIRE(strcmp(result, "Hello World!") == 0);

    free(result);
}

TEST_CASE("get_processed_string: even digits") {
    char input[] = "02468";
    char* result = get_processed_string(input);

    REQUIRE(result != nullptr);
    REQUIRE(strcmp(result, "ZEROTWOFOURSIXEIGHT") == 0);

    free(result);
}

TEST_CASE("get_processed_string: odd digits") {
    char input[] = "13579";
    char* result = get_processed_string(input);

    REQUIRE(result != nullptr);
    REQUIRE(strcmp(result, "#2#6#10#14#18") == 0);

    free(result);
}

TEST_CASE("get_processed_string: mixed string") {
    char input[] = "a1b2c3";
    char* result = get_processed_string(input);

    REQUIRE(result != nullptr);
    REQUIRE(strcmp(result, "a#2bTWOc#6") == 0);

    free(result);
}

TEST_CASE("get_processed_string: non-ascii ignored") {
    char input[] = { 'A', (char)0x80, '1', '\0' };
    char* result = get_processed_string(input);

    REQUIRE(result != nullptr);
    REQUIRE(strcmp(result, "A#2") == 0);

    free(result);
}

/*============================================================
  read_string_dynamic_malloc
============================================================*/

TEST_CASE("read_string_dynamic_malloc: simple") {
    FILE* f = make_input_file("hello\n");
    char* result = read_string_dynamic_malloc(f);

    REQUIRE(result != nullptr);
    REQUIRE(strcmp(result, "hello") == 0);

    free(result);
    fclose(f);
}

TEST_CASE("read_string_dynamic_malloc: empty line") {
    FILE* f = make_input_file("\n");
    char* result = read_string_dynamic_malloc(f);

    REQUIRE(result != nullptr);
    REQUIRE(strcmp(result, "") == 0);

    free(result);
    fclose(f);
}

TEST_CASE("read_string_dynamic_malloc: nullptr stream") {
    REQUIRE(read_string_dynamic_malloc(nullptr) == nullptr);
}

/*============================================================
  read_string_dynamic_realloc
============================================================*/

TEST_CASE("read_string_dynamic_realloc: simple") {
    FILE* f = make_input_file("hello\n");
    char* result = read_string_dynamic_realloc(f);

    REQUIRE(result != nullptr);
    REQUIRE(strcmp(result, "hello") == 0);

    free(result);
    fclose(f);
}

TEST_CASE("read_string_dynamic_realloc: long string") {
    FILE* f = make_input_file("abcdefghijklmnopqrstuvwxyz\n");
    char* result = read_string_dynamic_realloc(f);

    REQUIRE(result != nullptr);
    REQUIRE(strcmp(result, "abcdefghijklmnopqrstuvwxyz") == 0);

    free(result);
    fclose(f);
}

TEST_CASE("read_string_dynamic_realloc: nullptr stream") {
    REQUIRE(read_string_dynamic_realloc(nullptr) == nullptr);
}

/*============================================================
  read_and_process_string_to_file
============================================================*/

TEST_CASE("read_and_process_string_to_file: REALLOC") {
    const char* input_name = "test_input.txt";
    const char* output_name = "test_output.txt";

    // входной файл
    FILE* in = fopen(input_name, "w");
    REQUIRE(in != nullptr);
    fputs("a1b2\n", in);
    fclose(in);

    // перенаправляем stdin (MSVC-safe)
    freopen(input_name, "r", stdin);

    int err = read_and_process_string_to_file(REALLOC, (char*)output_name);
    REQUIRE(err == NONE);

    // проверяем выходной файл
    FILE* out = fopen(output_name, "r");
    REQUIRE(out != nullptr);

    char buffer[128] = {};
    fgets(buffer, sizeof(buffer), out);
    fclose(out);

    REQUIRE(strcmp(buffer, "a#2bTWO") == 0);

    remove(input_name);
    remove(output_name);
}
