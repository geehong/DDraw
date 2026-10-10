cmake_minimum_required(VERSION 3.15)
project(D2Draw02 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DUNICODE -D_UNICODE)

if(MSVC)
    add_compile_options(/utf-8 /FS)
endif()

# APP 소스코드 및 헤더파일 포함 (Tests, build 등 제외)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.rc"
)
list(FILTER SOURCES EXCLUDE REGEX "${CMAKE_CURRENT_SOURCE_DIR}/(build|out|Tests)/.*")

# 헤더 포함 경로 설정
include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/Common"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shapes"
    "${CMAKE_CURRENT_SOURCE_DIR}/Model"
    "${CMAKE_CURRENT_SOURCE_DIR}/Rendering"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editing"
    "${CMAKE_CURRENT_SOURCE_DIR}/Animation"
    "${CMAKE_CURRENT_SOURCE_DIR}/Commands"
)

# Windows GUI 앱 바이너리 생성
add_executable(D2Draw02 WIN32 ${SOURCES})

# Direct2D, DirectWrite, Win32 라이브러리 링크
target_link_libraries(D2Draw02 PRIVATE
    d2d1
    dwrite
    user32
    gdi32
    comctl32
)

# ---------------------------------------------------------
# 테스트 프레임워크 타깃 분리 (0단계)
# ---------------------------------------------------------
enable_testing()

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/Tests/*.cpp"
)

# pure C++ 모델/로직 소스들을 테스트 타깃에도 함께 빌드하기 위한 모음
file(GLOB_RECURSE MODEL_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/Model/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shapes/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Commands/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editing/*.cpp"
)

add_executable(D2Draw02_Tests ${TEST_SOURCES} ${MODEL_SOURCES})
target_link_libraries(D2Draw02_Tests PRIVATE
    d2d1
    dwrite
)

add_test(NAME D2Draw02_UnitTest COMMAND D2Draw02_Tests)

