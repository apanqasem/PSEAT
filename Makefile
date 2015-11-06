BUILDROOTDIR = /home/aq10/code/PSEAT
SUBDIRS = src
BUILDDIRS = $(addprefix $(BUILDROOTDIR)/, $(SUBDIRS))
BINDIR =  $(addprefix $(BUILDROOTDIR)/, bin)
SCRIPTDIR = $(addprefix $(BUILDROOTDIR)/, src/scripts)
EXTRA_DIST = config


all:
	@for i in $(BUILDDIRS) ; do \
	    cd "$$i" && $(MAKE); \
	done
clean:
	@for i in $(BUILDDIRS) ; do \
	    cd "$$i" && $(MAKE) clean; \
	done

distclean:
	make clean
	/bin/rm -rf $(BINDIR)/pseat
	/bin/rm -rf $(BINDIR)/build_run_profile.sh

install:
	/bin/ln -fs  $(BINDIR)/pseat /usr/local/bin/pseat
	/bin/ln -fs $(SCRIPTDIR)/build_run_profile.sh /usr/local/bin/build_run_profile.sh
	/bin/ln -fs $(SCRIPTDIR)/config_tuner.sh  /usr/local/bin/config_tuner.sh
	/bin/ln -fs $(SCRIPTDIR)/gen_makefile_with_opts.sh /usr/local/bin/gen_makefile_with_opts.sh
	/bin/ln -fs $(SCRIPTDIR)/config_tuner.sh  /usr/local/bin/config_tuner.sh

test:
	cd test && ./test.sh


.PHONY: install
.PHONY: test
