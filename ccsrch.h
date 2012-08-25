/*
 * ccsrch (c) 2007 Mike Beekey  - zaphod2718@yahoo.com
 * All rights reserved
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#ifndef SIGHUP
  #define SIGHUP 1
#endif
#ifndef SIGQUIT
  #define SIGQUIT 3
#endif

#define PROG_VER "ccsrch 1.0.3 (c) 2007 zaphod2718@yahoo.com"

#define MDBUFSIZE 512
#define MAXPATH 2048
#define BSIZE 4096
#define CARDTYPELEN 64
#define CARDSIZE 17

extern int errno;

int ccsrch (char *str);
void check_mastercard_16(long offset);
void check_visa_16(long offset);
void check_discover_16(long offset);
void check_jcb_16(long offset);
void check_amex_15(long offset);
void check_enroute_15(long offset);
void check_jcb_15(long offset);
void check_diners_club_cb_14(long offset);
void check_visa_13(long offset);
void cleanup_shtuff();
int escape_space(char *infile, char *outfile);
int get_file_stat(char *infile, struct stat *fattr);
int luhn_check (int len, long offset);
int open_logfile ();
void print_num (int *buf, int len);
void print_result(char *str, int len, long offset);
int proc_dir_list (char *instr);
void process_cleanup();
int process_prefix(int len, long offset);
void signal_proc();
int track1_srch(int len);
int track2_srch(int len);
void usage (char *prog);

