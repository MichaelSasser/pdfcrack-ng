/**
 * Copyright (C) 2016-2021 Michael Sasser
 * Copyright (C) 2006-2014 Henning Norén
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

#include "pdfparser.h"
#include <stdlib.h>
#include "version.h"


int main(int argc, char **argv) {
    int ret = 0;
    EncData *e;
    if (argc != 2) {
        printf("pdfreader-ng %s\n\n", VERSION);
        fprintf(stderr, "Usage: %s filename\n", argv[0]);
        exit(1);
    }

    FILE *file;
    if ((file = fopen(argv[1], "r")) == 0) {
        fprintf(stderr, "Error: file %s not found\n", argv[1]);
        exit(2);
    }
    e = calloc(1, sizeof(EncData));

    if (!openPDF(file, e)) {
        fprintf(stderr, "Error: Not a valid PDF\n");
        ret = 3;
        goto out;
    }

    ret = getEncryptedInfo(file, e);
    if (ret != 0) {
        if (ret == EENCNF)
            fprintf(stderr, "Error: Could not extract encryption information\n");
        else if (ret == ETRANF)
            fprintf(stderr, "Error: First trailer not found\n");
        else if (ret == ETRENF)
            fprintf(stderr, "Error: Encryption object not found in trailer\n");
        else if (ret == ETRINF)
            fprintf(stderr, "Error: ID object not found in trailer\n");
        ret = 4;
        goto out;
    }
    printEncData(e);

    freeEncData(e);

    if (fclose(file) != 0)
        fprintf(stderr, "Error: closing file %s\n", argv[1]);

    return 0;

    out:
    freeEncData(e);
    if (fclose(file) != 0)
        fprintf(stderr, "Error: closing file %s\n", argv[1]);
    exit(ret);
}
