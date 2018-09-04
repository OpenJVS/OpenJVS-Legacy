CC := gcc
TARGET := bin/JVSE
CFLAGS := -std=c99
INC := include
SRCDIR := src

JVSE: $(SRCDIR)/JVSE.c
	$(CC) -o $(TARGET) $(CFLAGS) -I $(INC) $(SRCDIR)/JVSE.c

clean:
	rm -f $(TARGET)
