include ../../src/Makeinclude.config

.SUFFIXES: .C .h .c

COREOBJS = IID.o  ML.o 
OBJS = $(COREOBJS) $(UTILOBJS)

LIBRARY =  $(PSEAT_LIB_PATH)/$(LIB_ML)

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

