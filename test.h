/*
 * =====================================================================
 *
 *       Filename:  test.h
 *
 *    Description:  Test suite for ccsrch
 *
 *        Version:  0.0.1
 *        Created:  26/08/12 22:24:56
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
#ifndef TEST_H
#define TEST_H

/* Prevent ccsrch.c from defining main */
#define TEST_MAIN

#include "mods.h"
#include "ccsrch.h"

/* Private global variable declarations */
#define MCARD_16 5
#define JCB_16 7
#define AMEX_15 2
#define ENROUTE_15 2
#define JCB_15 4
#define DINERS_14 16

/* Private method declarations */
int main(int argc, char *argv[]);

void sanity_checks(FILE *output);

void initialize_card_arrs();

void log_file_detection(FILE *output);

void image_file_detection(FILE *output);

void ignore_char_detection(FILE *output);

#endif
