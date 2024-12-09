cmake_minimum_required(VERSION 3.21)
project(LibJSNet)

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(DOTNET_DIR "${CMAKE_SOURCE_DIR}/dotnet")
set(LADYBIRD_DIR "$ENV{LADYBIRD_SOURCE_DIR}" CACHE STRING "Ladybird source directory")
set(AK_SOURCE_DIR "${LADYBIRD_DIR}/AK")
set(LIB_SOURCE_DIR "${LADYBIRD_DIR}/Libraries")

message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER} - MSVC: ${MSVC}")
message("SRC_DIR   \t${SRC_DIR}")
message("DOTNET_DIR\t${DOTNET_DIR}")
message("LADYBIRD_DIR\t${LADYBIRD_DIR}")
message("AK_SOURCE_DIR\t${AK_SOURCE_DIR}")
message("LIB_SOURCE_DIR\t${LIB_SOURCE_DIR}")
message("VCPKG_ROOT\t${VCPKG_ROOT}")
message("CMAKE_TOOLCHAIN_FILE\t${CMAKE_TOOLCHAIN_FILE}")

include("${LADYBIRD_DIR}/Meta/CMake/common_compile_options.cmake")
include("${LADYBIRD_DIR}/Meta/CMake/lagom_compile_options.cmake")
include("${LADYBIRD_DIR}/Meta/CMake/utils.cmake")

#Compile Options
add_definitions(-DLIBJS_SHARED)
add_compile_definitions(_POSIX_SOURCE _POSIX_C_SOURCE)
add_cxx_compile_options(
    -Wno-unused-variable
    -Wno-unused-parameter
    -Wno-unused-function
)

if(WIN32)
    if(MSVC)
        add_compile_options(/std:c++23)
        set(CMAKE_C_COMPILER_WORKS 1)
        set(CMAKE_CXX_COMPILER_WORKS 1)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /clang:-fno-exceptions")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /clang:-fno-exceptions")
    endif()

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -rtlib=compiler-rt")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rtlib=compiler-rt")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -rtlib=compiler-rt")
    endif()
endif()

#compiler-rt
find_library(COMPILER_RT_LIBRARY
    NAMES clang_rt.builtins-x86_64
    PATHS "C:/llvm/lib/clang/18/lib/windows"
)
message("COMPILER_RT_LIBRARY\t${COMPILER_RT_LIBRARY}")
#mman
find_path(MMAN_DIR NAMES sys/mman.h PATH_SUFFIXES mman)
find_library(MMAN_LIB mman)
message("MMAN_DIR\t${MMAN_DIR}")
message("MMAN_LIB\t${MMAN_LIB}")

#ICU
find_package(ICU 74 REQUIRED COMPONENTS data i18n uc)
message("ICU_VERSION\t${ICU_VERSION}")
message("ICU_data_FOUND\t${ICU_data_FOUND}")
message("ICU_i18n_FOUND\t${ICU_i18n_FOUND}")
message("ICU_uc_FOUND\t${ICU_uc_FOUND}")
message("ICU_INCLUDE_DIRS\t${ICU_INCLUDE_DIRS}")

# simdtuf
# I might need vcpkg install simdutf:x64-windows-dynamic
find_package(simdutf CONFIG REQUIRED)
message("SIMDUTF_FOUND\t${SIMDUTF_FOUND}")
message("SIMDUTF_VERSION\t${SIMDUTF_VERSION}")

# dirent
find_path(DIRENT_DIR NAMES dirent.h PATH_SUFFIXES dirent)
message("DIRENT_DIR\t${DIRENT_DIR}")

# Create a temporary Debug.h.in template
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/Debug.h.in "#pragma once")
configure_file(
    ${AK_SOURCE_DIR}/Debug.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/AK/Debug.h
    @ONLY
)

# Create a temporary Backtrace.h.in template
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/Backtrace.h.in "#pragma once")
configure_file(
    ${AK_SOURCE_DIR}/Backtrace.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/AK/Backtrace.h
    @ONLY
)

# == LibSyntax ==
file(GLOB_RECURSE LIBSYNTAX_SOURCES "${LIB_SOURCE_DIR}/LibSyntax/*.cpp")
add_library(LibSyntax ${LIBSYNTAX_SOURCES})
target_include_directories(LibSyntax PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${MMAN_DIR}
    "${LIB_SOURCE_DIR}/LibSyntax"
    "${LIB_SOURCE_DIR}/LibSyntax/.."
    "${AK_SOURCE_DIR}/.."
)

# == LibCrypto ==
file(GLOB_RECURSE LIBCRYPTO_SOURCES "${LIB_SOURCE_DIR}/LibCrypto/BigInt/*.cpp")
file(GLOB_RECURSE TMP "${LIB_SOURCE_DIR}/LibCrypto/BigFraction/*.cpp")
list(APPEND LIBCRYPTO_SOURCES ${TMP})
file(GLOB_RECURSE TMP "${LIB_SOURCE_DIR}/LibCrypto/NumberTheory/*.cpp")
list(APPEND LIBCRYPTO_SOURCES ${TMP})

add_library(LibCrypto ${LIBCRYPTO_SOURCES})
target_include_directories(LibCrypto PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${MMAN_DIR}
    "${LIB_SOURCE_DIR}/LibCrypto"
    "${LIB_SOURCE_DIR}/LibCrypto/.."
    "${AK_SOURCE_DIR}/.."
)

# == LibUnicode ==
file(GLOB_RECURSE LIBUNICODE_SOURCES "${LIB_SOURCE_DIR}/LibUnicode/*.cpp")
add_library(LibUnicode ${LIBUNICODE_SOURCES})
target_link_libraries(LibUnicode PRIVATE ICU::i18n ICU::uc ICU::data simdutf::simdutf)
target_include_directories(LibUnicode PRIVATE ${ICU_INCLUDE_DIRS})
target_include_directories(LibUnicode PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${MMAN_DIR}
    # "${ICU_INCLUDE_DIR}/unicode"
    "${LIB_SOURCE_DIR}/LibUnicode"
    "${LIB_SOURCE_DIR}/LibUnicode/.."
    "${AK_SOURCE_DIR}/.."
)

# == LibRegex ==
file(GLOB_RECURSE LIBREGEX_SOURCES "${LIB_SOURCE_DIR}/LibRegex/*.cpp")
add_library(LibRegex ${LIBREGEX_SOURCES})
target_include_directories(LibRegex PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${MMAN_DIR}
    "${LIB_SOURCE_DIR}/LibRegex"
    "${LIB_SOURCE_DIR}/LibRegex/.."
    "${AK_SOURCE_DIR}/.."
)
target_link_libraries(LibRegex LibUnicode)

# == LibCore ==
set(LIBCORE_SOURCES
    "${LIB_SOURCE_DIR}/LibCore/ElapsedTimer.cpp"
    "${LIB_SOURCE_DIR}/LibCore/DateTime.cpp"
    "${LIB_SOURCE_DIR}/LibCore/File.cpp"
    "${LIB_SOURCE_DIR}/LibCore/DirIterator.cpp"
    "${LIB_SOURCE_DIR}/LibCore/Directory.cpp"
    "${LIB_SOURCE_DIR}/LibCore/DirectoryEntry.cpp"
    # "${LIB_SOURCE_DIR}/LibCore/StandardPaths.cpp"
)

if(WIN32)
    list(APPEND LIBCORE_SOURCES "${LIB_SOURCE_DIR}/LibCore/SystemWindows.cpp")
else()
    list(APPEND LIBCORE_SOURCES "${LIB_SOURCE_DIR}/LibCore/System.cpp")
endif()

add_library(LibCore ${LIBCORE_SOURCES})
target_include_directories(LibCore PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${DIRENT_DIR}
    ${MMAN_DIR}
    "${LIB_SOURCE_DIR}/LibGC"
    "${LIB_SOURCE_DIR}/LibGC/.."
    "${AK_SOURCE_DIR}/.."
)
target_link_libraries(LibCore PUBLIC LibAK)

# == LibGC ==
file(GLOB_RECURSE LIBGC_SOURCES "${LIB_SOURCE_DIR}/LibGC/*.cpp")
add_library(LibGC ${LIBGC_SOURCES})
target_include_directories(LibGC PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${MMAN_DIR}
    "${LIB_SOURCE_DIR}/LibGC"
    "${LIB_SOURCE_DIR}/LibGC/.."
    "${AK_SOURCE_DIR}/.."
)
target_link_libraries(LibGC PUBLIC LibCore)

# == LibFileSystem ==
file(GLOB_RECURSE LIBFILESYSTEM_SOURCES "${LIB_SOURCE_DIR}/LibFileSystem/FileSystem.cpp")
add_library(LibFileSystem ${LIBFILESYSTEM_SOURCES})
target_include_directories(LibFileSystem PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${DIRENT_DIR}
    ${MMAN_DIR}
    "${LIB_SOURCE_DIR}/LibFileSystem"
    "${LIB_SOURCE_DIR}/LibFileSystem/.."
    "${AK_SOURCE_DIR}/.."
)

# == AK ==
set(LIBAK_SOURCES
    "${AK_SOURCE_DIR}/Assertions.cpp"
    "${AK_SOURCE_DIR}/Base64.cpp"
    "${AK_SOURCE_DIR}/ByteString.cpp"
    "${AK_SOURCE_DIR}/CircularBuffer.cpp"
    "${AK_SOURCE_DIR}/DeprecatedFlyString.cpp"
    "${AK_SOURCE_DIR}/Error.cpp"
    "${AK_SOURCE_DIR}/FloatingPointStringConversions.cpp"
    "${AK_SOURCE_DIR}/FlyString.cpp"
    "${AK_SOURCE_DIR}/Format.cpp"
    "${AK_SOURCE_DIR}/GenericLexer.cpp"
    "${AK_SOURCE_DIR}/JsonObject.cpp"
    "${AK_SOURCE_DIR}/JsonParser.cpp"
    "${AK_SOURCE_DIR}/JsonValue.cpp"
    "${AK_SOURCE_DIR}/MemoryStream.cpp"
    "${AK_SOURCE_DIR}/NumberFormat.cpp"
    "${AK_SOURCE_DIR}/Random.cpp"
    "${AK_SOURCE_DIR}/StackInfo.cpp"
    "${AK_SOURCE_DIR}/Stream.cpp"
    "${AK_SOURCE_DIR}/String.cpp"
    "${AK_SOURCE_DIR}/StringBase.cpp"
    "${AK_SOURCE_DIR}/StringBuilder.cpp"
    "${AK_SOURCE_DIR}/StringFloatingPointConversions.cpp"
    "${AK_SOURCE_DIR}/StringImpl.cpp"
    "${AK_SOURCE_DIR}/StringUtils.cpp"
    "${AK_SOURCE_DIR}/StringView.cpp"
    "${AK_SOURCE_DIR}/Time.cpp"
    "${AK_SOURCE_DIR}/Utf16View.cpp"
    "${AK_SOURCE_DIR}/Utf32View.cpp"
    "${AK_SOURCE_DIR}/Utf8View.cpp"
)

if(WIN32)
    list(APPEND LIBAK_SOURCES "${AK_SOURCE_DIR}/LexicalPathWindows.cpp")
else()
    list(APPEND LIBAK_SOURCES "${AK_SOURCE_DIR}/LexicalPath.cpp")
endif()
add_library(LibAK ${LIBAK_SOURCES})
target_include_directories(LibAK PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${MMAN_DIR}
    "${AK_SOURCE_DIR}/.."
)
target_link_libraries(LibAK PRIVATE simdutf::simdutf)
if(WIN32)
    target_link_libraries(LibAK PRIVATE ${COMPILER_RT_LIBRARY})
endif()

# == LibJS ==
# -- Add all the files
file(GLOB_RECURSE LIBJS_SOURCES "${LIB_SOURCE_DIR}/LibJS/*.cpp")
list(FILTER LIBJS_SOURCES EXCLUDE REGEX "Tests|Contrib")
add_library(LibJS ${LIBJS_SOURCES})
target_include_directories(LibJS PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${MMAN_DIR}
    "${LIB_SOURCE_DIR}/LibJS"
    "${LIB_SOURCE_DIR}/LibJS/.."
    "${AK_SOURCE_DIR}/.."
)
# -- Link LIbraries. If we are on Windows, we also need DBHELP
if(!WIN32)
    #-- Linux specific flags
    target_compile_options(LibJS PRIVATE -fno-omit-frame-pointer)
else()
    # - Include demangling
    file(READ "src/AK/Demangle.h" FILE_CONTENT)
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/AK/Demangle.h "${FILE_CONTENT}")
endif()
target_link_libraries(LibJS LibGC LibCrypto LibCore LibAK LibUnicode LibFileSystem)

# == LibJSNet ==
file(GLOB LIBJSNET_SOURCE src/*.cpp)
add_library(LibJSNet SHARED ${LIBJSNET_SOURCE})
# target_compile_definitions(LibJSNet PRIVATE BUILDING_LIBJS)
target_include_directories(LibJSNet PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${MMAN_DIR}
    ${SRC_DIR}
    "${AK_SOURCE_DIR}/.."
    "${LIB_SOURCE_DIR}/"
)

target_link_libraries(LibJSNet PRIVATE simdutf::simdutf)
target_link_libraries(LibJSNet PRIVATE LibAK)
target_link_libraries(LibJSNet PRIVATE LibGC)
target_link_libraries(LibJSNet PRIVATE LibCore)
target_link_libraries(LibJSNet PRIVATE LibCrypto)
target_link_libraries(LibJSNet PRIVATE LibJS)
target_link_libraries(LibJSNet PRIVATE LibUnicode)
target_link_libraries(LibJSNet PRIVATE LibRegex)
target_link_libraries(LibJSNet PRIVATE LibFileSystem)
if(WIN32)

    target_link_libraries(LibJSNet PRIVATE ${COMPILER_RT_LIBRARY})
    target_link_libraries(LibJSNet PRIVATE ${MMAN_LIB})

    target_link_libraries(LibJSNet PRIVATE ICU::i18n)
    target_link_libraries(LibJSNet PRIVATE ICU::uc)
    target_link_libraries(LibJSNet PRIVATE ICU::data)
endif()
