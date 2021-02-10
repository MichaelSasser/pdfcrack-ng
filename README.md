# pdfcrack-ng

## Introduction
pdfcrack-ng (pdfcrack next generation) is a simple tool written in C to 
recover forgotten passwords from PDFs.

It should be able to handle all PDFs that uses the standard security handler 
but the PDF-parsing routines are a bit hacky. You might stumble across some 
PDFs where the parser needs to be fixed to handle them.

## Features

- Owner- and/or user-passwords with the Standard Security Handler
  revision 2, 3 and 5.
- Multithreading enabled by default.
- Search by wordlist or brute-forcing with auto or specific charset.
- Search by:
  - Optimized search for owner-password when user-password is known 
    or unset.
  - Simple optional permutation of passwords which makes first letter 
    uppercase.

When brute-forcing with charsets:
- It auto-saves the current state when interrupted (Ctrl-C or 
  SIGINT to the process) and resumes on next try.
- Optional minimum password length
- Optional maximum password length

## Installation
You don't need any external libraries. Just run:

```console
$ mkdir build
$ cd build
$ cmake ..  # or 'gmake ..' if you have BSD-make as default
$ make
```

You will find the binaries in `build/bin/`.

## Tricks

- Sort your wordlist by length for best performance and consider, that almost
  all passwords in PDFs are in iso latin 1.
  Use the correct character encoding in your terminal and/or wordlist when 
  using special characters.
- To build a package for multi CPU envelopment remove `-march=native` and
  `-mtune=native` from the `CMakeLists.txt`.


## Semantic Versioning

This repository uses [SemVer](https://semver.org/) for its release cycle.

## Branching Model

This repository uses the
[git-flow](https://danielkummer.github.io/git-flow-cheatsheet/index.html)
branching model by [Vincent Driessen](https://nvie.com/about/). It has two branches with infinite lifetime:

- [master](https://github.com/MichaelSasser/pdfcrack-ng/tree/master)
- [develop](https://github.com/MichaelSasser/pdfcrack-ng/tree/develop)

The master branch gets updated on every release. The develop branch is the merging branch.

## License & Credits

Copyright &copy; 2016-2021 Michael Sasser <Info@MichaelSasser.org>. 
Released under the GPLv3 license.\
Copyright &copy; 2006-2015 Henning Norén.\
Copyright &copy; 2009-2009 Andreas Meier and Michael Kuhn.\
Copyright &copy; 1996-2005 Glyph & Cog, LLC.

Code and documentation are originally written by Henning Norén and extended by
Andreas Meier and Michael Kuhn. Henning Norén was inspired by `xpdf` and
`poppler` and derived/copied parts of them to pdfcrack.c and md5.c. 

`xpdfand` and `poppler` are Copyright &copy; 1995 Glyph & Cog, LLC. 

The PDF data structures, operators, and specification are &copy Copyright 
1985 Adobe Systems Inc.
