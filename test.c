/*
 * =====================================================================
 *
 *       Filename:  test.c
 *
 *    Description:  Test file for ccsrch
 *
 *        Version:  0.0.2
 *        Created:  26/08/12 22:23:48
 *
 *         Author:  Tony Lee (Roganartu), uni@roganartu.com
 *   Contributors:  Ashley Kurkowski <ashley.kurkowski@gmail.com>
 *                  Jonathan Daniels <jpsd1987@gmail.com>
 *                  Justin Vu <justin.vu90@gmail.com>
 *                  Nils Albjerk <nils.albjerk@gmail.com>
 *                  Dan Benveniste <dan.benveniste@gmail.com>
 *                  Michael Holzheimer <michael@mholzheimer.com>
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
    fprintf(output, "%s", "\n");

    /* Log file detection */
    fprintf(output, "%s\n", "==== Log File Tests =========================");
    run_test(output, SELF_LOG);
    fprintf(output, "%s", "\n\n");

    /* Image file detection */
    fprintf(output, "%s\n", "==== Image File Tests =======================");
    run_test(output, IMAGE);
    fprintf(output, "%s", "\n\n");

    /* Binary file detection */
    fprintf(output, "%s\n", "==== Binary File Tests ======================");
    run_test(output, BINARY);
    fprintf(output, "%s", "\n\n");

    /* Image file detection */
    fprintf(output, "%s\n", "==== Default Separator Tests ================");
    run_test(output, SKIPCHARS);
    fprintf(output, "%s", "\n\n");

    /* Compressed files */
    fprintf(output, "%s\n", "==== Compressed files =======================");
    run_test(output, ZIP);
    fprintf(output, "%s", "\n\n");

    /* PDF files */
    fprintf(output, "%s\n", "==== PDF files ==============================");
    run_test(output, PDF);
    fprintf(output, "%s", "\n\n");

    /* .xlsx files */
    fprintf(output, "%s\n", "==== .xlsx files ============================");
    run_test(output, MS_EXCELX);
    fprintf(output, "%s", "\n\n");

    /* .docx files */
    fprintf(output, "%s\n", "==== .docx files ============================");
    run_test(output, MS_WORDX);
    fprintf(output, "%s", "\n\n");

    /* ODS files */
    fprintf(output, "%s\n", "==== ODS files ==============================");
    run_test(output, ODS);
    fprintf(output, "%s", "\n\n");

    /* ODT files */
    fprintf(output, "%s\n", "==== ODT files ==============================");
    run_test(output, ODT);
    return 0;
}

/*
 * ===  FUNCTION  ==============================================================
 *         Name:  initialize_card_arrs
 *
 *  Description:  Initialise local global variables with dummy card values
 *                for each card type for memcpying into cardbuf for sanity
 *                checking.
 *
 *      Version:  0.0.1
 *       Params:  N/A
 *      Returns:  void
 *        Usage:  initialize_card_arrs()
 *      Outputs:  N/A
 * =============================================================================
 */
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

/*
 * ===  FUNCTION  ==============================================================
 *         Name:  sanity_checks
 *
 *  Description:  Check to ensure that all methods are functioning as expected
 *                and detect all the different card formats
 *
 *      Version:  0.0.1
 *       Params:  N/A
 *      Returns:  void
 *        Usage:  sanity_checks()
 *      Outputs:  Sanity check test results
 * =============================================================================
 */
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

/*
 * ===  FUNCTION  ==============================================================
 *         Name:  run_test
 *
 *  Description:  Flexible method for running tests on the ccsrch suite
 *
 *                LOG FILE TESTS - Format: SELF_LOG
 *                    Verifies that the modified ccsrch successfully detects
 *                    its own log file regardless of path
 *                    Directory scanned: ./tests
 *                    Log file outputs:
 *                        ./tests/log.log
 *                        tests/../tests/log.log
 *                        tests/log.log
 *                        log.log
 *
 *                    Tests absolute paths, relative with redundancy, relative,
 *                    symlinks and no log file output
 *                
 *                IMAGE FILE TESTS - Format: IMAGE
 *                    Verifies that the modified ccsrch successfully detects
 *                    and skips image files
 *                    Files scanned: tests/img.jpg
 *                                   tests/img.png
 *                                   tests/img.gif
 *                                   tests/img.not_an_image_extension
 *                                   tests/not_an_image.jpg
 *
 *                SEPARATOR TESTS - Format: SKIPCHARS
 *                    Verifies that the modified ccsrch successfully detects and
 *                    ignores the default ignore characters
 *                    Tests, in order:
 *                        space
 *                        \n
 *                        \r
 *                        -
 *                        combination of above
 *
 *                    There should be 3 credit card matches in the given file
 *                    out of 6 potential PANs
 *
 *                COMPRESSION TESTS - Format: ZIP
 *                    Verifies that the modified ccsrch successfully detects,
 *                    unpacks and parses compressed files.
 *                    Tests, in order:
 *                        tar.gzip
 *                        mac os zip file
 *                        zip file
 *                        zip file containing zip files
 *
 *                PDF TESTS - Format: PDF
 *                    Verifies that the modified ccsrch successfully detects,
 *                    unpacks and parses PDF Documents.
 *
 *                .xlsx TESTS - Format: MS_EXCELX
 *                    Verifies that the modified ccsrch successfully detects,
 *                    unpacks and parses xlsx documents.
 *
 *                .docx TESTS - Format: MS_WORDX
 *                    Verifies that the modified ccsrch successfully detects,
 *                    unpacks and parses docx documents.
 *
 *                ODS TESTS - Format: ODS
 *                    Verifies that the modified ccsrch successfully detects,
 *                    unpacks and parses ODS and OTS Open Document Spreadsheet
 *                    formatted documents.
 *                    Tests, in order:
 *                        ods.ods
 *                        ots.ots
 *
 *                ODT TESTS - Format: ODT
 *                    Verifies that the modified ccsrch successfully detects,
 *                    unpacks and parses ODT and OTT Open Document formatted
 *                    documents.
 *                    Tests, in order:
 *                        odt.odt
 *                        ott.ott
 *
 *                BINARY TESTS - FormatL BINARY
 *                    Verifies that AUDIO, VIDEO, EXECUTABLE and BINARY types
 *                    are skipped appropriately.
 *                    Tests, in order:
 *                        ogv.ogv
 *                        mp3.mp3
 *                        ccsrch.o
 *                        ccsrch
 *
 *      Version:  0.0.1
 *       Params:  FILE *output
 *                file_type type
 *      Returns:  void
 *        Usage:  run_test( FILE *output, file_type type )
 *      Outputs:  Test results for given format
 * =============================================================================
 */
void run_test(FILE *output, file_type type) {
    int num_tests, loop_count;
    char buffer[80], **test_files, **expected_results;
    int i, pipe;
    pid_t pid;
    FILE *in_out;
    bool found;

    // Test counts
    switch (type) {
        case SELF_LOG:
        case ZIP:
        case BINARY:
            num_tests = 4;
            break;
        case IMAGE:
        case SKIPCHARS:
            num_tests = 5;
            break;
        case PDF:
        case MS_EXCELX:
        case MS_WORDX:
            num_tests = 1;
            break;
        case ODS:
        case ODT:
            num_tests = 2;
            break;
    }

    // Initialise as empty strings. 2D memset
    test_files = malloc(sizeof(char*) * num_tests);
    expected_results = malloc(sizeof(char*) * num_tests);
    for ( i = 0; i < num_tests; i++ ) {
        test_files[i] = malloc(MAXPATH);
        expected_results[i] = malloc(MAXPATH);
        memset(test_files[i], '\0', MAXPATH);
        memset(expected_results[i], '\0', MAXPATH);
    }

    // Files and expected outputs
    switch (type) {
        case SELF_LOG:
            strncpy(test_files[0], "./tests/log.log", MAXPATH); /* Absolute path (as best we can) */
            strncpy(test_files[1], "tests/../tests/log.log", MAXPATH); /* Weird path */
            strncpy(test_files[2], "tests/log.log", MAXPATH); /* Relative path */
            strncpy(test_files[3], "tests/log_symlink", MAXPATH); /* Symlink test */
            for ( i = 0; i < num_tests; i++ )
                strncpy(expected_results[i], "Skipping log file: ", MAXPATH);
            break;
        case IMAGE:
            strncpy(test_files[0], "tests/img.jpg", MAXPATH);
            strncpy(test_files[1], "tests/img.png", MAXPATH);
            strncpy(test_files[2], "tests/img.gif", MAXPATH);
            strncpy(test_files[3], "tests/img.not_an_image_extension", MAXPATH);
            strncpy(test_files[4], "tests/not_an_image.jpg", MAXPATH);
            for ( i = 0; i < num_tests - 1; i++ )
                strncpy(expected_results[i], "Binary types skipped ->\t\t1",
                        MAXPATH);
            strncpy(expected_results[4], "Binary types skipped ->\t\t0",
                    MAXPATH);
            break;
        case BINARY:
            strncpy(test_files[0], "tests/ogv.ogv", MAXPATH);
            strncpy(test_files[1], "tests/mp3.mp3", MAXPATH);
            strncpy(test_files[2], "tests/ccsrch.o", MAXPATH);
            strncpy(test_files[3], "tests/ccsrch", MAXPATH);
            for ( i = 0; i < num_tests - 1; i++ )
                strncpy(expected_results[i], "Binary types skipped ->\t\t1",
                        MAXPATH);
            break;
        case SKIPCHARS:
            strncpy(test_files[0], "tests/ignore_space.txt", MAXPATH);
            strncpy(test_files[1], "tests/ignore_unix_newline.txt", MAXPATH);
            strncpy(test_files[2], "tests/ignore_windows_newline.txt", MAXPATH);
            strncpy(test_files[3], "tests/ignore_dash.txt", MAXPATH);
            strncpy(test_files[4], "tests/ignore_combination.txt", MAXPATH);
            for ( i = 0; i < num_tests; i++ )
                strncpy(expected_results[i], "Credit card matches ->\t\t3",
                        MAXPATH);
            break;
        case ZIP:
            strncpy(test_files[0], "tests/tartest.tar.gz", MAXPATH);
            strncpy(test_files[1], "tests/test.mac.zip", MAXPATH);
            strncpy(test_files[2], "tests/test.zip", MAXPATH);
            strncpy(test_files[3], "tests/test2.zip", MAXPATH);
            strncpy(expected_results[0], "Credit card matches ->\t\t9", MAXPATH);
            strncpy(expected_results[1], "Credit card matches ->\t\t30", MAXPATH);
            strncpy(expected_results[2], "Credit card matches ->\t\t30", MAXPATH);
            strncpy(expected_results[3], "Credit card matches ->\t\t15", MAXPATH);
            break;
        case PDF:
            strncpy(test_files[0], "tests/pdf.pdf", MAXPATH);
            strncpy(expected_results[0], "Credit card matches ->\t\t15", MAXPATH);
            break;
        case MS_EXCELX:
            strncpy(test_files[0], "tests/xlsx.xlsx", MAXPATH);
            strncpy(expected_results[0], "Credit card matches ->\t\t3", MAXPATH);
            break;
        case MS_WORDX:
            strncpy(test_files[0], "tests/docx.docx", MAXPATH);
            strncpy(expected_results[0], "Credit card matches ->\t\t3", MAXPATH);
            break;
        case ODS:
            strncpy(test_files[0], "tests/ods.ods", MAXPATH);
            strncpy(test_files[1], "tests/ots.ots", MAXPATH);
            for ( i = 0; i < num_tests; i++ )
                strncpy(expected_results[i], "Credit card matches ->\t\t3",
                        MAXPATH);
            break;
        case ODT:
            strncpy(test_files[0], "tests/odt.odt", MAXPATH);
            strncpy(test_files[1], "tests/ott.ott", MAXPATH);
            for ( i = 0; i < num_tests; i++ )
                strncpy(expected_results[i], "Credit card matches ->\t\t3",
                        MAXPATH);
            break;
    }

    // Loop counts
    switch (type) {
        case SELF_LOG:
            loop_count = num_tests + 1;
            break;
        default:
            loop_count = num_tests;
    }

    for ( i = 0; i < loop_count; i++ ) {
        pid = pipe_and_fork(&pipe, true);
        if (pid == (pid_t) 0) {
            /* Child */
            dup2(pipe, STDOUT_FILENO);
            dup2(pipe, STDERR_FILENO);

            // Particulars of the invocation of ccsrch
            switch (type) {
                case SELF_LOG:
                    if (i < num_tests)
                        execl("./ccsrch", "ccsrch", "-o", test_files[i], "tests",
                                NULL);
                    else
                        execl("./ccsrch", "ccsrch", "tests", NULL);
                    break;
                default:
                    execl("./ccsrch", "ccsrch", test_files[i], NULL);
            }

        } else if (pid > (pid_t) 0) {
            /* Parent */
            in_out = fdopen(pipe, "r");
            found = false;
            while (!found && in_out != NULL && !feof(in_out)) {
                fgets(buffer, 80, in_out);

                // Result checking
                switch (type) {
                    case SELF_LOG:
                        if (strstr(buffer, expected_results[0]) != NULL)
                            found = true;
                        break;
                    default:
                        if (strstr(buffer, expected_results[i]) != NULL)
                            found = true;
                }
            }

            // Result printing
            switch (type) {
                case SELF_LOG:
                    if ((i < num_tests && found) || (i == num_tests && !found))
                        fprintf(output, "%s", ".");
                    else
                        fprintf(output, "%s", "F");
                    break;
                default:
                    if (found)
                        fprintf(output, "%s", ".");
                    else
                        fprintf(output, "%s", "F");
            }
            fflush(output);

            wait(NULL);
            close(pipe);
        } else {
            /* Fork failed */
            fprintf(stderr, "\n%s\n", "Failed to pipe and fork");
        }
    }

    // Clean up
    for ( i = 0; i < num_tests; i++ ) {
        free(test_files[i]);
        free(expected_results[i]);
    }
    free(test_files);
    free(expected_results);
}
