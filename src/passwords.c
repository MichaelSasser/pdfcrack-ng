/**
 * Copyright (C) 2016 Michael Sasser
 * Copyright (C) 2006-2015 Henning Norén
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

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "passwords.h"
#include "pdfcrack-ng.h"

#define PASSLENGTH 33

static FILE *wordList = NULL;
static const char *wordListName;
static bool wlMore;

static bool (*npw)() = NULL;

static unsigned int (*spw)(uint8_t *outbuf) = NULL;

static passwordMethod pwMethod;

static int nrOfZones;

bool nextPassword() { return npw(); }

unsigned int setPassword(uint8_t *outbuf) { return spw(outbuf); }

static bool wlNextPassword() { return wlMore; }

static unsigned int wlSetPassword(uint8_t *outbuf) {
    int ch;
    unsigned int passlength;

    passlength = 0;

    ch = getc(wordList);
    while (ch != '\n' && ch != '\r' && ch != EOF && passlength < 32) {
        outbuf[passlength++] = (uint8_t) ch;
        ch = getc(wordList);
    }

    /** clean up garbage of passwords longed than 32 chars */
    if (unlikely(passlength == 32))
        while (ch != '\n' && ch != '\r' && ch != EOF)
            ch = getc(wordList);

    if (ch == '\r') {
        ch = getc(wordList);
        if (ch != '\n')
            ungetc(ch, wordList);
    }
    if (unlikely(ch == EOF))
        wlMore = false;

    return passlength;

}

static void setWordList(FILE *file, const char *wl) {
    wordList = file;
    wordListName = wl;
    npw = &wlNextPassword;
    spw = &wlSetPassword;
    wlMore = true;
}

static const uint8_t stdchars[63] = {"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"};

static const uint8_t *charset;
static unsigned int charsetLen;
static unsigned int maxPasswordLen;
static int password[PASSLENGTH];

static unsigned int genSetPassword(uint8_t *outbuf) {
    unsigned int i;

    for (i = 0; password[i] != -1; i++)
        outbuf[i] = charset[password[i]];

    return i;
}

/*
 * Init password offset for zones (0 for zone 1, 1 for zone 2, etc.)
 */
static bool initPasswordOffsetZones(int off) {
    unsigned int i = 0;

    password[i] = off;
    while (password[i] >= (int) charsetLen) {
        if (password[i + 1] == -1) {
            password[i + 1] = 0;
        }
        password[i + 1] += password[i] / (int) charsetLen;
        password[i] = password[i] % (int) charsetLen;
        i++;
    }

    //	printPwd();
    return (i < maxPasswordLen);
}


static bool genNextPasswordZones() {
    unsigned int i = 0;

    // step size = nrOfZones
    password[i] += nrOfZones;
    while (password[i] >= (int) charsetLen) {
        if (password[i + 1] == -1) {
            password[i + 1] = 0;
        }
        password[i + 1] += password[i] / (int) charsetLen;
        password[i] = password[i] % (int) charsetLen;
        i++;
    }

    //	printPwd();
    return (i < maxPasswordLen);
}


static bool genNextPassword() {
    unsigned int i = 0;

    /** this is pretty simple...
        Change the current position (i) to the value in the next position of the
        charset.
        If we have reached the end of the charset, move the current position to
        the next one and return true unless we have reached the last position we
        want to try.
    */
    while (++password[i] == (int) charsetLen)
        password[i++] = 0;

    return (i != maxPasswordLen);
}

static bool recovery = false;

static void setCharset(const char *cs, const unsigned int minPw, const unsigned int maxPw, const int zone,
                       const int nrOfZones_local) {
    int i;
    unsigned int min;

    nrOfZones = nrOfZones_local;


    if (nrOfZones > 1) {
        npw = &genNextPasswordZones;
    } else {
        npw = &genNextPassword;
    }
    spw = &genSetPassword;

    if (!recovery) {
        /** This should already be set if we are loading from a saved state */
        if (cs)
            charset = (const uint8_t *) cs;
        else
            charset = stdchars;
        charsetLen = (unsigned int) strlen((const char *) charset);

        /** Make sure that max- and min-password are smaller than 32 */
        if (maxPw < PASSLENGTH)
            maxPasswordLen = maxPw;
        else
            maxPasswordLen = PASSLENGTH - 1;
        if (minPw < PASSLENGTH)
            min = minPw;
        else
            min = PASSLENGTH - 1;

        /** Initialize starting position */
        for (i = 0; i < (int) maxPasswordLen; i++) {
            if (i < ((int) min) - 1)
                password[i] = charsetLen - 1;
            else
                password[i] = -1;
        }
        while (i < PASSLENGTH - 1)
            password[i++] = -1;
    }
    /** Put terminator (-1) at the last position */
    password[PASSLENGTH - 1] = -1;

    if (nrOfZones > 1) {
        //	  printf("before init offset:\n");
        //	  printPwd();
        initPasswordOffsetZones(zone - 1); // init to the offset (which is zone - 1)
        //	  printf("after init offset:\n");
        //	  printPwd();
    }

}

void initPasswords(const passwordMethod pm, FILE *file, const char *wl, const char *cs, const unsigned int minPw,
                   const unsigned int maxPw, const int zone, const int nrOfZones_local) {
    if (!recovery)
        pwMethod = pm;

    switch (pwMethod) {
        case Generative:
            setCharset(cs, minPw, maxPw, zone, nrOfZones_local);
            break;
        case Wordlist:
            setWordList(file, wl);
            break;
        default:
            /** The programmer is a twit!  */
            break;
    }
}

/** Common patterns that is shared between pw_loadState and pw_saveState */
static const char string_PM[] = "\nPM: %d\n";
static const char string_MPCLC[] = "MaxPWL: %d\nCharset(%d): ";

bool pw_loadState(FILE *file, char **wl) {
    int pm, len;
    unsigned int i;
    char *__restrict string;

    if (fscanf(file, string_PM, &pm) < 1)
        return false;
    if (pm == Generative) {
        if (fscanf(file, string_MPCLC, &maxPasswordLen, &charsetLen) < 2)
            return false;

        /** check for extremely long charsets */
        if (charsetLen > 256)
            return false;

        string = malloc(sizeof(uint8_t) * charsetLen + 1);
        for (i = 0; i < charsetLen; i++)
            string[i] = (char) getc(file);
        string[i] = '\0';
        charset = (uint8_t *) string;

        /** get the linebreak */
        getc(file);

        for (i = 0; i < PASSLENGTH - 1; i++)
            if (fscanf(file, " %d", &password[i]) < 1) {
                free(string);
                return false;
            }
    }
    else if (pm == Wordlist) {
        if (fscanf(file, "Wordlist(%d): ", &len) < 1)
            return false;
        string = malloc(sizeof(char) * len + 1);
        if (fscanf(file, "%[^\n]\n", string) < 1) {
            free(string);
            return false;
        }
        string[len] = '\0';
        *wl = string;
        wordListName = string;
    }
    pwMethod = (passwordMethod) pm;
    recovery = true;

    return true;
}

void pw_saveState(FILE *file) {
    //setPwdMultiThread(&password[0], charset, charsetLen);
    unsigned int i;
    static int pwdMultiThread[32];
    int *pwdArray = password;

    fprintf(file, string_PM, pwMethod);
    if (pwMethod == Generative) {
        fprintf(file, string_MPCLC, maxPasswordLen, charsetLen);
        fprintf(file, "%s\n", charset);
        // adaption for multithreaded version
        if (getNrOfThreads() > 1) {
            // Find smallest password currently processed by the different threads
            pwdArray = pwdMultiThread;
            setPwdMultiThread(&pwdMultiThread[0], charset, charsetLen);
        }
        for (i = 0; i < PASSLENGTH - 1; i++)
            fprintf(file, " %d", pwdArray[i]);
    }
    else if (pwMethod == Wordlist) {
        fprintf(file, "Wordlist(%lu): %s", (size_t) strlen(wordListName), wordListName);
    }
    fprintf(file, "\n");
}
