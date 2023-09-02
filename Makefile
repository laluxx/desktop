CC = gcc
CFLAGS = `pkg-config --cflags gtk+-3.0 gio-2.0`
LIBS = `pkg-config --libs gtk+-3.0 gio-2.0`
SRC = desktopfm.c
TARGET = dfm
all: $(TARGET)
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LIBS)
clean:
	rm -f $(TARGET)
.PHONY: all clean
