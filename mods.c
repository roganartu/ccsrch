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
