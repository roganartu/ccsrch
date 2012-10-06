/*
 * =============================================================================
 *
 *       Filename:  mods.h
 *
 *    Description:  ccsrch modifications as per COMS3000 Assignment 2
 *                  All global vars from ccsrch source can be accessed in here.
 *
 *        Version:  0.0.2
 *        Created:  25/08/12 14:10:54
 *
 *         Author:  Tony Lee (Roganartu) <uni@roganartu.com>
 *   Contributors:  Ashley Kurkowski <ashley.kurkowski@gmail.com>
 *                  Jonathan Daniels <jpsd1987@gmail.com>
 *                  Justin Vu <justin.vu90@gmail.com>
 *                  Nils Albjerk <nils.albjerk@gmail.com>
 *                  Dan Benveniste <dan.benveniste@gmail.com>
 *   Organisation:  University of Queensland - COMS3000 Semester 2 2012
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59
 * Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * =============================================================================
 */
#ifndef MODS_H
#define MODS_H

#include "ccsrch.h"
#include <unistd.h>
#include <fcntl.h>

/* Global variable declarations */
typedef int bool;
#define false 0
#define true 1

#ifndef MAXPATH
#define MAXPATH 2048
#endif

struct stat log_file_stat;

char pwd[2048];

int skipped_executable_count;
int extracted_archive_count;

int skipchar_count;
char *skipchars;

int nextrnd;

char extracted_parent[MAXPATH];

typedef enum FILE_TYPES {
    ASCII,
    EXECUTABLE,
    IMAGE,
    VIDEO,
    BINARY,
    MS_WORD,
    MS_EXCEL,
    MS_WORDX,
    MS_EXCELX,
    PDF,
    XML,
    ZIP,
    GZIP,
    TAR,
    ODT,
    OTT,
    ODS,
    OTS,
    SELF_LOG,
    UNKNOWN
} file_type;

/* Method declarations */
bool initialise_mods();

pid_t pipe_and_fork(int *fd, bool reverse);

file_type detect_file_type(char *filename);

void reset_skip_chars(void);

bool in_skipped_arr(char check);

int unzip_and_parse(char *filename);

void gen_rand_string(char *output, int len);

void remove_directory(char *dir);

static char *last_strstr(const char *haystack, const char *needle);

#endif
