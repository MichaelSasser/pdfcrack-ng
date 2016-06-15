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

void rc4Decrypt(const uint8_t *key, const uint8_t *bs, const unsigned int len, uint8_t *out);

bool rc4Match40b(const uint8_t *key, const uint8_t *bs, const uint8_t *match);

bool setrc4DecryptMethod(const unsigned int length);

