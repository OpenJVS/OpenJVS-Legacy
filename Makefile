CC := gcc
TARGET := bin/JVSE
CFLAGS := -std=c99
INC := include
SRCDIR := src
BUILDDIR := build

all: $(TARGET)

$(TARGET): JVSE.o Utilities.o
	mkdir -p bin
	$(CC) $(CFLAGS) -o $(TARGET) $(BUILDDIR)/JVSE.o $(BUILDDIR)/Utilities.o
	cp docs/.default_config bin/.config

JVSE.o: $(SRCDIR)/JVSE.c $(INC)/Utilities.h
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -I $(INC) -o $(BUILDDIR)/JVSE.o -c $(SRCDIR)/JVSE.c

Utilities.o: $(SRCDIR)/Utilities.c
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -I $(INC) -o $(BUILDDIR)/Utilities.o -c $(SRCDIR)/Utilities.c


clean:
	$(RM) -r $(BUILDDIR) $(TARGET)
	$(RM) -r bin
