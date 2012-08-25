#
# ccsrch (c) 2007 Mike Beekey  - zaphod2718@yahoo.com
# All rights reserved
#
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#
#
# Makefile for ccsrch
#

CC	=gcc
#
# for mingw use something like the following
#INCL	= -I./ -I/mingw/include
#LIBSDIR	=  -L./ -L/mingw/lib -L/mingw/mingw32/lib
#
# for *UNIX* the following tends to work
INCL	= -I./
CFLAGS	=-O2 -static
#CFLAGS	=-O2 # for linux
#CFLAGS	=-DDEBUG # if you want a lot of noise
LDFLAGS	=-s
OBJS=  ccsrch.o
LIBSDIR	=  -L./
LIBS	= 
PROGS	=ccsrch

all:	${PROGS}

ccsrch:	${OBJS}
	${CC} ${CFLAGS} ${INCL} ${LDFLAGS} ${OBJS} ${LIBSDIR} ${LIBS} -o ccsrch

clean:
	rm -f core *.core ${PROGS} ${OBJS} ccsrch.exe

.c.o:
	${CC} ${CFLAGS} ${INCL} -c $<

install:
	cp ccsrch /usr/local/bin/
	chmod 4755 /usr/local/bin/ccsrch

