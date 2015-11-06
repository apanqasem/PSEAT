include Makeinclude.config

SUBDIRS = eval search driver interface ml
BUILDDIRS = $(addprefix $(PSEAT_TOP_SRC_PATH)/, $(SUBDIRS))

all: $(EXEC)

$(EXEC): 
	@for i in $(BUILDDIRS) ; do \
	    cd "$$i" && $(MAKE); \
	done
	$(CXX) -o $(PSEAT_EXE_PATH)/$(EXEC) $(CXXFLAGS) $(LDFLAGS) $(ALLLIBS)
	/bin/ln -fs $(PSEAT_SCRIPT_PATH)/build_run_profile.sh $(PSEAT_EXE_PATH)/build_run_profile.sh
	/bin/ln -fs $(PSEAT_SCRIPT_PATH)/config_tuner.sh  $(PSEAT_EXE_PATH)/config_tuner.sh 
	/bin/ln -fs $(PSEAT_SCRIPT_PATH)/gen_makefile_with_opts.sh $(PSEAT_EXE_PATH)/gen_makefile_with_opts.sh 
	/bin/ln -fs $(PSEAT_SCRIPT_PATH)/gen_primary.sh $(PSEAT_EXE_PATH)/get_primary.sh 
clean:
	@for i in $(BUILDDIRS) ; do \
	    cd "$$i" && $(MAKE) clean; \
	done



