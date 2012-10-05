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

    // Ensure skipchars array is NULL so that reset_skip_chars initialises it
    skipchars = NULL;
    reset_skip_chars();

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
 *  Description:  Detects filetype of given file and returns true if we should
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
 * =============================================================================
 */
file_type detect_file_type(char *filename) {
    char buffer[80], *pipe_output, full_filename[MAXPATH], *file_cmd_output;
    int pipe, read_count;
    pid_t pid;
    file_type type;
    FILE *in_out;
    
    pid = pipe_and_fork(&pipe, true);
    if (pid == (pid_t) 0) {
        /* Child */
        dup2(pipe, STDOUT_FILENO);
        //dup2(pipe, STDERR_FILENO); /* Remove comment if you need to debug */

        full_filename[0] = '\0';
        strncat(full_filename, pwd, MAXPATH);
        strncat(full_filename, filename, MAXPATH);

        execlp("file", "file", "-b", full_filename, NULL);

        // Exec failed
        perror("Failed to execlp \"file\"");
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
            if (strstr(file_cmd_output, "ASCII") != NULL)
                type = ASCII;
            else if (strstr(file_cmd_output, "executable") != NULL)
                type = EXECUTABLE;
            else if (strstr(file_cmd_output, "binary") != NULL)
                type = EXECUTABLE;
            else if (strstr(file_cmd_output, "image data") != NULL)
                type = IMAGE;
            else if (strstr(file_cmd_output, "tar archive") != NULL)
                type = TAR;
            else
                type = UNKNOWN;
        }

        // Clean up
        wait(NULL);
        close(pipe);
        free(file_cmd_output);

        return type;
    } else {
        /* Fork failed */
        fprintf(stderr, "\n%s\n", "Failed to pipe and fork");
    }

    return UNKNOWN;
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
        fprintf(stderr, "reset_skip_chars: can't allocate memory; errno=%d\n",
                errno);
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
