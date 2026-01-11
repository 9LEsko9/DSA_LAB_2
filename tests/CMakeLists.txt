# Скачиваем Catch2
include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.2
)
FetchContent_MakeAvailable(Catch2)

# Создаем исполняемый файл с тестами (это основная программа проекта)
add_executable(lab2_tests tests.cpp)

# Линкуем с библиотекой и Catch2
target_link_libraries(lab2_tests PRIVATE 
    lab2_lib
    Catch2::Catch2WithMain
)

# Добавляем include директории
target_include_directories(lab2_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Регистрируем тесты в CTest
include(CTest)
include(Catch)
catch_discover_tests(lab2_tests)

# Делаем так, чтобы после сборки тесты запускались автоматически
add_custom_command(TARGET lab2_tests POST_BUILD
    COMMAND lab2_tests --reporter compact --success
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Автоматический запуск всех тестов..."
)