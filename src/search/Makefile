include ../../src/Makeinclude.config

.SUFFIXES: .C .h .c

COREOBJS = RandomSearch.o  SearchAlgo.o Anneal.o Exhaustive.o DirectSearch.o 
UTILOBJS = #Assert.o Point.o
OBJS = $(COREOBJS) $(UTILOBJS)

LIBRARY =  $(PSEAT_LIB_PATH)/$(LIB_SEARCH)

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

