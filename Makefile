CC := gcc
TARGET := bin/JVSE
CFLAGS := -std=gnu99 -pthread
INC := include
SRCDIR := src
BUILDDIR := build

all: $(TARGET)

$(TARGET): JVSE.o Utilities.o Control.o Config.o
	mkdir -p bin
	$(CC) $(CFLAGS) -o $(TARGET) $(BUILDDIR)/JVSE.o $(BUILDDIR)/Utilities.o $(BUILDDIR)/Control.o $(BUILDDIR)/Config.o

JVSE.o: $(SRCDIR)/JVSE.c $(INC)/Utilities.h $(INC)/Control.h $(INC)/Config.h
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -I $(INC) -o $(BUILDDIR)/JVSE.o -c $(SRCDIR)/JVSE.c

Utilities.o: $(SRCDIR)/Utilities.c
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -I $(INC) -o $(BUILDDIR)/Utilities.o -c $(SRCDIR)/Utilities.c

Control.o: $(SRCDIR)/Control.c
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -I $(INC) -o $(BUILDDIR)/Control.o -c $(SRCDIR)/Control.c

Config.o: $(SRCDIR)/Config.c
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -I $(INC) -o $(BUILDDIR)/Config.o -c $(SRCDIR)/Config.c

Keyboard.o: $(SRCDIR)/Keyboard.c
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -I $(INC) -o $(BUILDDIR)/Keyboard.o -c $(SRCDIR)/Keyboard.c

clean:
	$(RM) -r $(BUILDDIR) $(TARGET)
	$(RM) -r bin
