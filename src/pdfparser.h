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

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "common.h"

#define EENCNF -1 /* Encryption Object Not Found */
#define ETRANF -2 /* Trailer Information Not Found */
#define ETRENF -3 /* Trailer: Encryption Object Not Found */
#define ETRINF -4 /* Trailer: FileID Object Not Found */

bool openPDF(FILE *file, EncData *e);

int getEncryptedInfo(FILE *file, EncData *e);

