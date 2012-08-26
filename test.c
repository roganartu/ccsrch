/*
 * =====================================================================
 *
 *       Filename:  test.c
 *
 *    Description:  Test file for ccsrch
 *
 *        Version:  0.0.1
 *        Created:  26/08/12 22:23:48
 *
 *         Author:  Tony Lee (Roganartu), uni@roganartu.com
 *   Contributors:  Ashley Kurkowski <ashley.kurkowski@gmail.com>
 *                  Jonathan Daniels <jpsd1987@gmail.com>
 *                  Justin Vu <justin.vu90@gmail.com>
 *                  Nils Albjerk <nils.albjerk@gmail.com>
 *                  Dan Benveniste <dan.benveniste@gmail.com>
 *   Organisation:  University of Queensland - COMS3000 Semester 2 2012
 *
 * =====================================================================
 */
#include "test.h"

/* Local globals */
int mastercard_16[MCARD_16][CARDSIZE];
int visa_16[CARDSIZE];
int discover_16[CARDSIZE];
int jcb_16[JCB_16][CARDSIZE];
int amex_15[AMEX_15][CARDSIZE];
int enroute_15[ENROUTE_15][CARDSIZE];
int jcb_15[JCB_15][CARDSIZE];
int diners_club_cb_14[DINERS_14][CARDSIZE];

int main(int argc, char *argv[]) {
    FILE *output;

    /* TODO fix this so that we can accept a log file */
    output = stdout;

    /* Prepare environment */
    initialize_buffer();
    initialize_card_arrs();

    fprintf(output, "Commencing testing of: %s\n\n", PROG_VER);

    fprintf(output, "%s\n\n", ". indicates success\nF indicates failure");

    /* Sanity checks */
    fprintf(output, "%s\n", "==== Sanity Checks ==========================");
    sanity_checks(output);
}

void initialize_card_arrs() {
    int i, j;

    /* Initialise them all to zero */
    for ( i = 0; i < CARDSIZE; i++ ) {
        for ( j = 0; j < MCARD_16; j++ ) {
            mastercard_16[j][i] = 0;
        }

        visa_16[i] = 0;

        discover_16[i] = 0;

        for ( j = 0; j < JCB_16; j++ ) {
            jcb_16[j][i] = 0;
        }

        for ( j = 0; j < AMEX_15; j++ ) {
            amex_15[j][i] = 0;
        }

        for ( j = 0; j < ENROUTE_15; j++ ) {
            enroute_15[j][i] = 0;
        }

        for ( j = 0; j < JCB_15; j++ ) {
            jcb_15[j][i] = 0;
        }

        for ( j = 0; j < DINERS_14; j++ ) {
            diners_club_cb_14[j][i] = 0;
        }
    }

    /* mastercard 16 digit */
    /* 51, 52, 53, 54, 55 */
    for ( i = 0; i < MCARD_16; i++ ) {
        mastercard_16[i][0] = 5;
        mastercard_16[i][1] = 1 + i;
    }

    /* Visa 16 digit */
    /* 4 */
    visa_16[0] = 4;

    /* Discover 16 digit */
    /* 6011 */
    discover_16[0] = 6;
    discover_16[1] = 0;
    discover_16[2] = 1;
    discover_16[3] = 1;

    /* JCB 16 digit */
    /* 3088, 3096, 3112, 3158, 3337, 3528, 3529 */
    for ( i = 0; i < JCB_16; i++ ) {
        jcb_16[i][0] = 3;
    }
    /* 3088 */
    jcb_16[0][2] = 8;
    jcb_16[0][3] = 8;
    /* 3096 */
    jcb_16[1][2] = 9;
    jcb_16[1][3] = 6;
    /* 3112 */
    jcb_16[2][1] = 1;
    jcb_16[2][2] = 1;
    jcb_16[2][3] = 2;
    /* 3158 */
    jcb_16[3][1] = 1;
    jcb_16[3][2] = 5;
    jcb_16[3][3] = 8;
    /* 3337 */
    jcb_16[4][1] = 3;
    jcb_16[4][2] = 3;
    jcb_16[4][3] = 7;
    /* 3528 */
    jcb_16[5][1] = 5;
    jcb_16[5][2] = 2;
    jcb_16[5][3] = 8;
    /* 3529 */
    jcb_16[6][1] = 5;
    jcb_16[6][2] = 2;
    jcb_16[6][3] = 9;

    /* AMEX 15 digit */
    /* 34, 37 */
    for ( i = 0; i < AMEX_15; i++ ) {
        amex_15[i][0] = 3;
        amex_15[i][1] = 1 + 3 * (i + 1);
    }

    /* Enroute 15 digit */
    /* 2014, 2149 */
    for ( i = 0; i < ENROUTE_15; i++ ) {
        enroute_15[i][0] = 2;
    }
    /* 2014 */
    enroute_15[0][2] = 1;
    enroute_15[0][3] = 4;
    /* 2149 */
    enroute_15[1][1] = 1;
    enroute_15[1][2] = 4;
    enroute_15[1][3] = 9;

    /* JCB 15 digit */
    /* 1800, 2131, 3528, 3529 */
    for ( i = 0; i < JCB_15; i++ ) {
        jcb_15[i][0] = 3;
    }
    /* 1800 */
    jcb_15[0][0] = 1;
    jcb_15[0][1] = 8;
    /* 2131 */
    jcb_15[1][0] = 2;
    jcb_15[1][1] = 1;
    jcb_15[1][2] = 3;
    jcb_15[1][3] = 1;
    /* 3528 */
    jcb_15[2][1] = 5;
    jcb_15[2][2] = 2;
    jcb_15[2][3] = 8;
    /* 3529 */
    jcb_15[3][1] = 5;
    jcb_15[3][2] = 2;
    jcb_15[3][3] = 9;

    /* Diners 14 digit */
    /* 36, 300, 301, 302, 303, 304, 305, 380, 381, 382, 383, 384, 385, 386, 387, 388 */
    for ( i = 0; i < DINERS_14; i++ ) {
        diners_club_cb_14[i][0] = 3;
        switch (i) {
            case 0:
                diners_club_cb_14[i][1] = 6;
                break;
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
                diners_club_cb_14[i][1] = 0;
                diners_club_cb_14[i][2] = i - 1;
                break;
            default:
                diners_club_cb_14[i][1] = 8;
                diners_club_cb_14[i][2] = i - 7;
        }
    }

}

void sanity_checks(FILE *output) {
    long int offset = 0;
    char result;
    int i;

    /* Mastercard 16 digit */
    fprintf(output, "%s", "Mastercard 16: ");
    for ( i = 0; i < MCARD_16; i++ ) {
        memcpy(cardbuf, mastercard_16[i], CARDSIZE);
        result = (check_mastercard_16(offset)) ? '.' : 'F';
        fprintf(output, "%c", result);
    }
    fprintf(output, "%s", "\n");

    /* Visa 16 digit */
    fprintf(output, "%s", "Visa 16: ");
    memcpy(cardbuf, visa_16, CARDSIZE);
    result = (check_visa_16(offset)) ? '.' : 'F';
    fprintf(output, "%c", result);
    fprintf(output, "%s", "\n");

    /* Discover 16 digit */
    fprintf(output, "%s", "Discover 16: ");
    memcpy(cardbuf, discover_16, CARDSIZE);
    result = (check_discover_16(offset)) ? '.' : 'F';
    fprintf(output, "%c", result);
    fprintf(output, "%s", "\n");

    /* JCB 16 digit */
    fprintf(output, "%s", "JCB 16: ");
    for ( i = 0; i < JCB_16; i++ ) {
        memcpy(cardbuf, jcb_16[i], CARDSIZE);
        result = (check_jcb_16(offset)) ? '.' : 'F';
        fprintf(output, "%c", result);
    }
    fprintf(output, "%s", "\n");

    /* AMEX 15 digit */
    fprintf(output, "%s", "AMEX 15: ");
    for ( i = 0; i < AMEX_15; i++ ) {
        memcpy(cardbuf, amex_15[i], CARDSIZE);
        result = (check_amex_15(offset)) ? '.' : 'F';
        fprintf(output, "%c", result);
    }
    fprintf(output, "%s", "\n");

    /* Enroute 15 digit */
    fprintf(output, "%s", "Enroute 15: ");
    for ( i = 0; i < ENROUTE_15; i++ ) {
        memcpy(cardbuf, enroute_15[i], CARDSIZE);
        result = (check_enroute_15(offset)) ? '.' : 'F';
        fprintf(output, "%c", result);
    }
    fprintf(output, "%s", "\n");

    /* JCB 15 digit */
    fprintf(output, "%s", "JCB 15: ");
    for ( i = 0; i < JCB_15; i++ ) {
        memcpy(cardbuf, jcb_15[i], CARDSIZE);
        result = (check_jcb_15(offset)) ? '.' : 'F';
        fprintf(output, "%c", result);
    }
    fprintf(output, "%s", "\n");

    /* Diners 14 digit */
    fprintf(output, "%s", "Diners 14: ");
    for ( i = 0; i < DINERS_14; i++ ) {
        memcpy(cardbuf, diners_club_cb_14[i], CARDSIZE);
        result = (check_diners_club_cb_14(offset)) ? '.' : 'F';
        fprintf(output, "%c", result);
    }
    fprintf(output, "%s", "\n");
}
