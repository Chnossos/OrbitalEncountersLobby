OUTDIR := bin
TARGET := $(OUTDIR)/OrbitalEncountersLobby

SRCDIR := src
SOURCE := $(shell find $(SRCDIR) -type f -name "*.cpp")
BLDDIR := build
OBJECT := $(SOURCE:$(SRCDIR)/%.cpp=$(BLDDIR)/%.o)
DEPEND := $(OBJECT:.o=.d)

CPPFLAGS := -MMD -MP -Iinclude -pthread
CXXFLAGS := -std=c++14 -Wall -W -pedantic -Wconversion
LDFLAGS  := -pthread
LDLIBS   := -lboost_system

.PHONY: all clean fclean re install

all: $(TARGET)

clean:
	$(RM) -r $(BLDDIR)

fclean: clean
	$(RM) $(TARGET)

re: fclean all

install: all
	$(CP) $(TARGET) .

$(TARGET): $(OBJECT)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

.SECONDEXPANSION:
$(BLDDIR)/%.o: $(SRCDIR)/%.cpp | $$(@D)/
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

%/:
	mkdir -p $*
