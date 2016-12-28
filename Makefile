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
HIGHTLIGHT += -e "s/(warning[^:]*):/\\033[33m\1\\033[0m:/I"
HIGHTLIGHT += -e "s/(error[^:]*):/\\033[31m\1\\033[0m:/I"
HIGHTLIGHT += -e "s/(note):/\\033[36m\1\\033[0m:/I"

.PHONY: all clean fclean re client doc test

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
	@printf "Linking %b with %b" "\033[36m$@\033[0m" "\033[33m$(LDLIBS)\033[0m\n"
	@$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(CLIENT): $(BLDDIR)/Client/main.o | $(OUTDIR)
	@printf "Linking %b with %b" "\033[36m$@\033[0m" "\033[33m$(LDLIBS)\033[0m\n"
	@$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

.SECONDEXPANSION:
$(OBJECT): $(BLDDIR)/%.o: $(SRCDIR)/%.cpp | $$(@D)
	@printf "Compiling %b\n" "\033[36m$*.cpp\033[0m"
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

$(BLDDIR)/Client/%.o: Client/%.cpp | $(BLDDIR)/Client
	@printf "Compiling %b\n" "\033[36m$*.cpp\033[0m"
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

$(OUTDIR) $(DIR) $(BLDDIR)/Client: %:
	@echo
	@mkdir $*

ifeq "$(MAKECMDGOALS)" ""
-include $(DEPEND)
endif
