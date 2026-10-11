cmake_minimum_required(VERSION 3.10)

# Название проекта
project(EcoActivationFunctions C)

# Установка стандарта C (C99 строго соблюдает требования ACOM)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Исправление для macOS: автоматическое определение актуального SDK для устранения ошибок линковки arm64e
if(APPLE)
    execute_process(
        COMMAND xcrun --show-sdk-path
        OUTPUT_VARIABLE CMAKE_OSX_SYSROOT
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif()

# Флаги компилятора: предупреждения и отключение неявных оптимизаций libc
if(MSVC)
    add_compile_options(/W4 /Za)
else()
    add_compile_options(-Wall -Wextra -pedantic -fno-builtin)
endif()

# Директории с заголовочными файлами
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc
)

# Исходные файлы компонента
set(COMPONENT_SOURCES
    Src/CEcoActivationFunctions.c
    Src/CEcoNeuralLayer.c
    EcoMathMathUtils.c
)

# 1. Сборка компонента в виде статической библиотеки (для прошивок/SoC/микроконтроллеров)
add_library(EcoActivationFunctionsStatic STATIC ${COMPONENT_SOURCES})
set_target_properties(EcoActivationFunctionsStatic PROPERTIES OUTPUT_NAME "EcoActivationFunctions")

# 2. Сборка консольного тестового клиента (Unit-тесты и замеры производительности)
add_executable(test_suite
    Test/test_main.c
)

# Связывание тестов с компонентом ACOM
target_link_libraries(test_suite PRIVATE EcoActivationFunctionsStatic)

# Вывод информации о сборке
message(STATUS "ACOM Project configured successfully.")
message(STATUS "Build target: test_suite")