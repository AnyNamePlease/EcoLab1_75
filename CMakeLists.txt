cmake_minimum_required(VERSION 3.10)
project(EcoLabProject C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# --- ДОБАВЬТЕ ЭТИ СТРОКИ ДЛЯ ОПРЕДЕЛЕНИЯ ПЛАТФОРМЫ И АРХИТЕКТУРЫ ---
if(APPLE)
    add_compile_definitions(ECO_APPLE)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64")
        add_compile_definitions(ECO_AARCH64)
    else()
        add_compile_definitions(ECO_X86_64)
    endif()
elseif(UNIX)
    add_compile_definitions(ECO_LINUX)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64")
        add_compile_definitions(ECO_AARCH64)
    else()
        add_compile_definitions(ECO_X86_64)
    endif()
elseif(WIN32)
    add_compile_definitions(ECO_WINDOWS)
    add_compile_definitions(ECO_X86_64)
endif()
# ------------------------------------------------------------------

# Пути к заголовочным файлам (интерфейсам)
include_directories(${CMAKE_SOURCE_DIR}/Src)
include_directories(${CMAKE_SOURCE_DIR}/Src_2)
include_directories(${CMAKE_SOURCE_DIR}/Inc)
include_directories(${CMAKE_SOURCE_DIR}/Eco.Framework/Eco.Core1/SharedFiles)
include_directories(${CMAKE_SOURCE_DIR}/Eco.Framework/Eco.MemoryManager1/SharedFiles)

# Исходные файлы EcoLab1
set(ECOLAB1_SOURCES
    Src/CEcoNeuralLayer.c
    Src/CEcoActivationFunctions.c
    Src/EcoMathMathUtils.c
)

# Исходные файлы EcoLab2 (если добавлены)
set(ECOLAB2_SOURCES
    Src_2/CEcoLab2.c
)

add_library(EcoComponents STATIC
    ${ECOLAB1_SOURCES}
    ${ECOLAB2_SOURCES}
)

add_executable(EcoLab1Test
    Test/test_lab1.c
)
target_link_libraries(EcoLab1Test PRIVATE EcoComponents)

add_executable(EcoLab2Test
    Test/test_lab2.c
)
target_link_libraries(EcoLab2Test PRIVATE EcoComponents)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)