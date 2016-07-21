OUTDIR := bin
TARGET := $(OUTDIR)/OrbitalEncountersLobby
CLIENT := $(OUTDIR)/Client

SRCDIR := src
SOURCE := $(shell find $(SRCDIR) -type f -name "*.cpp")
BLDDIR := build
OBJECT := $(SOURCE:$(SRCDIR)/%.cpp=$(BLDDIR)/%.o)
DEPEND := $(OBJECT:.o=.d)

CXX      := g++ -fdiagnostics-color
CPPFLAGS := -MMD -MP -Iinclude -pthread
CXXFLAGS := -std=c++14 -Wall -W -pedantic -Wconversion -g3
LDFLAGS  := -pthread
LDLIBS   := -lboost_system

.PHONY: all clean fclean re client
.PRECIOUS: $(BLDDIR)/%

all: $(TARGET) $(CLIENT)

clean:
	$(RM) -r $(BLDDIR)

fclean: clean
	$(RM) -r $(OUTDIR)

re: fclean all

client: $(CLIENT)

$(CLIENT): $(BLDDIR)/Client/main.o | $(OUTDIR)/
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(TARGET): $(OBJECT) | $(OUTDIR)/
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

.SECONDEXPANSION:
$(BLDDIR)/%.o: $(SRCDIR)/%.cpp | $$(@D)/
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

$(BLDDIR)/Client/%.o: Client/%.cpp | $(BLDDIR)/Client/
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

%/:
	mkdir -p $*
