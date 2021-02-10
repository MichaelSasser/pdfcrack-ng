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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#define __USE_GNU
#include <signal.h>
#undef __USE_GNU
#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include "pdfparser.h"
#include "pdfcrack-ng.h"
#include "benchmark.h"
#include "version.h"

#define PRINTERVAL 1 //Print Progress Interval (seconds)
#define SAVEINTERVAL 30 // in seconds (should be a multiple of PRINTINVERVAL)
#define CRASHFILE "savedstatecrash.sav"
#define SAVEFILE "savedstateperiodic.sav"

char crashStateFileName[64];
char periodicStateFileName[64];

/** alarmInterrupt is used to print out the progress at specific intervals */
static void alarmInterrupt() {
    static int counter = 0;

    if (!printProgress())
        alarm(PRINTERVAL);

    if (++counter * PRINTERVAL >= SAVEINTERVAL) {
        counter = 0;
        FILE *file;
        if ((file = fopen(periodicStateFileName, "w")) == 0) {
            fprintf(stderr, " [!] Could not save progress to file %s\n", periodicStateFileName);
            printf(" [!] Could not save progress to file %s\n", periodicStateFileName);
        } else {
            saveState(file);
            fclose(file);
            //printf("Successfully saved progress to %s\n", periodicStateFileName);
        }
    }
}

/** autoSave is used to save down the current state when interrupted */
__attribute__ ((noreturn)) static void autoSave(int sig) {
    FILE *file;

    if (sig)
        fprintf(stderr, "\n [*] Caught signal %d!\n [*] Trying to save state...\n", sig);
    if ((file = fopen(crashStateFileName, "w")) == 0)
        fprintf(stderr, " [!] Could not open %s for writing\n", crashStateFileName);
    else {
        saveState(file);
        fclose(file);
        fprintf(stderr, " [+] Successfully saved state to %s!\n", crashStateFileName);
    }
    exit(sig);
}

/** print some help for the user */
static void printHelp(char *progname) {
    printf("Usage: %s -f filename [OPTIONS]\n"
           "OPTIONS:\n"
           "-b, --bench\t\tperform benchmark and exit\n"
           "-c, --charset=STRING\tUse the characters in STRING as charset\n"
           "-w, --wordlist=FILE\tUse FILE as source of passwords to try\n"
           "-n, --minpw=INTEGER\tSkip trying passwords shorter than this\n"
           "-m, --maxpw=INTEGER\tStop when reaching this passwordlength\n"
           "-t, --threads=INTEGER\tNumber of thread. Recommended for multi-core\n"
           "\t\t\tSystems (default \"auto\", 0 = Don't spawn extra Threads)\n"
           "-l, --loadState=FILE\tContinue from the state saved in FILENAME\n"
           "-o, --owner\t\tWork with the ownerpassword\n"
           "-u, --user\t\tWork with the userpassword (default)\n"
           "-p, --password=STRING\tGive userpassword to speed up breaking\n"
           "\t\t\townerpassword (implies -o)\n"
           "-q, --quiet\t\tRun quietly\n"
           "-s, --permutate\t\tTry permutating the passwords (currently only\n"
           "\t\t\tsupports switching first character to uppercase)\n"
           "-v, --version\t\tPrint version and exit\n"
           "-z, --zone=INT1/INT2\tRun program on multiple hosts (split search space)\n"
           "\t\t\te.g. -z 3/8 (run on host 3 out of 8)\n", progname);
}

static bool getZoneInfo(char *arg, int *zone, int *nrOfZones) {

    //printf("zone parameter '%s'\n", arg);
    char *str = &arg[0];
    char ch = str[0];
    char zoneStr[8];
    char nrOfZonesStr[8];
    int i = 0;
    while (ch != '/' && ch != 0 && i < 8) {
        zoneStr[i] = ch;
        ch = str[++i];
    }

    if (i == 8 || ch == 0)
        return false; // malformed zone string

    // else: everything ok
    zoneStr[i] = 0;
    str = &arg[i + 1];
    i = 0;
    ch = str[0];
    while (ch != 0 && i < 8) {
        nrOfZonesStr[i] = ch;
        ch = str[++i];
    }

    if (i == 8)
        return false;

    nrOfZonesStr[i] = 0;
    *zone = atoi(zoneStr);
    *nrOfZones = atoi(nrOfZonesStr);
    return true;
}

static void setZoneFileName(char *str, const char *prefix, int zone, int nrOfZones) {
    sprintf(str, "%s.zone%iof%i", prefix, zone, nrOfZones);
}

int main(int argc, char **argv) {
    int ret = 0, minpw = 0, maxpw = 32, nrOfZones = 1, zone = 1;
    struct sigaction act1, act2;
    FILE *file = NULL, *wordlist = NULL;
    bool recovery = false, quiet = false,
            work_with_user = true, permutation = false;
    uint8_t *userpassword = NULL;
    char *charset = NULL, *inputfile = NULL, *wordlistfile = NULL;
    EncData *e;
    const unsigned int numCpuCores = (unsigned int) sysconf(_SC_NPROCESSORS_ONLN);
    unsigned int nrofthreads = numCpuCores / 2;

    printf("pdfcrack-ng %s - %s\n\n", VERSION, DESCRIPTION);

    /** Parse arguments */
    while (true) {
        int c, option_index;
        static struct option long_options[] = {
                {"bench",     no_argument,       0, 'b'},
                {"charset",   required_argument, 0, 'c'},
                {"file",      required_argument, 0, 'f'},
                {"loadState", required_argument, 0, 'l'},
                {"maxpw",     required_argument, 0, 'm'},
                {"minpw",     required_argument, 0, 'n'},
                {"threads",   required_argument, 0, 't'},
                {"owner",     no_argument,       0, 'o'},
                {"password",  required_argument, 0, 'p'},
                {"quiet",     required_argument, 0, 'q'},
                {"permutate", no_argument,       0, 's'},
                {"user",      no_argument,       0, 'u'},
                {"wordlist",  required_argument, 0, 'w'},
                {"version",   no_argument,       0, 'v'},
                {"zones",     required_argument, 0, 'z'},
                {0, 0,                           0, 0}};
        /* getopt_long stores the option index here. */
        option_index = 0;

        c = getopt_long(argc, argv, "bc:f:l:m:n:t:op:qsuw:vz:", long_options, &option_index);

        /* Detect the end of the options. */
        bool doBenchmark = false;

        if (c == -1)
            break;

        switch (c) {
            case 'b':
                doBenchmark = true; //nrofthreads
                break;
                //return 0;

            case 'c':
                if (charset)
                    fprintf(stderr, "Charset already set\n");
                else
                    charset = strdup(optarg);
                break;

            case 'f':
                if (!recovery)
                    inputfile = strdup(optarg);
                break;

            case 'l':
                if (inputfile)
                    free(inputfile);
                inputfile = strdup(optarg);
                recovery = true;
                break;

            case 'm':
                maxpw = atoi(optarg);
                break;

            case 'n':
                minpw = atoi(optarg);
                break;

            case 't':
                nrofthreads = (unsigned int) atoi(optarg);
                break;

            case 'o':
                work_with_user = false;
                break;

            case 'p':
                userpassword = (uint8_t *) strdup(optarg);
                work_with_user = false;
                break;

            case 'q':
                quiet = true;
                break;

            case 'u':
                if (!work_with_user) {
                    fprintf(stderr, "Cannot work with both user- and owner-password\n");
                    exit(1);
                }
                work_with_user = true;
                break;

            case 's':
                permutation = true;
                break;

            case 'w':
                if (wordlistfile)
                    fprintf(stderr, "Wordlist already set\n");
                else
                    wordlistfile = strdup(optarg);
                break;

            case 'v':
                printf("pdfcrack-ng %s\n", VERSION);
                return 0;

            case 'z':
                if (!getZoneInfo(optarg, &zone, &nrOfZones)) {
                    printf("Malformed zone info. Expected format 'zone/numZones'.\n");
                }
                if (nrOfZones < 1) {
                    printf("Number of zones must be > 0.\n");
                    exit(1);
                }
                if (zone < 1) {
                    printf("Zone must be > 0.\n");
                    exit(1);
                }
                if (zone > nrOfZones) {
                    printf("Zone must be smaller or equal to number of zones.\n");
                    exit(1);
                }
                break;

            default:
                printHelp(argv[0]);
                ret = 1;
                goto out3;
        }
        if (doBenchmark) {
            runBenchmark(nrofthreads, numCpuCores);
            return 0;
        }
    }

    /** If there are any unhandled arguments and the filename and/or wordlist is
        not set: try to match the first as the filename and the second as
        wordlist.
    */
    {
        int i = optind;
        if (i > 0) {
            if (i < argc && !inputfile)
                inputfile = strdup(argv[i++]);
            if (i < argc && !wordlistfile)
                wordlistfile = strdup(argv[i++]);
            if (i < argc)
                while (i < argc) {
                    fprintf(stderr, "Non-option argument %s\n", argv[i++]);
                }
        }
    }

    if (!inputfile || minpw < 0 || maxpw < 0) {
        printHelp(argv[0]);
        ret = 1;
        goto out3;
    }

    if (nrOfZones > 1 && wordlistfile) {
        printf("Zones cannot be used in combination with a wordlist. Split wordlist instead and run pdfcrack-ng with different wordlists on the different hosts.\n");
        ret = 1;
        goto out3;
    }

    if ((file = fopen(inputfile, "rb")) == 0) {
        fprintf(stderr, "Error: file %s not found\n", inputfile);
        ret = 2;
        goto out3;
    }

    e = calloc(1, sizeof(EncData));

    if (recovery) {
        if (wordlistfile) {
            free(wordlistfile);
            wordlistfile = NULL;
        }
        if (!loadState(file, e, &wordlistfile, &work_with_user)) {
            fprintf(stderr, "Error: Not a savefile or savefile is damaged\n");
            ret = 3;
            goto out1;
        }
        if (!quiet)
            printf(" [+] Loaded state from %s\n", inputfile);
    } else { /** !recovery */
        if (!openPDF(file, e)) {
            fprintf(stderr, "Error: Not a valid PDF\n");
            ret = 3;
            goto out1;
        }
        ret = getEncryptedInfo(file, e);
        if (ret) {
            if (ret == EENCNF)
                fprintf(stderr, "Error: Could not extract encryption information\n");
            else if (ret == ETRANF || ret == ETRENF || ret == ETRINF) {
                fprintf(stderr, "Error: Encryption not detected (is the document password protected?)\n");
                ret = 4;
                goto out1;
            }
        } else if (e->revision < 2 || (strcmp(e->s_handler, "Standard") != 0 || e->revision > 5)) {
            fprintf(stderr, "The specific version is not supported (%s - %d)\n", e->s_handler, e->revision);
            ret = 5;
            goto out1;
        }
    }

    if (fclose(file)) {
        fprintf(stderr, "Error: closing file %s\n", inputfile);
    }

    if (minpw > maxpw) {
        fprintf(stderr, "Warning: minimum pw-length bigger than max\n");
    }

    if (wordlistfile) {
        if ((wordlist = fopen(wordlistfile, "r")) == 0) {
            fprintf(stderr, "Error: file %s not found\n", wordlistfile);
            ret = 6;
            goto out2;
        }
    }

    if (nrOfZones > 1) {
        setZoneFileName(periodicStateFileName, SAVEFILE, zone, nrOfZones);
        setZoneFileName(crashStateFileName, CRASHFILE, zone, nrOfZones);
    } else {
        strcpy(periodicStateFileName, SAVEFILE);
        strcpy(crashStateFileName, CRASHFILE);
    }

    act2.sa_handler = autoSave;
    sigfillset(&act2.sa_mask);
    act2.sa_flags = 0;
    sigaction(SIGINT, &act2, 0);

    if (!quiet) {
        printEncData(e);
        act1.sa_handler = (sighandler_t) alarmInterrupt;
        sigemptyset(&act1.sa_mask);
        act1.sa_flags = 0;
        sigaction(SIGALRM, &act1, 0);
        alarm(PRINTERVAL);
    }

    /** Try to initialize the cracking-engine */
    if (!initPDFCrack(e, userpassword, work_with_user, wordlistfile,
                      wordlistfile ? Wordlist : Generative, wordlist, charset,
                      (unsigned int) minpw, (unsigned int) maxpw, permutation, nrofthreads, numCpuCores, zone,
                      nrOfZones)) {
        cleanPDFCrack();
        fprintf(stderr, "Wrong userpassword, '%s'\n", userpassword);
        ret = 7;
        goto out2;
    }

    /** Do the actual crunching */
    runCrack();


    /** cleanup */
    cleanPDFCrack();
    freeEncData(e);

    if (wordlistfile) {
        fclose(wordlist);
        free(wordlistfile);
    }
    free(inputfile);
    if (charset)
        free(charset);
    if (userpassword)
        free(userpassword);

    return 0;

    out1:
    if (fclose(file))
        fprintf(stderr, "Error: closing file %s\n", inputfile);

    out2:
    freeEncData(e);

    out3:
    if (inputfile)
        free(inputfile);
    if (charset)
        free(charset);
    if (userpassword)
        free(userpassword);

    exit(ret);
}
