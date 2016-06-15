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

#include <stdio.h>
#include <stdlib.h>
#include "common.h"

void freeEncData(EncData *e) {
    if (!e)
        return;
    if (e->o_string)
        free(e->o_string);
    if (e->u_string)
        free(e->u_string);
    if (e->fileID)
        free(e->fileID);
    if (e->s_handler)
        free(e->s_handler);
    free(e);
}

void printEncData(EncData *e) {
    unsigned int i;
    uint8_t ch;

    printf("\nPDF Information\n--------------\n");
    printf("PDF Version:\t\t%u.%u\n", e->version_major, e->version_minor);

    if (e->s_handler)
        printf("Security Handler:\t%s\n", e->s_handler);

    printf("Document Version:\t%d\nDocument Revision:\t%d\nDocument Permissions:\t%d\nEncryption Strength:\t%d Bit\n"
           "Encrypted Metadata:\t%s\nFileID:\t\t\t", e->version, e->revision, e->permissions, e->length, e->encryptMetaData ? "True" : "False");
    for (i = 0; i < e->fileIDLen; i++) {
        ch = e->fileID[i];
        if (ch < 16)
            printf("0%x", ch);
        else
            printf("%x", ch);
    }

    /** Assume u_string and o_string is of length 32. Not safe, but the code
        as a whole needs a rewrite anyway
    */
    if (e->u_string) {
        printf("\nUser Hash:  \"");
        for (i = 0; i < 32; i++) {
            ch = e->u_string[i];
            if (ch < 16)
                printf("0%x", ch);
            else
                printf("%x", ch);
        }
    }
    if (e->o_string) {
        printf("\"\nOwner Hash: \"");
        for (i = 0; i < 32; i++) {
            ch = e->o_string[i];
            if (ch < 16)
                printf("0%x", ch);
            else
                printf("%x", ch);
        }
    }
    printf("\"\n\nCracking\n--------\n");
}
