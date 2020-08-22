CC := gcc
SRCDIR := src
BUILDDIR := build
BINDIR := bin
TARGET := bin/openjvs
 
SRCEXT := c
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS := -std=gnu99
DEFINES := 
LIB := -pthread
INC := -I include

$(TARGET): $(OBJECTS)
	@echo " Linking:"
	@echo "  $(CC) $^ -o $(TARGET) $(LIB)"; $(CC) $^ -o $(TARGET) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@echo " Building:"
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(BINDIR)
	@echo "  $(CC) $(CFLAGS) $(DEFINES) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(DEFINES) $(INC) -c -o $@ $<

clean:
	@echo " Cleaning"; 
	@echo "  $(RM) -r $(BUILDDIR) $(TARGET) $(BINDIR)"; $(RM) -r $(BUILDDIR) $(TARGET) $(BINDIR)

.PHONY: clean
