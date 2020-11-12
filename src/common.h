/**
 * Copyright (C) 2016 Michael Sasser
 * Copyright (C) 2006-2015 Henning Norén
 * Copyright (C) 2009 Andreas Meier, Michael Kuhn
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

/**
    EncData holds all the information regarding the encryption-setting of a
    specific pdf.
    s_handler - Security handler string.
    o_string - Owner-string, 32 bytes, not null-terminated
    u_string - User-string, 32 bytes, not null-terminated
    fileID - file ID in fileIDLen bytes, not null-terminated
*/
struct EncData {
    char *s_handler;
    uint8_t *o_string;
    uint8_t *u_string;
    uint8_t *fileID;
    bool encryptMetaData;
    unsigned int fileIDLen;
    unsigned int version_major;
    unsigned int version_minor;
    int length;
    int permissions;
    int revision;
    int version;
};

typedef struct EncData EncData;

typedef enum passwordMethod {
    Wordlist = 1,
    Generative
} passwordMethod;

void freeEncData(EncData *e);

void printEncData(EncData *e);
