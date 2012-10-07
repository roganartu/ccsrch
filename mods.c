/*
 * =============================================================================
 *
 *       Filename:  mods.c
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
#include "mods.h"

/* Global variable definitions */
struct stat log_file_stat = (struct stat) {0};

/* Method definitions */

/*
 * ===  FUNCTION  ==============================================================
 *         Name:  initialise_mods
 *
 *  Description:  Any initialisations you have to do to variables go here.
 *                This method is called immediately after argument count
 *                checking.
 *                This method is expected to set errno to some value on error
 *
 *      Version:  0.0.1
 *       Params:  N/A
 *      Returns:  bool true if success
 *                bool false otherwise
 *        Usage:  initialise_mods()
 *      Outputs:  N/A
 * =============================================================================
 */
bool initialise_mods() {
    // Work out the current working directory
    if (getcwd(pwd, MAXPATH) == NULL) {
        errno = EACCES;
        return false;
    }
    // This should work for windows.
    // See http://stackoverflow.com/a/7314690/943833
    strncat(pwd, "/", MAXPATH);

    skipped_executable_count = 0;
    extracted_archive_count = 0;

    // Ensure skipchars array is NULL so that reset_skip_chars initialises it
    skipchars = NULL;
    reset_skip_chars();

    // Zip file parent tracking
    memset(extracted_parent, '\0', MAXPATH);
    memset(filetype_parent, '\0', MAXPATH);

    return true;
}


/*
 * ===  FUNCTION  ==============================================================
 *         Name:  pipe_and_fork
 *
 *  Description:  Opens a pipe, forks and then closes the output side of
 *                of the parent and the input side of the child. Changes fd
 *                to be the input/output side of the pipe depending on whether
 *                child or parent
 *
 *      Version:  0.0.1
 *       Params:  int *fd - Pointer to file descriptor to set
 *                bool reverse - Declares which direction the pipe should be
 *      Returns:  pid_t pid of child if parent
 *                0 if child
 *                < 0 if either pipe or fork failed
 *        Usage:  pipe_and_fork(int *fd, bool reverse)
 *      Outputs:  N/A
 * =============================================================================
 */
pid_t pipe_and_fork(int *fd, bool reverse) {
    pid_t pid;
    int pipefds[2];
    bool parent, child;

    if (pipe(pipefds))
        return -1;

    parent = 1;
    child = 0;
    if (reverse) {
        parent = 0;
        child = 1;
    }

    pid = fork();

    if (pid == (pid_t) 0) {
        /* Child */
        close(pipefds[parent]);
        *fd = pipefds[child];
        return 0;
    } else if (pid > (pid_t) 0) {
        /* Parent */
        close(pipefds[child]);
        *fd = pipefds[parent];
        return pid;
    } else {
        /* Error */
        close(pipefds[child]);
        close(pipefds[parent]);
        return -1;
    }
}


/*
 * ===  FUNCTION  ==============================================================
 *         Name:  detect_file_type
 *
 *  Description:  Detects file type of given file and returns true if we should
 *                skip it and false otherwise.
 *                Forks and calls file from command line. Parses result to
 *                determine file type.
 *
 *      Version:  0.0.1
 *       Params:  char *filename
 *      Returns:  bool true if type to skip
 *                bool false otherwise
 *        Usage:  detect_file_type( char *filename )
 *      Outputs:  N/A
 *
 *        Notes:
 *                Open Office mime types from:
 *                  http://www.openoffice.org/framework/documentation/mimetypes/mimetypes.html
 *                Image mime types from:
 *                  http://www.iana.org/assignments/media-types/image/index.html
 * =============================================================================
 */
file_type detect_file_type(char *filename) {
    char buffer[80], *pipe_output, full_filename[MAXPATH], *file_cmd_output;
    int pipe, read_count, fork_count = 0;
    pid_t pid;
    file_type type;
    FILE *in_out;

    // Default
    type = UNKNOWN;
    while (fork_count++ < 2) {
        pid = pipe_and_fork(&pipe, true);
        if (pid == (pid_t) 0) {
            /* Child */
            dup2(pipe, STDOUT_FILENO);
            //dup2(pipe, STDERR_FILENO); /* Remove comment if you need to debug */

            full_filename[0] = '\0';
            if (filename[0] != '/')
                strncat(full_filename, pwd, MAXPATH - strlen(full_filename));
            strncat(full_filename, filename, MAXPATH - strlen(full_filename));

            if (fork_count == 1)
                execlp("file", "file", "--mime", "-b", full_filename, NULL);
            else
                execlp("file", "file", "-b", full_filename, NULL);

            // Exec failed
#ifdef DEBUG
            perror("detect_file_type: failed to execlp \"file\"");
#endif
            close(pipe);
            exit(errno);

        } else if (pid > (pid_t) 0) {
            /* Parent */
            in_out = fdopen(pipe, "r");
            file_cmd_output = malloc(80);
            file_cmd_output[0] = '\0';
            read_count = 1;
            while (in_out != NULL && !feof(in_out)) {
                pipe_output = fgets(buffer, 80, in_out);
                if (pipe_output != NULL) {
                    file_cmd_output = realloc(file_cmd_output,
                            strlen(file_cmd_output) + strlen(pipe_output) + 1);
                    strncat(file_cmd_output, pipe_output, 80);
                }
            }

            // Work out what it is
            if (file_cmd_output != NULL) {
                if (strstr(file_cmd_output, "text/plain") != NULL)
                    type = ASCII;
                else if (strstr(file_cmd_output, "application/x-executable") !=
                        NULL || strstr(file_cmd_output, "executable") != NULL ||
                        strstr(file_cmd_output, "relocatable") != NULL)
                    type = EXECUTABLE;
                else if (strstr(file_cmd_output, "image/") != NULL ||
                        strstr(file_cmd_output, "image data") != NULL)
                    type = IMAGE;
                else if (strstr(file_cmd_output, "video/") != NULL || 
                        strstr(file_cmd_output, "video") != NULL ||
                        strstr(file_cmd_output, "AVI") != NULL ||
                        strstr(file_cmd_output, "MPEG") != NULL)
                    type = VIDEO;
                else if (strstr(file_cmd_output, "audio/") != NULL || 
                        strstr(file_cmd_output, "application/octet-stream") !=
                        NULL || strstr(file_cmd_output, "Audio file") != NULL)
                    type = AUDIO;
                else if (strstr(file_cmd_output, "application/x-tar") != NULL)
                    type = TAR;
                else if (strstr(file_cmd_output, "application/zip") != NULL) {
                    // Due to poor msword filetype detection, we need to rely on
                    // file extensions here
                    if (strncmp(last_strstr(filename, "."), ".docx", 7) == 0)
                        type = MS_WORDX;
                    else if (strncmp(last_strstr(filename, "."), ".xlsx", 7) == 0)
                        type = MS_EXCELX;
                    else
                        // Make ZIP the default here
                        type = ZIP;
                }
                else if (strstr(file_cmd_output, "application/x-gzip") != NULL)
                    type = GZIP;
                else if (strstr(file_cmd_output, "application/xml") != NULL)
                    type = XML;
                else if (strstr(file_cmd_output, "application/pdf") != NULL)
                    type = PDF;
                else if (strstr(file_cmd_output, "opendocument.text-template") != NULL)
                    type = OTT;
                else if (strstr(file_cmd_output, "opendocument.text") != NULL)
                    type = ODT;
                else if (strstr(file_cmd_output, "opendocument.spreadsheet.template") != NULL)
                    type = OTS;
                else if (strstr(file_cmd_output, "opendocument.spreadsheet") != NULL)
                    type = ODS;
                else if (strstr(file_cmd_output, "application/vnd.ms-excel") != NULL)
                    type = MS_EXCEL;
                else if (strstr(file_cmd_output, "application/vnd.ms-word") != NULL)
                    type = MS_WORD;
                else if (strstr(file_cmd_output, "application/msword") != NULL ||
                        strstr(file_cmd_output, "application/vnd.ms-office") != NULL) {
                    // Due to poor msword filetype detection, we need to rely on
                    // file extensions here
                    if (strncmp(last_strstr(filename, "."), ".doc", 6) == 0)
                        type = MS_WORD;
                    else if (strncmp(last_strstr(filename, "."), ".xls", 6) == 0)
                        type = MS_EXCEL;
                    else
                        // Make MS_WORD the default here
                        type = MS_WORD;
                }
                // This should stay last. Other legitimate mime types often use this charset
                else if (fork_count == 2 && strstr(file_cmd_output, "binary") !=
                         NULL)
                    type = BINARY;
                else
                    type = UNKNOWN;
            }

            // Clean up
            wait(NULL);
            close(pipe);
            free(file_cmd_output);

#ifdef DEBUG
        } else {
            /* Fork failed */
            fprintf(stderr, "\n%s\n", "Failed to pipe and fork");
#endif
        }

        // Only check again if we didn't know what it was
        if (type != UNKNOWN)
            break;

    }

    return type;
}

/*
 * ===  FUNCTION  ==============================================================
 *         Name:  reset_skip_chars
 *
 *  Description:  The skipchars object allows flexible ignoring of any set of
 *                characters. This is useful for cleaning up MS Word.
 *
 *                Success is assumed on return. Failure will exit program (we
 *                can't really continue parsing if this doesn't work).
 *
 *                This method should be called by you before and after modifying
 *                skipchars char array. Call it before to ensure it is in a
 *                state you expect (if you're changing it, you're overriding),
 *                and after to ensure it is in default state in other cases.
 *
 *                When you add characters to the array, remember to modify the
 *                skipchars_count integer, so looping over the array is
 *                simplified.
 *
 *      Version:  0.0.1
 *       Params:  void
 *      Returns:  void
 *        Usage:  reset_skip_chars()
 *      Outputs:  N/A

 *        Notes:
 * =============================================================================
 */
void reset_skip_chars(void) {
    if (skipchars == NULL)
        skipchars = malloc(4);
    else
        skipchars = realloc(skipchars, 4);

    if (skipchars == NULL) {
#ifdef DEBUG
        fprintf(stderr, "reset_skip_chars: can't allocate memory; errno=%d\n",
                errno);
#endif
        exit(ENOMEM);
    }

    skipchar_count = 4;

    // These are the defaults
    skipchars[0] = ' ';  // Default for ccsrch
    skipchars[1] = '\n'; // Unix and Windows line feed
    skipchars[2] = '\r'; // Windows line feed
    skipchars[3] = '-';  // As per specification
}

/*
 * ===  FUNCTION  ==============================================================
 *         Name:  in_skipped_arr
 *
 *  Description:  Checks if the supplied character is within the skipchars
 *                array. Relies on skipchars_count reliably representing the
 *                length of the arrays.
 *
 *      Version:  0.0.1
 *       Params:  char check
 *      Returns:  bool
 *                    true if check is within skipchars
 *                    false otherwise
 *        Usage:  in_skipped_arr( char check )
 *      Outputs:  N/A

 *        Notes:
 * =============================================================================
 */
bool in_skipped_arr(char check) {
    int i;

    if (skipchars == NULL)
        return false;

    for ( i = 0; i < skipchar_count; i++ ) {
        if (check == skipchars[i])
            return true;
    }
    return false;
}

/*
 * ===  FUNCTION  ==============================================================
 *         Name:  unzip_and_parse
 *
 *  Description:  Extract files from zip archive, parse the contents and return
 *                the number of PANs found within.
 *
 *      Version:  0.0.1
 *       Params:  char *filename
 *      Returns:  int
 *                    0 on success
 *                    exit(errno) otherwise
 *        Usage:  unzip_and_parse( char *filename )
 *      Outputs:  N/A

 *        Notes:  Due to where this method is always called from, there is no
 *                argument checking. Filename is assumed to be non-null
 * =============================================================================
 */
int unzip_and_parse(char *filename) {
    char *temp_folder, parent[MAXPATH];
    pid_t pid;
    int pipe, devnull, statval, exit_code;
    char template[] = "ccsrch-tmp_folder-XXXXXX";

    if (mkdtemp(template) == NULL) {
#ifdef DEBUG
        fprintf(stderr, "unzip_and_parse: unable to create temp directory: err=%d\n",
                errno);
#endif
        return 0;
    }

    temp_folder = malloc(26);
    memset(temp_folder, '\0', 26);
    strcat(temp_folder, template);
    strcat(temp_folder, "/");

    // Extracted file attribution. Need to remember parent if necessary
    parent[0] = 0;
    if (extracted_parent[0] != 0) {
        strncpy(parent, extracted_parent, MAXPATH);
        if (index(filename, '/') != NULL) {
            strncat(extracted_parent, " -> ", 5);
            strncat(extracted_parent, index(filename, '/'),
                    MAXPATH - strlen(extracted_parent));
        }
    } else
        strncpy(extracted_parent, filename, strlen(filename) + 1);

    pid = pipe_and_fork(&pipe, true);
    if (pid == (pid_t) 0) {
        /* Child */
        devnull = open("/dev/null", O_WRONLY);
        dup2(devnull, STDOUT_FILENO);
        dup2(devnull, STDERR_FILENO);
        execlp("unzip", "unzip", "-o", "-d", temp_folder, filename, NULL);
    } else if (pid > (pid_t) 0) {
        /* Parent */
        exit_code = 0;

        wait(&statval);
        if(WIFEXITED(statval))
            exit_code = WEXITSTATUS(statval);

        if (exit_code != 0){
#ifdef DEBUG
            fprintf(stderr, "unzip_and_parse: failed to extract file %s\n",
                    filename);
#endif
            return 0;
        }

        close(pipe);
    } else {
        /* Fork failed */
#ifdef DEBUG
        fprintf(stderr, "\n%s\n", "unzip_and_parse: failed to pipe and fork\n");
#endif
        return 0;
    }

    // Now that we've unzipped, let's parse that folder and then delete it
    proc_dir_list(temp_folder);

    // Cleanup
    remove_directory(temp_folder);
    free(temp_folder);

    extracted_archive_count++;
    if (parent[0] != 0)
        strncpy(extracted_parent, parent, MAXPATH);
    else
        extracted_parent[0] = 0;

    return 0;
}

/*
 * ===  FUNCTION  ==============================================================
 *         Name:  gunzip_and_parse
 *
 *  Description:  Extract files from gzip archive, parse the contents and return
 *                the number of PANs found within.
 *
 *      Version:  0.0.1
 *       Params:  char *filename
 *      Returns:  int
 *                    0 on success
 *                    exit(errno) otherwise
 *        Usage:  gunzip_and_parse( char *filename )
 *      Outputs:  N/A

 *        Notes:  Due to where this method is always called from, there is no
 *                argument checking. Filename is assumed to be non-null
 * =============================================================================
 */
int gunzip_and_parse(char *filename) {
    char parent[MAXPATH];
    pid_t pid;
    int pipe, gunzipped_file, devnull, total, temp_file, statval, exit_code;
    char template[] = "ccsrch-tmp_file-XXXXXX";

    temp_file = mkstemp(template);

    if (temp_file < 1) {
#ifdef DEBUG
        fprintf(stderr, "gunzip_and_parse: unable to create tmp folder\n");
#endif
        return 0;
    }

    // Extracted file attribution. Need to remember parent if necessary
    parent[0] = 0;
    if (extracted_parent[0] != 0)
        strncpy(parent, extracted_parent, MAXPATH);
    else
        strncpy(extracted_parent, filename, strlen(filename) + 1);

    pid = pipe_and_fork(&pipe, true);
    if (pid == (pid_t) 0) {
        /* Child */
        devnull = open("/dev/null", O_WRONLY);
        dup2(temp_file, STDOUT_FILENO);
        dup2(devnull, STDERR_FILENO);
        execlp("gunzip", "gunzip", "-c", "-f", filename, NULL);
    } else if (pid > (pid_t) 0) {
        /* Parent */
         exit_code = 0;

        wait(&statval);
        if(WIFEXITED(statval))
        	exit_code = WEXITSTATUS(statval);

        close(pipe);

        if (exit_code != 0){
#ifdef DEBUG
            fprintf(stderr, "gunzip_and_parse: failed to extract file %s | exit_code=%d\n",
                    filename, exit_code);
#endif
            return 0;
        }
    } else {
        /* Fork failed */
#ifdef DEBUG
        fprintf(stderr, "\n%s\n", "gunzip_and_parse: failed to pipe and fork\n");
#endif
        return 0;
    }

    // Now that we've gunzipped, let's parse that file and then delete it
    total = ccsrch(template);

    // Cleanup
    close(temp_file);
    remove(template);

    extracted_archive_count++;
    if (parent[0] != 0)
        strncpy(extracted_parent, parent, MAXPATH);
    else
        extracted_parent[0] = 0;

    return total;
}

/*
 * ===  FUNCTION  ==============================================================
 *         Name:  untar_and_parse
 *
 *  Description:  Extract files from tar archive, parse the contents and return
 *                the number of PANs found within.
 *
 *      Version:  0.0.1
 *       Params:  char *filename
 *      Returns:  int
 *                    0 on success
 *                    exit(errno) otherwise
 *        Usage:  untar_and_parse( char *filename )
 *      Outputs:  N/A

 *        Notes:  Due to where this method is always called from, there is no
 *                argument checking. Filename is assumed to be non-null
 * =============================================================================
 */
int untar_and_parse(char *filename) {
    char parent[MAXPATH], *temp_folder;
    pid_t pid;
    int pipe, devnull, total, statval, exit_code;
    char template[] = "ccsrch-tmp_folder-XXXXXX";

    if (mkdtemp(template) == NULL) {
#ifdef DEBUG
        fprintf(stderr, "untar_and_parse: unable to create tmp folder\n");
#endif
        return 0;
    }

    temp_folder = malloc(26);
    memset(temp_folder, '\0', 26);
    strcat(temp_folder, template);
    strcat(temp_folder, "/");

    // Extracted file attribution. Need to remember parent if necessary
    parent[0] = 0;
    if (extracted_parent[0] != 0) {
        strncpy(parent, extracted_parent, MAXPATH);
        if (index(filename, '/') != NULL) {
            strncat(extracted_parent, " -> ", 5);
            strncat(extracted_parent, index(filename, '/'),
                    MAXPATH - strlen(extracted_parent));
        }
    } else
        strncpy(extracted_parent, filename, strlen(filename) + 1);

    pid = pipe_and_fork(&pipe, true);
    if (pid == (pid_t) 0) {
        /* Child */
        devnull = open("/dev/null", O_WRONLY);
        dup2(devnull, STDOUT_FILENO);
        dup2(devnull, STDERR_FILENO);
        execlp("tar", "tar", "-xvf", filename, "-C", temp_folder, NULL);
    } else if (pid > (pid_t) 0) {
        /* Parent */
        exit_code = 0;

        wait(&statval);
        if(WIFEXITED(statval))
            exit_code = WEXITSTATUS(statval);

        close(pipe);

        if (exit_code != 0){
#ifdef DEBUG
            fprintf(stderr, "untar_and_parse: failed to extract file %s | exit_code=%d\n",
                    filename, exit_code);
#endif
            return 0;
        }
    } else {
        /* Fork failed */
#ifdef DEBUG
        fprintf(stderr, "\n%s\n", "untar_and_parse: failed to pipe and fork\n");
#endif
        return 0;
    }

    // Now that we've unzipped, let's parse that folder and then delete it
    proc_dir_list(temp_folder);

    // Cleanup
    remove_directory(temp_folder);
    free(temp_folder);

    extracted_archive_count++;
    if (parent[0] != 0)
        strncpy(extracted_parent, parent, MAXPATH);
    else
        extracted_parent[0] = 0;

    return 0;
}

/*
 * ===  FUNCTION  ==============================================================
 *         Name:  remove_directory
 *
 *  Description:  Traverses a directory, recursively removing all files before
 *                finally removing the empty directory.
 *
 *      Version:  0.0.1
 *       Params:  char *dir
 *      Returns:  void
 *        Usage:  remove_directory( char *dir )
 *      Outputs:  N/A
 *
 *        Notes:
 * =============================================================================
 */
void remove_directory(char *dir) {
    struct dirent *d;
    DIR *dirptr;
    int err, dir_len;
    struct stat fstat;
    char *curr_path = NULL;

    dir_len = strlen(dir);

    curr_path = malloc(MAXPATH + 1);
    if (curr_path == NULL) {
#ifdef DEBUG
        fprintf(stderr, "remove_directory: unable to allocate memory. err=%d\n",
                errno);
#endif
        return;
    }
    memset(curr_path, '\0', MAXPATH+1);
    strncpy(curr_path, dir, MAXPATH);

    dirptr = opendir(dir);
    while ((d = readdir(dirptr)) != NULL) {

        /* readdir give us everything and not necessarily in order. This
        logic is just silly, but it works */
        if (((d->d_name[0] == '.') &&
                (d->d_name[1] == '\0')) ||
                ((d->d_name[0] == '.') &&
                (d->d_name[1] == '.') &&
                (d->d_name[2] == '\0')))
            continue;

        // Get each entry
        strncat(curr_path, d->d_name, MAXPATH);
        err = get_file_stat(curr_path, &fstat);

        if (err == -1) {
#ifdef DEBUG
            if (errno == ENOENT)
                fprintf(stderr, "remove_directory: file %s not found, can't stat\n", curr_path);
            else
                fprintf(stderr, "remove_directory: Cannot stat file %s; errno=%d\n", curr_path, errno);
#endif
            closedir(dirptr);
            exit(errno);
        }

        // Do the removing
        if ((fstat.st_mode & S_IFMT) == S_IFDIR)
        {
            strcat(curr_path, "/");
            remove_directory(curr_path);
        }
        else if ((fstat.st_mode & S_IFMT) == S_IFREG)
            remove(curr_path);

        // Reset the current path back to the base directory
        curr_path[dir_len] = '\0';
    }

    free(curr_path);

    rmdir(dir);
}

/*
 * ===  FUNCTION  ==============================================================
 *         Name: convert_and_parse_pdf
 *
 *  Description: Converts PDF file, parses and returns the number of PANs found
 *
 *      Version: 0.0.1
 *       Params: char *filename
 *      Returns: int status
 *                   0 on success
 *                   exit(errno) otherwise
 *        Usage: convert_and_parse_pdf( char *filename)
 *      Outputs: N/A
 *
 *        Notes: Due to where this method is always called from, there is no
 *               argument checking. Filename is assumed to be non-null
 *
 * =============================================================================
 */
int convert_and_parse_pdf(char *filename) {
    char parent[MAXPATH];
    pid_t pid;
    int pipe, devnull, total, temp_file;
    char template[] = "ccsrch-tmp_file-XXXXXX";

    temp_file = mkstemp(template);

    if (temp_file < 1) {
#ifdef DEBUG
        fprintf(stderr, "convert_and_parse_pdf: unable to create temp file\n");
#endif
        return 0;
    }

    // Extracted file attribution. Need to remember parent if necessary
    parent[0] = 0;
    if (extracted_parent[0] != 0) {
        strncpy(parent, extracted_parent, MAXPATH);
        if (index(filename, '/') != NULL) {
            strncat(extracted_parent, " -> ", 5);
            strncat(extracted_parent, index(filename, '/'),
                    MAXPATH - strlen(extracted_parent));
        }
    } else
        strncpy(extracted_parent, filename, strlen(filename) + 1);
    // Ensure we treat the file as a PDF
    strncpy(filetype_parent, filename, strlen(filename) + 1);

    pid = pipe_and_fork(&pipe, true);
    if (pid == (pid_t) 0) {
        /* Child */
        devnull = open("/dev/null", O_WRONLY);
        dup2(devnull, STDOUT_FILENO);
        dup2(devnull, STDERR_FILENO);
#ifdef __APPLE__
        execlp("bin/pdftotext", "bin/pdftotext", filename, template, NULL);
#else
        execlp("pdftotext", "pdftotext", filename, template, NULL);

#endif
    } else if (pid > (pid_t) 0) {
        /* Parent */
        wait(NULL);
        close(pipe);
    } else {
        /* Fork failed */
#ifdef DEBUG
        fprintf(stderr, "\n%s\n", "convert_and_parse_pdf: failed to pipe and fork\n");
#endif
        exit(ENOSYS);
    }

    // Now that we've scanned the pdf to txt, let's parse that file and then delete it
    total = ccsrch(template);

    // Cleanup
    close(temp_file);
    remove(template);

    if (parent[0] != 0)
        strncpy(extracted_parent, parent, MAXPATH);
    else
        memset(extracted_parent, '\0', MAXPATH);
    memset(filetype_parent, '\0', MAXPATH);

    return total;
}

/*
 * ===  FUNCTION  ==============================================================
 *         Name:  parse_xlsx
 *
 *  Description:  .xlsx files behave extremely similar to zip archives. They can
 *                be extracted and then the files within them parsed
 *                individually.
 *
 *      Version:  0.0.1
 *       Params:  char *filename
 *      Returns:  int
 *                    0 on success
 *                    exit(errno) otherwise
 *        Usage:  parse_xlsx( char *filename )
 *      Outputs:  N/A

 *        Notes:  Due to where this method is always called from, there is no
 *                argument checking. Filename is assumed to be non-null
 * =============================================================================
 */
int parse_xlsx(char *filename) {
    char parent[MAXPATH], *temp_folder, *search_file;
    pid_t pid;
    int pipe, devnull, total, statval, exit_code;
    char template[] = "ccsrch-tmp_folder-XXXXXX";

    if (mkdtemp(template) == NULL) {
#ifdef DEBUG
        fprintf(stderr, "parse_xlsx: unable to create tmp folder\n");
#endif
        return 0;
    }

    temp_folder = malloc(26);
    memset(temp_folder, '\0', 26);
    strcat(temp_folder, template);
    strcat(temp_folder, "/");
    search_file = malloc(strlen(temp_folder) + 21);
    memset(search_file, '\0', strlen(temp_folder) + 21);
    strncpy(search_file, temp_folder, strlen(temp_folder));
    strcat(search_file, "xl/sharedStrings.xml");

    // Extracted file attribution. Need to remember parent if necessary
    parent[0] = 0;
    if (extracted_parent[0] != 0) {
        strncpy(parent, extracted_parent, MAXPATH);
        if (index(filename, '/') != NULL) {
            strncat(extracted_parent, " -> ", 5);
            strncat(extracted_parent, index(filename, '/'),
                    MAXPATH - strlen(extracted_parent));
        }
    } else
        strncpy(extracted_parent, filename, strlen(filename) + 1);
    // Ensure we treat the file as a .xlsx
    strncpy(filetype_parent, filename, strlen(filename) + 1);

    pid = pipe_and_fork(&pipe, true);
    if (pid == (pid_t) 0) {
        /* Child */
        devnull = open("/dev/null", O_WRONLY);
        dup2(devnull, STDOUT_FILENO);
        dup2(devnull, STDERR_FILENO);
        execlp("unzip", "unzip", "-o", "-d", temp_folder, filename, NULL);
    } else if (pid > (pid_t) 0) {
        /* Parent */
        exit_code = 0;

        wait(&statval);
        if(WIFEXITED(statval))
            exit_code = WEXITSTATUS(statval);

        close(pipe);

        if (exit_code != 0){
#ifdef DEBUG
            fprintf(stderr, "parse_xlsx: failed to extract file %s | exit_code=%d\n",
                    filename, exit_code);
#endif
            return 0;
        }
    } else {
        /* Fork failed */
#ifdef DEBUG
        fprintf(stderr, "\n%s\n", "parse_xlsx: failed to pipe and fork\n");
#endif
        return 0;
    }

    // Now that we've got our xlsx extracted, let's have a look at the content
    processing_xlsx = 1;
    ccsrch(search_file);
    processing_xlsx = 0;

    // Cleanup
    remove_directory(temp_folder);
    free(temp_folder);
    free(search_file);

    if (parent[0] != 0)
        strncpy(extracted_parent, parent, MAXPATH);
    else
        memset(extracted_parent, '\0', MAXPATH);
    memset(filetype_parent, '\0', MAXPATH);

    return 0;
}

/*
 * ===  FUNCTION  ==============================================================
 *         Name:  parse_docx
 *
 *  Description:  .docx files behave extremely similar to zip archives. They can
 *                be extracted and then the files within them parsed
 *                individually.
 *                ODS and Microsoft differ only in location of content xml and
 *                xml structure. This method can be used for both.
 *
 *      Version:  0.0.1
 *       Params:  char *filename
 *                bool ods
 *      Returns:  int
 *                    0 on success
 *                    exit(errno) otherwise
 *        Usage:  parse_docx( char *filename )
 *      Outputs:  N/A

 *        Notes:  Due to where this method is always called from, there is no
 *                argument checking. Filename is assumed to be non-null
 * =============================================================================
 */
int parse_docx(char *filename, bool ods) {
    char parent[MAXPATH], *temp_folder, *search_file;
    pid_t pid;
    int pipe, devnull, total, statval, exit_code;
    char template[] = "ccsrch-tmp_folder-XXXXXX";

    if (mkdtemp(template) == NULL) {
#ifdef DEBUG
        fprintf(stderr, "parse_docx: unable to create tmp folder\n");
#endif
        return 0;
    }

    temp_folder = malloc(26);
    memset(temp_folder, '\0', 26);
    strcat(temp_folder, template);
    strcat(temp_folder, "/");
    search_file = malloc(strlen(temp_folder) + 18);
    memset(search_file, '\0', strlen(temp_folder) + 18);
    strncpy(search_file, temp_folder, strlen(temp_folder));
    if (ods)
        strcat(search_file, "content.xml");
    else
        strcat(search_file, "word/document.xml");

    // Extracted file attribution. Need to remember parent if necessary
    parent[0] = 0;
    if (extracted_parent[0] != 0) {
        strncpy(parent, extracted_parent, MAXPATH);
        if (index(filename, '/') != NULL) {
            strncat(extracted_parent, " -> ", 5);
            strncat(extracted_parent, index(filename, '/'),
                    MAXPATH - strlen(extracted_parent));
        }
    } else
        strncpy(extracted_parent, filename, strlen(filename) + 1);
    // Ensure we treat the file as a .docx
    strncpy(filetype_parent, filename, strlen(filename) + 1);

    pid = pipe_and_fork(&pipe, true);
    if (pid == (pid_t) 0) {
        /* Child */
        devnull = open("/dev/null", O_WRONLY);
        dup2(devnull, STDOUT_FILENO);
        dup2(devnull, STDERR_FILENO);
        execlp("unzip", "unzip", "-o", "-d", temp_folder, filename, NULL);
    } else if (pid > (pid_t) 0) {
        /* Parent */
        exit_code = 0;

        wait(&statval);
        if(WIFEXITED(statval))
            exit_code = WEXITSTATUS(statval);

        close(pipe);

        if (exit_code != 0){
#ifdef DEBUG
            fprintf(stderr, "parse_docx: failed to extract file %s | exit_code=%d\n",
                    filename, exit_code);
#endif
            return 0;
        }
    } else {
        /* Fork failed */
#ifdef DEBUG
        fprintf(stderr, "\n%s\n", "parse_docx: failed to pipe and fork\n");
#endif
        return 0;
    }

    // Now that we've got our docx extracted, let's have a look at the content
    processing_docx = 1;
    ccsrch(search_file);
    processing_docx = 0;

    // Cleanup
    remove_directory(temp_folder);
    free(temp_folder);
    free(search_file);

    if (parent[0] != 0)
        strncpy(extracted_parent, parent, MAXPATH);
    else
        memset(extracted_parent, '\0', MAXPATH);
    memset(filetype_parent, '\0', MAXPATH);

    return 0;
}

/* 
 * ===  FUNCTION  ==============================================================
 *         Name:  buf_strstr
 *
 *  Description:  Designed to search the given ccsrch_buf for the given needle,
 *                starting at the given index and finishing at index +
 *                strlen(needle)
 * 
 *      Version:  0.0.1
 *       Params:  char *buf
 *                int index
 *                int cnt
 *                char *needle
 *      Returns:  bool result
 *                    true if match
 *                    false otherwise
 *        Usage:  buf_strstr( char *buf, int index, int cnt, char *needle )
 *      Outputs:  N/A

 *        Notes:  int cnt is the length of the buffer. This is to avoid
 *                potential segfaults
 * =============================================================================
 */
bool buf_strstr(char *buf, int index, int cnt, char *needle) {
    int i;

    for ( i = index; i < index + strlen(needle); i++ ) {
        if (i >= cnt || buf[i] != needle[i - index])
            return false;
    }
    return true;
}

/*
 * From http://stackoverflow.com/a/1643946/943833
 * In response to
 * http://stackoverflow.com/questions/1634359/is-there-a-reverse-fn-for-strstr
 *
 * Basically, strstr but return last occurence, not first.
 *
 * This file contains several implementations and a harness to test and
 * benchmark them.
 *
 * Some of the implementations of the actual function are copied from
 * elsewhere; they are commented with the location. The rest of the coe
 * was written by Lars Wirzenius (liw@liw.fi) and is hereby released into
 * the public domain. No warranty. If it turns out to be broken, you get
 * to keep the pieces.
 */

/* By liw. */
static char *last_strstr(const char *haystack, const char *needle) {
    if (*needle == '\0')
        return (char *) haystack;

    char *result = NULL;
    for (;;) {
        char *p = strstr(haystack, needle);
        if (p == NULL)
            break;
        result = p;
        haystack = p + 1;
    }

    return result;
}
