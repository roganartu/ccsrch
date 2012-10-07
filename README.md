# CCSRCH

CCSRCH is a cross-platform tool for searching filesystems for credit card information.

### Copyright

Copyright (c) 2012 UQ COMS3000 Assignment 2 Group (CCFinders) <uni@roganartu.com>
Copyright (c) 2007 Mike Beekey <zaphod2718@yahoo.com>

This application is licensed under the GNU General Public License (see below & COPYING).

This project is a fork of CCSRCH as maintained by Mike Beekey at: http://sourceforge.net/projects/ccsrch/ - it is based on the last version (1.0.3) which was released in August 2007.

This readme is based on the readme from https://github.com/adamcaudill/ccsrch

### Using CCSRCH

```none
Usage: ./ccsrch <options> <start path>
  where <options> are:
    -b             Add the byte offset into the file of the number
    -e             Include the Modify Access and Create times in terms
                   of seconds since the epoch
    -f             Just output the filename with potential PAN data
    -j             Include the Modify Access and Create times in terms
                   of normal date/time
    -o <filename>  Output the data to the file <filename> vs. standard out
    -t <1 or 2>    Check if the pattern follows either a Track 1
                   or 2 format
    -T             Check for both Track 1 and Track 2 patterns
    -s             Stop parsing a file as soon as CHD is found
    -h             Usage information
```

**Examples:**

Generic search for credit card data starting in current directory with output to screen:

`ccsrch ./`

Generic search for credit card data starting in c:\storage with output to mycard.log:

`ccsrch -o mycard.log c:\storage`

Search for credit card data and check for Track 2 data formats with output to screen:

`ccsrch -t 2 ./`

Search for credit card data and check for Track 2 data formats with output to file c.log:

`ccsrch -t 2 -o c.log ./`

### Output

All output is tab deliminated with the following order (depending on the parameters):

* Source File
* Card Type
* Card Number
* Byte Offset
* Modify Time
* Access Time
* Create Time
* Track Pattern Match

### Assumptions

The following assumptions are made throughout the program searching for the 
card numbers:

1. Cards can be a minimum of 14 numbers and up to 16 numbers.
2. Card numbers must be contiguous. The only characters ignored when processing the files are carriage returns, new line feeds, and nulls.
3. Files are treated as raw binary objects and processed one character at a time.
4. Solo and Switch cards are not processed in the prefix search.
5. gzip, zip and tar archives are extracted and the archive files processed. Any other archive types must be manually extracted and checked.

**Prefix Logic**  
The following prefixes are used to validate the potential card numbers that have passed the mod 10 (Luhn) algorithm check.

Original Sources for Credit Card Prefixes:  
http://javascript.internet.com/forms/val-credit-card.html  
http://www.erikandanna.com/Business/CreditCards/credit_card_authorization.htm

### Logic Checks

```none
Card Type: MasterCard
Valid Length: 16
Valid Prefixes: 51, 52, 53, 54, 55

Card Type: VISA
Valid Length: 16
Valid Prefix: 4

Card Type: Discover
Valid Length: 16
Valid Prefix: 6011

Card Type: JCB
Valid Length: 16
Valid Prefixes: 3088, 3096, 3112, 3158, 3337, 3528, 3529

Card Type: American Express
Valid Length: 15
Valid Prefixes: 34, 37

Card Type: EnRoute
Valid Length: 15
Valid Prefixes: 2014, 2149

Card Type: JCB
Valid Length: 15
Valid Prefixes: 1800, 2131, 3528, 3529

Card Type: Diners Club, Carte Blanche
Valid Length: 14
Valid Prefixes: 36, 300, 301, 302, 303, 304, 305, 380, 381, 382, 383, 384, 385, 386, 387, 388
```

### Known Issues

One typical observation/complaint is the number of false positives that still come up.  You will need to manually review and remove these. Certain patterns will repeatedly come up which match all of the criteria for valid cards, but are clearly bogus. If there are enough cries for help, I may add some additional sanity checks into the logic such as bank information. In addition, there are certain system files which clearly should not have cardholder data in them and can be ignored.  There may be an "ignore file list" in a new release to reduce the amount of stuff to go through, however this will impact the speed of the tool.

Note that since this program opens up each file and processes it, obviously the access time (in epoch seconds) will change.  If you are going to do forensics, one assumes that you have already collected an image following standard forensic practices and either have already collected and preserved the MAC times, or are using this tool on a copy of the image.

For the track data search feature, the tool just examines the preceding characters before the valid credit card number and either the delimiter, or the delimeter and the characters (e.g. expiration date) following the credit card number. This public release does not perform a full pattern match using the Track 1 or Track 2 formats.

We have found that for some POS software log files are generated that not only wrap across multiple lines, but insert hex representations of the ASCII values of the PAN data as well. Furthermore, these log files may contain track data. Remember that the only way that ccsrch will find the PAN data and track data is if it is contiguous. In certain instances you may luck out because the log files will contain an entire contigous PAN and will get flagged. We would encourage you to visually examine the files identified for confirmation. Introducing logic to capture all of the crazy possible storage representations of PAN and track data we've seen would make this tool a beast.

Please note that ccsrch recurses through the filesystem given a start directory and will attempt to open any file or object read-only one at a time. Given that this could be performance or load intensive depending on the existing load on the system or its configuration, we recommend that you run the tool on a subset or sample of directories first in order to get an idea of the potential impact. We disclaim all liability for any performance impact, outages, or problems ccsrch could cause.

### Porting

This tool has been successfully compiled and run on the following operating systems: FreeBSD, Linux, SCO 5.0.4-5.0.7, Solaris 8, AIX 4.1.X, Windows 2000, Windows XP, and Windows 7.  If you have any issues getting it to run on any systems, please contact the author.

### Building

Linux/Unix:  

```none
$ wget -O ccsrch.tar.gz https://github.com/roganartu/ccsrch/tarball/master
$ tar -xvzf ccsrch.tar.gz 
$ cd roganartu-ccsrch-<rev>/
$ make all
```

Windows:  
Install [MinGW](http://www.mingw.org/) ([installer](http://sourceforge.net/projects/mingw/files/Installer/mingw-get-inst/))  
`mingw32-make all`

### Contributing

1. Contact me. Your modifications might already exist, be in progress or not be wanted
2. Fork
3. Make changes
4. Make tests. Your changes will not be accepted without tests
5. Submit a pull request

Don't change the version numbers. If you have to change the them, do it in a separate commit so I can ignore it when merging your pull request.

### Revisions

1.0.9 (Oct. 7, 2012):

* Fixed pdf parsing on OSX
* Fixed potential segfault in print_result

1.0.8 (Oct. 7, 2012):

* Added line numbers to results
* Added compressed file tests
* Added checking of sub process exit codes
* Cleaned up compiler warnings

1.0.7 (Oct. 7, 2012):

* Added gzip and tar handling
* Added PDF handling

1.0.6 (Oct. 6, 2012):

* Added shortcut option
* Fixed unzip occasionally handing at prompt
* Added more file type detections
* Changed core of file type detection to use mime types instead of file tool strings
* Fixed bug with handling of absolute search paths

1.0.5 (Oct. 6, 2012):

* Modified character ignore logic to allow dynamic modification of ignore characters, depending on filetype.
* Added zip archive handling. Supports recursive extraction.
* Fixed bug in test suite regarding newer versions of waitpid
* Added file type detection using POSIX fork and file
* Added executable, image and binary detection and skipping using file type detection

1.0.4 (Sep. 3, 2012):

* Added basic test suite
* Detect parsing of log file through absolute paths, symlinks and hardlinks

1.0.3 (Aug. 28, 2007):

* Added the ability to just output filenames of potential PAN data.
* Removed the 13 digit VISA  number check.
* Cleaned up some error and signal handling that varied across operating systems.

1.0.2 (Dec. 12, 2005):

* Added some additional track data format assumptions for track 1.

1.0.1 (Sep. 27, 2005):

* Added options for searching files for track data patterns.
* Added the ability to select certain output options on the command line.

0.9.3 (Jul. 28, 2005):

* Removed extraneous calls.
* Simplified parameter passing. 
* Fixed non-portable type issues. 
* Removed debugging info.

0.9.1 (Jul. 15, 2005):

* Initial release.

### Contributors

#### New Source (From 1.0.4)
Tony L, Ashley K, Jonathan D, Justin V, Nils A, Dan B, Michael H

#### Original Source (Up to 1.0.3) - May be wrong. Hard to track down
John A, Kyley S, Anand S, Chris L, Mitch A, Bill L, Phoram M

### License

This program is free software; you can redistribute it and/or modify it under  the terms of the GNU General Public License as published by the Free  Software Foundation; either version 2 of the License, or (at your option)  any later version.
 
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 
You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
