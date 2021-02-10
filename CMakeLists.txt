# Copyright (C) 2016-2021 Michael Sasser
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
cmake_minimum_required(VERSION 3.1)

if(${CMAKE_VERSION} VERSION_LESS 3.19)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
    cmake_policy(VERSION 3.19)
endif()

project(pdfcrack-ng VERSION 0.1.1
                    DESCRIPTION "The Next Generation PDF Password Cracker"
                    LANGUAGES C)

configure_file (
    "${PROJECT_SOURCE_DIR}/src/version.h.in"
    "${PROJECT_BINARY_DIR}/src/version.h"
)

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

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(${PROJECT_BINARY_DIR}/src)

add_executable(pdfcrack-ng
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
               src/sha256.h
)
target_link_libraries(pdfcrack-ng ${CMAKE_THREAD_LIBS_INIT})

add_executable(pdfreader-ng
               src/common.c
               src/common.h
               src/pdfparser.c
               src/pdfparser.h
               src/pdfreader-ng.c
               )

install(TARGETS pdfcrack-ng)
install(TARGETS pdfreader-ng)
