#pdfcrack-ng

## Introduction
pdfcrack-ng (pdfcrack next generation) is a simple tool written in "C" to recover forgotten passwords from PDFs.
It should be able to handle all PDFs that uses the standard security handler but the PDF-parsing routines are a bit of a
quick hack so you might stumble across some PDFs where the parser needs to be fixed to handle.

## Features

* Owner- and/or user-passwords with the "Standard Security Handler", revision 2, 3 and 5.
* Multithreading enabled by default.
* Search by wordlist or brute-forcing with auto or specific charset.
* Search by
* Optimized search for owner-password when user-password is known (or not set)
* Extremely simple permutations of passwords (makes first letter uppercase)

On brute-forcing with charsets:
* Auto-save the current state when interrupted (Ctrl-C or send SIGINT to the process).
* Resume on next start
* Minimum length of password to start at
* Maximum length of password to try

## Installation
Type 'cmake', then 'make' (or 'gmake' if you have BSD-make as default) to build the program. You will need to have
GNU Make and a recent version of GCC installed but there are no external dependencies on libraries.

## Tricks

* Sort your wordlist by length for best performance and consider that almost all passwords in PDFs are in iso latin 1
so use the correct character encoding in your terminal and/or wordlist when using special characters.
* To build a package for multi CPU envelopment remove "-march=native" and "-mtune=native" from the "CMakeLists.txt".

## Credits & License
* Copyright &copy; 2016 Michael Sasser
* Copyright &copy; 2006-2015 Henning Norén
* Copyright &copy; 2009 Andreas Meier and Michael Kuhn
* Copyright &copy; 1996-2005 Glyph & Cog, LLC.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not,
see <http://www.gnu.org/licenses/>.

Code and documentation are originally written by Henning Norén and extended by Andreas Meier and Michael Kuhn
Henning Norén used parts in pdfcrack.c and md5.c by derived/copied/inspired from xpdf and poppler. xpdfand poppler are
copyright 1995-2006 Glyph & Cog, LLC. The PDF data structures, operators, and specification are copyright
1985-2006 Adobe Systems Inc.
