include ../../src/Makeinclude.config

.SUFFIXES: .C .h .c

COREOBJS = main.o Args.o SearchLog.o
OBJS = $(COREOBJS)
LIBRARY = $(PSEAT_LIB_PATH)/$(LIB_DRIVER)



all: $(LIBRARY)

$(LIBRARY): $(OBJS)
	$(AR) $(ARRFLAGS) $(LIBRARY) $(OBJS)

$(EXEC): 
	cd $(PSEAT_TOP_SRC_PATH); $(MAKE)

%.o: %.C
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $< -o $@
%.o: %.c
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $< -o $@

clean:
	rm -rf $(LIBRARY) *.o *~

