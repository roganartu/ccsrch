/*
 * ccsrch (c) 2007 Mike Beekey  - zaphod2718@yahoo.com All rights reserved
 *
 * Modifications (c) 2012 COMS3000 Assignment 2 Group Semester 2 <uni@roganartu.com>
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
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include "ccsrch.h"

char           *logfilename = NULL;
char           *currfilename = NULL;
FILE           *logfilefd = NULL;
long            total_count = 0;
long		currfile_atime=0;
long		currfile_mtime=0;
long		currfile_ctime=0;
int             init_time = 0;
int 		print_byte_offset=0;
int 		print_epoch_time=0;
int 		print_julian_time=0;
int 		print_filename_only=0;
char            ccsrch_buf[BSIZE];
int    		ccsrch_index = 0;
int		tracksrch=0;
int		tracktype1=0;
int		tracktype2=0;
int		trackdatacount=0;
char		lastfilename[MAXPATH];
int		filename_pan_count=0;

void
initialize_buffer()
{
  int	i=0;
  for (i=0; i<CARDSIZE; i++)
    cardbuf[i]=0;
}

void 
print_result(char *cardname, int cardlen, long byte_offset)
{
#ifndef TEST_MAIN
  int             i = 0;
  char            nbuf[20];
  char            buf[MAXPATH];
  char		  basebuf[MDBUFSIZE];
  char		  bytebuf[MDBUFSIZE];
  char		  datebuf[MDBUFSIZE];
  char		  mdatebuf[CARDTYPELEN];
  char		  adatebuf[CARDTYPELEN];
  char		  cdatebuf[CARDTYPELEN];
  char		  trackbuf[MDBUFSIZE];
  char            print_filename[MAXPATH];

  memset(&nbuf, '\0', 20);

  for (i = 0; i < cardlen; i++)
    nbuf[i] = cardbuf[i]+48;

  memset(&buf,'\0',MAXPATH);
  memset(&basebuf,'\0',MDBUFSIZE);

  // Show the filename of the archive if this file was extracted from somewhere
  if (extracted_parent[0] != 0) {
    strncpy(print_filename, extracted_parent, MAXPATH);
    strncat(print_filename, " -> ", 5);
    strncat(print_filename, index(currfilename, '/'),
            MAXPATH - strlen(extracted_parent));
  } else
      strncpy(print_filename, currfilename, MAXPATH);

  /* MB we need to figure out how to update the count and spit out the final filename with the count.  ensure that it gets flushed out on the last match if you are doing a diff between previous filename and new filename */

  if (print_filename_only)
    snprintf(basebuf, MDBUFSIZE, "%s", print_filename);
  else
    snprintf(basebuf, MDBUFSIZE, "%s\t%s\t%s", print_filename, cardname, nbuf);

  strncat(buf,basebuf,MAXPATH);

    if (print_byte_offset)
    {
      memset(&bytebuf,'\0',MDBUFSIZE);
      snprintf(bytebuf, MDBUFSIZE, "\t%ld", byte_offset);
      strncat(buf,bytebuf,MAXPATH);
    }

    if (print_julian_time)
    {
      memset(&mdatebuf,'\0',CARDTYPELEN);
      strncpy(mdatebuf,ctime((time_t *)&currfile_mtime),CARDTYPELEN);
      mdatebuf[strlen(mdatebuf)-1]='\0';

    memset(&adatebuf,'\0',CARDTYPELEN);
    strncpy(adatebuf,ctime((time_t *)&currfile_atime),CARDTYPELEN);
    adatebuf[strlen(adatebuf)-1]='\0';

    memset(&cdatebuf,'\0',CARDTYPELEN);
    strncpy(cdatebuf,ctime((time_t *)&currfile_ctime),CARDTYPELEN);
    cdatebuf[strlen(cdatebuf)-1]='\0';

      memset(&datebuf,'\0',MDBUFSIZE);
      snprintf(datebuf, MDBUFSIZE, "\t%s\t%s\t%s", mdatebuf,adatebuf,cdatebuf);
      strncat(buf,datebuf,MAXPATH);
    }

    if (print_epoch_time)
    {
      memset(&datebuf,'\0',MDBUFSIZE);
      snprintf(datebuf, MDBUFSIZE, "\t%ld\t%ld\t%ld", currfile_mtime,currfile_atime,currfile_ctime);
      strncat(buf,datebuf,MAXPATH);
    }

    if (tracksrch)
    {
      memset(&trackbuf,'\0',MDBUFSIZE);
      if (tracktype1)
      {
        if (track1_srch(cardlen))
        {
          snprintf(trackbuf, MDBUFSIZE, "\tTRACK_1");
        }
      }
      if (tracktype2)
      {
        if (track2_srch(cardlen))
        {
          snprintf(trackbuf, MDBUFSIZE, "\tTRACK_2");
        }
      }
      strncat(buf,trackbuf,MAXPATH);
    }
  if (logfilefd != NULL)
    fprintf(logfilefd, "%s\n", buf);
  else
    fprintf(stdout, "%s\n", buf);

  total_count++;

  // Abide by shortcut option
  if (opt_shortcut)
    shortcut_breakout = 1;
#endif
}

int track1_srch(int cardlen)
{
  /* [%:B:cardnum:^:name (first initial cap?, let's ignore the %)] */
  if ((ccsrch_buf[ccsrch_index+1] == '^')
    && (ccsrch_buf[ccsrch_index-cardlen] == 66)
    && (ccsrch_buf[ccsrch_index+2] > 64)
    && (ccsrch_buf[ccsrch_index+2] < 91))
  {
    trackdatacount++;
    return(1);
  }
  else
    return(0);
}

int track2_srch(int cardlen)
{
  /* [;:cardnum:=:expir date(YYMM), we'll use the ; here] */
  if (((ccsrch_buf[ccsrch_index+1] == '=') || (ccsrch_buf[ccsrch_index+1] == 68))
    && ((ccsrch_buf[ccsrch_index-cardlen] == ';')||
    ((ccsrch_buf[ccsrch_index-cardlen] > 57) || (ccsrch_buf[ccsrch_index-cardlen] < 91)) )
    && ((ccsrch_buf[ccsrch_index+2] > 47)
    && (ccsrch_buf[ccsrch_index+2] < 58))
    && ((ccsrch_buf[ccsrch_index+3] > 47)
    && (ccsrch_buf[ccsrch_index+3] < 58)))
  {
    trackdatacount++;
    return(1);
  }
  else
    return(0);
}


int 
process_prefix(int len, long offset)
{
  switch (len)
  {
    case 16:
    check_mastercard_16(offset);
    check_visa_16(offset);
    check_discover_16(offset);
    check_jcb_16(offset);
    break;
  case 15:
    check_amex_15(offset);
    check_enroute_15(offset);
    check_jcb_15(offset);
    break;
  case 14:
    check_diners_club_cb_14(offset);
    break;
/** It has been purported that these cards are no longer in use
  case 13:
    check_visa_13(offset);
    break;
**/
  }
  return (0);
}


int 
luhn_check(int len, long offset)
{
  int    i = 0;
  int    tmp = 0;
  int    total = 0;
  int    nummod = 0;
  int            num[CARDSIZE];


  if (cardbuf[i]<=0)
    return(0);

  for (i=0; i<CARDSIZE; i++)
    num[i]=0;

  for (i=0; i<len; i++)
   num[i]=cardbuf[i];

#ifdef DEBUG
  for (i=0; i<len; i++)
    printf("%d ",num[i]);
  printf("\n");
#endif

  for (i=len-2; i>=0; i-=2)
  {
    tmp=2*num[i];
    num[i]=tmp;
    if (num[i]>9)
      num[i]-=9;
  }

  for (i = 0; i < len; i++)
    total += num[i];


  nummod = total % 10;
  if (nummod == 0)
  {
#ifdef DEBUG
  printf("Luhn Check passed ***********************************\n");
#endif
    process_prefix(len, offset);
  }
  return (nummod);
}


int 
ccsrch(char *filename)
{
  FILE           *in = NULL;
  int             infd = 0;
  int             cnt = 0;
  long		  byte_offset=1;
  int    k = 0;
  int    counter = 0;
  int    total = 0;
  int    check = 0;

  memset(&lastfilename,'\0',MAXPATH);
  ccsrch_index=0;
  errno = 0;
  in = fopen(filename, "rb");
  if (in == NULL)
  {
    if (errno==13)
      fprintf(stderr, "ccsrch: Unable to open file %s for reading; Permission Denied\n", filename);
    else
      fprintf(stderr, "ccsrch: Unable to open file %s for reading; errno=%d\n", filename, errno);
    return (-1);
  }
  infd = fileno(in);
  currfilename = filename;
  byte_offset=1;

  // Ensure that char skip array is reset to normal for each parse. Onus is on
  // file type handling to set it up every time they need to
  reset_skip_chars();

  switch (detect_file_type(filename)) {
    case ASCII:
    case UNKNOWN:
      break;
    case EXECUTABLE:
    case BINARY:
    case IMAGE:
    case VIDEO:
      skipped_executable_count++;
      return 1;
    case TAR:
      return untar_and_parse(filename);
    case ZIP:
      return unzip_and_parse(filename);
    case GZIP:
      return gunzip_and_parse(filename);
    case MS_WORD:
      // Do something specific with word
      break;
    case MS_EXCEL:
      // Do something specific with excel
      break;
    case MS_WORDX:
      // Do something specific with new word docs
      break;
    case MS_EXCELX:
      // Do something specific with new excel docs
      break;
    case PDF:
      return convert_and_parse_pdf(filename);
    case XML:
      // Do something specific with XML files. Likely includes skipping opening
      // and closing tags.
      break;
    case ODT:
    case OTT:
      // OpenDocument Text document - normal and template. Same for both
      break;
    case ODS:
    case OTS:
      // OpenDocument Spreadsheet document - normal and template. Same for both
      break;
    case SELF_LOG:
      // TODO Print that we're skipping the log.
      return 1;
  }

  file_count++;

  initialize_buffer();

  while (1)
  {
    memset(&ccsrch_buf, '\0', BSIZE);
    cnt = read(infd, &ccsrch_buf, BSIZE - 1);
    if (cnt <= 0)
      break;
    ccsrch_index = 0;

    while (ccsrch_index < cnt)
    {
      if ((ccsrch_buf[ccsrch_index] > 47) && (ccsrch_buf[ccsrch_index] < 58))
      {
        check = 1;
        cardbuf[counter] = ((int)ccsrch_buf[ccsrch_index])-48;
        counter++;
      } else if (ccsrch_buf[ccsrch_index] == 0 || in_skipped_arr(ccsrch_buf[ccsrch_index]))
      {
        /*
         * we consider anything within the skipchars array to be noise, and
         * hence ignored. See ccsrch mods method in_skipped_arr for details
         * otherwise, restart the count
         */
        check = 0;
      } else
      {
        check = 0;
        initialize_buffer();
        counter=0;
      }

      if (((counter > 12) && (counter < CARDSIZE)) && (check)) 
      {
        switch (counter)
        {
        case 16:
          luhn_check(16,byte_offset-16);
          break;
        case 15:
          luhn_check(15,byte_offset-15);
          break;
        case 14:
          luhn_check(14,byte_offset-14);
          break;
        case 13:
          luhn_check(13,byte_offset-13);
          break;
        }
      } else if ((counter == CARDSIZE) && (check)) 
      {
        for (k = 0; k < counter - 1; k++)
        {
          cardbuf[k] = cardbuf[k + 1];
        }
        cardbuf[k] = (-1);
        luhn_check(13,byte_offset-13);
        luhn_check(14,byte_offset-14);
        luhn_check(15,byte_offset-15);
        luhn_check(16,byte_offset-16);
        counter--;
      }
      byte_offset++;
      ccsrch_index++;

      // Abide by shortcut option
      if (shortcut_breakout) {
        shortcut_breakout = 0;
        fclose(in);
        return total;
      }
    }
  }

  fclose(in);

  return (total);
}


int 
escape_space(char *infile, char *outfile)
{
  int    i = 0;
  int    spc = 0;
  char           *tmpbuf = NULL;
  int    filelen = 0;
  int    newlen = 0;
  int    newpos = 0;

  filelen = strlen(infile);
  for (i = 0; i < filelen; i++)
    if (infile[i] == ' ')
      spc++;

  newlen = filelen + spc + 1;
  errno = 0;
  tmpbuf = (char *)malloc(newlen);
  if (tmpbuf == NULL)
  {
    fprintf(stderr, "escape_space: can't allocate memory; errno=%d\n", errno);
    return (1);
  }
  memset(tmpbuf, '\0', newlen);

  i = 0;
  while (i < filelen)
  {
    if (infile[i] == ' ')
    {
      tmpbuf[newpos++] = '\\';
      tmpbuf[newpos] = infile[i];
    } else
      tmpbuf[newpos] = infile[i];
    newpos++;
    i++;
  }
  strncpy(outfile, tmpbuf, newlen);
  free(tmpbuf);
  return (0);
}

int 
get_file_stat(char *inputfile, struct stat * fileattr)
{
  struct stat     ffattr;
  int             err = 0;
  char           *tmp2buf = NULL;
  int    filelen = 0;


  filelen = strlen(inputfile);
  errno = 0;
  tmp2buf = (char *) malloc(filelen+1);
  if (tmp2buf == NULL)
  {
    fprintf(stderr, "get_file_stat: can't allocate memory; errno=%d\n", errno);
    return (1);
  }
  memset(tmp2buf, '\0', filelen+1);
  strncpy(tmp2buf, inputfile, filelen);

  errno=0;
  err = stat(tmp2buf, &ffattr);
  if (err != 0)
  {
    if (errno == ENOENT)
      fprintf(stderr, "get_file_stat: File %s not found, can't get stat info\n", inputfile);
    else
      fprintf(stderr, "get_file_stat: Cannot stat file %s; errno=%d\n", inputfile, errno);
    free(tmp2buf);
    return (-1);
  }
  memcpy(fileattr, &ffattr, sizeof(ffattr));
  currfile_atime=ffattr.st_atime;
  currfile_mtime=ffattr.st_mtime;
  currfile_ctime=ffattr.st_ctime;
  free(tmp2buf);
  return (0);
}

int 
proc_dir_list(char *instr)
{
  DIR            *dirptr;
  struct dirent  *direntptr;
  int             dir_name_len = 0;
  char           *curr_path = NULL;
  struct stat     fstat;
  struct stat     infd_stat;
  int             err = 0;
  char            tmpbuf[4096];

  errno = 0;
  dir_name_len = strlen(instr);
  dirptr = opendir(instr);

#ifdef DEBUG
  printf("Checking directory <%s>\n",instr);
#endif

  if (dirptr == NULL)
  {
    fprintf(stderr, "proc_dir_list: Can't open dir %s; errno=%d\n", instr, errno);
    return (1);
  }
  errno = 0;
  curr_path = (char *)malloc(MAXPATH + 1);
  if (curr_path == NULL)
  {
    fprintf(stderr, "proc_dir_list: Can't allocate enough space; errno=%d\n", errno);
    closedir(dirptr);
    return (1);
  }
  memset(curr_path, '\0', MAXPATH+1);
  strncpy(curr_path, instr, MAXPATH);
  errno = 0;
  while ((direntptr = readdir(dirptr)) != NULL)
  {
    /* readdir give us everything and not necessarily in order. This 
       logic is just silly, but it works */
    if (((direntptr->d_name[0] == '.') &&
         (direntptr->d_name[1] == '\0')) ||
        ((direntptr->d_name[0] == '.') &&
         (direntptr->d_name[1] == '.') &&
         (direntptr->d_name[2] == '\0')))
      continue;

    errno = 0;
    strncat(curr_path, direntptr->d_name, MAXPATH);
    err = get_file_stat(curr_path, &fstat);

    if (err == -1)
    {
      if (errno == ENOENT)
        fprintf(stderr, "proc_dir_list: file %s not found, can't stat\n", curr_path);
      else
        fprintf(stderr, "proc_dir_list: Cannot stat file %s; errno=%d\n", curr_path, errno);
      closedir(dirptr);
      return (1);
    }
    if ((fstat.st_mode & S_IFMT) == S_IFDIR)
    {
      strncat(curr_path, "/", MAXPATH);
      proc_dir_list(curr_path);
    } else if ((fstat.st_size > 0) && ((fstat.st_mode & S_IFMT) == S_IFREG))
    {

      memset(&tmpbuf, '\0', 4096);
      if (escape_space(curr_path, tmpbuf) == 0)
      {
        /*
         * This uses the device id and the inode id for both the log file
         * and the current file. These two pieces of information combined form
         * a key for each file that is guaranteed to be unique no matter what.
         * Easiest way to reliably check if two files are the same without
         * requiring any new libraries
         */
        if (logfilename != NULL && fstat.st_dev == log_file_stat.st_dev && 
                fstat.st_ino == log_file_stat.st_ino)
            printf("Skipping log file: %s\n", curr_path);
        else
        {
#ifdef DEBUG
printf("Processing file %s\n",curr_path);
#endif
          ccsrch(curr_path);
        }
      }
    }
#ifdef DEBUG
    else
    {
      if (fstat.st_size > 0)
        fprintf(stderr, "proc_dir_list: Unknown mode returned-> %x for file %s\n", fstat.st_mode,curr_path);
      else
        fprintf(stderr, "Not processing file of size 0 bytes: %s\n", curr_path);
    }
#endif
    curr_path[dir_name_len] = '\0';
  }

  free(curr_path);

  closedir(dirptr);
  return (0);
}

int 
check_mastercard_16(long offset)
{
  char            num2buf[3];
  int             vnum = 0;

  memset(&num2buf, '\0', 3);
  snprintf(num2buf, 3, "%d%d", cardbuf[0], cardbuf[1]);
  vnum = atoi(num2buf);
  if ((vnum > 50) && (vnum < 56))
  {
    print_result("MASTERCARD", 16, offset);
    return 1;
  } else
      return 0;
}

int 
check_visa_16(long offset)
{
  char            num2buf[2];
  int             vnum = 0;

  memset(&num2buf, '\0', 2);
  snprintf(num2buf, 2, "%d", cardbuf[0]);
  vnum = atoi(num2buf);
  if (vnum == 4)
  {
    print_result("VISA", 16, offset);
    return 1;
  } else
      return 0;
}

int 
check_discover_16(long offset)
{
  char            num2buf[5];
  int             vnum = 0;

  memset(&num2buf, '\0', 5);
  snprintf(num2buf, 5, "%d%d%d%d", cardbuf[0], cardbuf[1], cardbuf[2], cardbuf[3]);
  vnum = atoi(num2buf);
  if (vnum == 6011)
  {
    print_result("DISCOVER", 16, offset);
    return 1;
  } else
      return 0;
}

int 
check_jcb_16(long offset)
{
  char            num2buf[5];
  int             vnum = 0;

  memset(&num2buf, '\0', 2);
  snprintf(num2buf, 5, "%d%d%d%d", cardbuf[0], cardbuf[1], cardbuf[2], cardbuf[3]);
  vnum = atoi(num2buf);
  if ((vnum == 3088) || (vnum == 3096) || (vnum == 3112) || (vnum == 3158) || (vnum == 3337) || (vnum == 3528) || (vnum == 3529))
  {
    print_result("JCB", 16, offset);
    return 1;
  } else
      return 0;
}

int 
check_amex_15(long offset)
{
  char            num2buf[3];
  int             vnum = 0;

  memset(&num2buf, '\0', 3);
  snprintf(num2buf, 3, "%d%d", cardbuf[0], cardbuf[1]);
  vnum = atoi(num2buf);
  if ((vnum == 34) || (vnum == 37))
  {
    print_result("AMEX", 15, offset);
    return 1;
  } else
      return 0;
}

int 
check_enroute_15(long offset)
{
  char            num2buf[5];
  int             vnum = 0;

  memset(&num2buf, '\0', 5);
  snprintf(num2buf, 5, "%d%d%d%d", cardbuf[0], cardbuf[1], cardbuf[2], cardbuf[3]);
  vnum = atoi(num2buf);
  if ((vnum == 2014) || (vnum == 2149))
  {
    print_result("ENROUTE", 15, offset);
    return 1;
  } else
      return 0;
}

int 
check_jcb_15(long offset)
{
  char            num2buf[5];
  int             vnum = 0;

  memset(&num2buf, '\0', 5);
  snprintf(num2buf, 5, "%d%d%d%d", cardbuf[0], cardbuf[1], cardbuf[2], cardbuf[3]);
  vnum = atoi(num2buf);
  if ((vnum == 2131) || (vnum == 1800) || (vnum == 3528) || (vnum == 3529))
  {
    print_result("JCB", 15, offset);
    return 1;
  } else
      return 0;
}

int 
check_diners_club_cb_14(long offset)
{
  char            num2buf[4];
  char            num2buf2[3];
  int             vnum = 0;
  int             vnum2 = 0;

  memset(&num2buf, '\0', 4);
  memset(&num2buf2, '\0', 2);
  snprintf(num2buf, 4, "%d%d%d", cardbuf[0], cardbuf[1], cardbuf[2]);
  snprintf(num2buf2, 3, "%d%d", cardbuf[0], cardbuf[1]);
  vnum = atoi(num2buf);
  vnum2 = atoi(num2buf2);
  if (((vnum > 299) && (vnum < 306)) || ((vnum > 379) && (vnum < 389)) || (vnum2 == 36))
  {
    print_result("DINERS_CLUB_CARTE_BLANCHE", 14, offset);
    return 1;
  } else
      return 0;
}

/** It has been purported that these cards are no longer in use
void 
check_visa_13(long offset)
{
  char            num2buf[2];
  int             vnum = 0;

  memset(&num2buf, '\0', 2);
  snprintf(num2buf, 2, "%d\0", cardbuf[0]);
  vnum = atoi(num2buf);
  if (vnum == 4)
    print_result("VISA", 13, offset);
}
**/

void 
cleanup_shtuff()
{
  int end_time=0;

  end_time=time(NULL);
  fprintf(stdout, "\n\nFiles searched ->\t\t%ld\n", file_count);
  fprintf(stdout, "Search time (seconds) ->\t%d\n", ((int)time(NULL) - init_time));
  fprintf(stdout, "Credit card matches ->\t\t%ld\n", total_count);
  fprintf(stdout, "Binary types skipped ->\t\t%d\n", skipped_executable_count);
  fprintf(stdout, "Archives extracted ->\t\t%d\n", extracted_archive_count);
  if (tracksrch)
    fprintf(stdout, "Track data pattern matches->\t%d\n\n", trackdatacount);
  fprintf(stdout, "\nLocal end time: %s\n\n", ctime((time_t *)&end_time));
}

void 
process_cleanup()
{
  if (logfilefd>0)
    fclose(logfilefd);
  cleanup_shtuff();
  exit(0);
}

void 
signal_proc()
{
  signal(SIGHUP, process_cleanup);
  signal(SIGTERM, process_cleanup);
  signal(SIGINT, process_cleanup);
  signal(SIGQUIT, process_cleanup);
}

void 
usage(char *progname)
{
  printf("%s\n", PROG_VER);
  printf("Usage: %s <options> <start path>\n", progname);
  printf("  where <options> are:\n");
  printf("    -b\t\t   Add the byte offset into the file of the number\n"); 
  printf("    -e\t\t   Include the Modify Access and Create times in terms \n\t\t   of seconds since the epoch\n");
  printf("    -f\t\t   Only print the filename w/ potential PAN data\n"); 
  printf("    -j\t\t   Include the Modify Access and Create times in terms \n\t\t   of normal date/time\n");
  printf("    -o <filename>  Output the data to the file <filename> vs. standard out\n");

  printf("    -t <1 or 2>\t   Check if the pattern follows either a Track 1 \n\t\t   or 2 format\n");
  printf("    -T\t\t   Check for both Track 1 and Track 2 patterns\n");

  printf("    -h\t\t   This stuff\n\n");
  printf("Please see the README file for a description of how this program works.\n\n");
  exit(0);
}

int 
open_logfile()
{
  errno = 0;
  if (logfilename!=NULL)
  {
    logfilefd = fopen(logfilename, "a+");
    if (logfilefd == NULL)
    {
      fprintf(stderr, "Unable to open logfile %s for writing; errno=%d\n", logfilename, errno);
      return (-1);
    }
    stat(logfilename, &log_file_stat);
  }
  return (0);
}

int check_dir (char *name)
{
  DIR            *dirptr;
  
  dirptr = opendir(name);
  if (dirptr!=NULL)
  {
    closedir(dirptr);
    return(1);
  }
  else
    return (0);
}

/* Don't define this if we're compiling for the test suite */
#ifndef TEST_MAIN
int 
main(int argc, char *argv[])
{
  struct stat	ffstat;
  char		*inputstr = NULL;
  char		*inbuf = NULL;
  char		*tracktype_str=NULL;
  char		tmpbuf[4096];
  int		inlen = 0, err=0, c=0;

  if (argc < 2)
    usage(argv[0]);

  /* Initialise the extensions. Will set errno on failure */
  if (!initialise_mods())
    exit(-1);

  // Default shortcut option
  opt_shortcut = 0;
  shortcut_breakout = 0;

  while ((c = getopt(argc, argv,"befjt:To:s")) != -1)
  {
    switch (c)
    {
    case 'b':
      print_byte_offset=1;
      break;
    case 'e':
      print_epoch_time=1;
      break;
    case 'f':
      print_filename_only=1;
      break;
    case 'j':
      print_julian_time=1;
      break;
    case 'o':
      logfilename=optarg;
      break;
    case 't':
      tracksrch=1;
      tracktype_str=optarg;
      if (atoi(tracktype_str)==1)
        tracktype1=1;
      else if (atoi(tracktype_str)==2)
        tracktype2=1;
      else
        usage(argv[0]);
      break;
    case 'T':
      tracksrch=1;
      tracktype1=1;
      tracktype2=1;
      break;
    case 's':
      opt_shortcut = 1;
      break;
    case 'h':
    default:
      usage(argv[0]);
      break;
    }
  } 

  inputstr = argv[optind];
  if (inputstr == NULL)
    usage(argv[0]);

  inlen = strlen(inputstr) + 1;

  if (open_logfile() < 0)
    exit(-1);

  inbuf = (char *)malloc(inlen + 1);

  memset(inbuf, '\0', inlen+1);
  strncpy(inbuf, inputstr, inlen);

  file_count = 0;

  signal_proc();

  init_time = time(NULL);
  printf("\n\t\t%s\n", PROG_VER);
  printf("\nLocal start time: %s\n",ctime((time_t *)&init_time));
  if (check_dir(inbuf)) 
  {
#ifdef WINDOWS
    if ((inbuf[strlen(inbuf) - 1]) != '\\')
      inbuf[strlen(inbuf)] = '\\';
#else
    if ((inbuf[strlen(inbuf) - 1]) != '/')
      inbuf[strlen(inbuf)] = '/';
#endif
    proc_dir_list(inbuf);
  }
  else
  {
    err = get_file_stat(inbuf, &ffstat);
    if (err == -1)
    {
      if (errno == ENOENT)
        fprintf(stderr, "File %s not found, can't stat\n", inbuf);
      else
        fprintf(stderr, "Cannot stat file %s; errno=%d\n", inbuf, errno);
      exit (-1);
    }

    if ((ffstat.st_size > 0) && ((ffstat.st_mode & S_IFMT) == S_IFREG))
    {

      memset(&tmpbuf, '\0', 4096);
      if (escape_space(inbuf, tmpbuf) == 0)
      {
        if (logfilename != NULL)
          if (strstr(inbuf, logfilename) != NULL)
              fprintf(stderr, "main: We seem to be hitting our log file, so we'll leave this out of the search -> %s\n", inbuf);
          else
          {
#ifdef DEBUG
            printf("Processing file %s\n",inbuf);
#endif
            ccsrch(inbuf);
          }
        else
        {
#ifdef DEBUG
          printf("Processing file %s\n",inbuf);
#endif
          ccsrch(inbuf);
        }
      }
    }
    else if ((ffstat.st_mode & S_IFMT) == S_IFDIR)
    {
#ifdef WINDOWS
      if ((inbuf[strlen(inbuf) - 1]) != '\\')
        inbuf[strlen(inbuf)] = '\\';
#else
      if ((inbuf[strlen(inbuf) - 1]) != '/')
        inbuf[strlen(inbuf)] = '/';
#endif
      proc_dir_list(inbuf);

    }
    else {
      fprintf(stderr, "main: Unknown mode returned-> %x\n", ffstat.st_mode);
    }
  }

  cleanup_shtuff();
  free(inbuf);

  return (0);
}
#endif /* TEST_MAIN */
