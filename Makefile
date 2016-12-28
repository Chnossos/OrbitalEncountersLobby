OUTDIR := bin
SRCDIR := src
BLDDIR := build
TARGET := $(OUTDIR)/OrbitalEncountersLobby
CLIENT := $(OUTDIR)/Client

DIR    := $(patsubst $(SRCDIR)%,$(BLDDIR)%,$(shell find $(SRCDIR) -type d))
SOURCE := $(shell find $(SRCDIR) -iname "*.cpp" -printf '%d\t%p\n' | sort -nk1 | cut -f2-)
OBJECT := $(SOURCE:$(SRCDIR)/%.cpp=$(BLDDIR)/%.o)
DEPEND := $(OBJECT:.o=.d)

CXX      := g++ -fdiagnostics-color
CPPFLAGS := -MMD -MP -Iinclude -pthread
CXXFLAGS := -std=c++14 -Wall -W -pedantic -g3
LDFLAGS  := -pthread
LDLIBS   := -lboost_system

ifeq "$(OS)" "Windows_NT"
TARGET   := $(TARGET).exe
CLIENT   := $(CLIENT).exe
CPPFLAGS += -IC:\C++\boost\boost_1_59_0
CXXFLAGS += -Wno-deprecated-declarations
LDFLAGS  += -LC:\C++\boost\boost_1_59_0\lib
LDLIBS   := -lboost_system-mgw51-mt-s-1_59 -lws2_32 -lwsock32
endif

HIGHTLIGHT := sed -r
HIGHTLIGHT += -e "s/(warning[^:]*):/\\x1b[33m\1\\x1b[0m:/I"
HIGHTLIGHT += -e "s/(error[^:]*):/\\x1b[31m\1\\x1b[0m:/I"
HIGHTLIGHT += -e "s/(note):/\\x1b[36m\1\\x1b[0m:/I"

.PHONY: all clean fclean re client doc

all: $(TARGET) $(CLIENT)

clean:
	$(RM) -r $(BLDDIR)

fclean: clean
	$(RM) -r $(OUTDIR)

re: fclean all

client: $(CLIENT)

export PROJECT_ROOT_PATH := $(shell git rev-parse --show-toplevel)
doc: ; @doxygen doc/Doxyfile 2>&1 | $(HIGHTLIGHT)

$(TARGET): $(OBJECT) | $(OUTDIR)
	@echo -e "Linking \x1b[36m$@\x1b[0m with \x1b[33m$(LDLIBS)\x1b[0m"
	@$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(CLIENT): $(BLDDIR)/Client/main.o | $(OUTDIR)
	@echo -e "Linking \x1b[36m$@\x1b[0m with \x1b[33m$(LDLIBS)\x1b[0m"
	@$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

.SECONDEXPANSION:
$(OBJECT): $(BLDDIR)/%.o: $(SRCDIR)/%.cpp | $$(@D)
	@echo -e "Compiling \x1b[36m$*.cpp\x1b[0m"
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

$(BLDDIR)/Client/%.o: Client/%.cpp | $(BLDDIR)/Client
	@echo -e "Compiling \x1b[36m$*.cpp\x1b[0m"
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

$(OUTDIR) $(DIR) $(BLDDIR)/Client: %:
	@echo
	@mkdir $*

ifeq "$(MAKECMDGOALS)" ""
-include $(DEPEND)
endif
