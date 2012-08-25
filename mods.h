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

/* Global variable declarations */
typedef int bool;
#define false 0
#define true 1

struct stat log_file_stat;

/* Method declarations */
bool initialise_mods();

#endif
