# Copyright (C) 2016 Michael Sasser
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>

# Minimum required cmake version
cmake_minimum_required(VERSION 3.5)

# Project definition
project(pdfcrack-ng)

# Build Output
message("Building pdfcrack-ng...")

# Included Packages
find_package(Threads)

# C definitions
# Do not use -march=native or -mtune=native when compiling for multible CPUs.
add_definitions(#-std=c11
        -Wall
        -Wextra
        -D_FILE_OFFSET_BITS=64
        -D_LARGEFILE64_SOURCE
        -march=native
        -mtune=native
        -pipe
        -O2
        -g)

# Source files for pdfcrack-ng
set(SOURCE_FILES_PDFCRACK-NG
        src/benchmark.c
        src/benchmark.h
        src/common.c
        src/common.h
        src/main.c
        src/md5.c
        src/md5.h
        src/passwords.c
        src/passwords.h
        src/pdfcrack-ng.c
        src/pdfcrack-ng.h
        src/pdfparser.c
        src/pdfparser.h
        src/rc4.c
        src/rc4.h
        src/sha256.c
        src/sha256.h)

# Source files for pdfreader-ng
set(SOURCE_FILES_PDFREADER-NG
        src/common.c
        src/common.h
        src/pdfparser.c
        src/pdfparser.h
        src/pdfreader-ng.c)

# Output Directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Debug Output (Normally commented)
#message(STATUS "Using Variables:")
#get_cmake_property(_variableNames VARIABLES)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach ()

# pdfcrack-ng binary
add_executable(pdfcrack-ng ${SOURCE_FILES_PDFCRACK-NG})
target_link_libraries(pdfcrack-ng ${CMAKE_THREAD_LIBS_INIT})

# pdfreader-ng binary
add_executable(pdfreader-ng ${SOURCE_FILES_PDFREADER-NG})