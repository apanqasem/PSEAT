#
# Program reads a list of compiler flags from the supplied input file and inserts into makefile
#
# @author: Apan Qasem
# @date: 07/21/15
#

#!/usr/bin/python 

import sys, getopt

# must have at least one argument 
if len(sys.argv) < 2:
    print "Usage: insert_opt_flags.py [OPTIONS] -f <flagsfile>"
    sys.exit(1)

# process command-line arugments 
try:
    opts, args = getopt.getopt(sys.argv[1:],"hf:",["help","flagsfile="])
except getopt.GetoptError:
    print "insert_opt_flags.py [OPTIONS] -f <flagsfile>"
    sys.exit(1)

optfilename = ""    
for opt, arg in opts:
    if opt in ("-h","--help"):
        print "insert_opt_flags.py [OPTIONS] -f FILE"
        sys.exit()
    elif opt in ("-f", "--flagsfile"):
        optfilename = arg

# read flags from input file 
flags=""
try:
    optfile = open(optfilename,"r")
    flags = optfile.read()
    flags = "TUNEFLAGS=" + flags
except (OSError, IOError):
    print "unable to read file ", optfilename
    sys.exit(1)


# look for "makefile" or "Makefile" in current directory 
makefile = ""
try: 
    makefile = open("makefile", "r+")
except (OSError, IOError):
    try:
        makefile = open("Makefile", "r+")
    except (OSError, IOError):
        print "could not find makefile or Makefile in current directory"
        sys.exit(1)

# create a new makefile by inserting flags into current makefile
for line in makefile:
    if line.find("$(CC)") != -1:
        s = line.split(" ")
        for i in range (0,len(s)):
            if i == len(s) - 1:
                strlist = list(s[i])
                strlist[len(strlist) - 1] = " "
                s[i] = "".join(strlist)
            flags = flags + s[i] + " "
            if i == len(s) - 1:
                flags = flags + " $(TUNEFLAGS)\n" 
        continue
    flags = flags + line


# write to makefile 
try: 
    newMakefile = open("makefile.gen", "w")
    newMakefile.write(flags)
except (OSError, IOError):
    print "could not create new makefile"
    sys.exit(1)
