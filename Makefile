CC = gcc
CFLAGS = -I"C:\raylib\raylib\src"
LDFLAGS = -L"C:\raylib\raylib\src" -lraylib -lgdi32 -lwinmm

TARGET = build.exe
SRCS = snake.c

$(TARGET): $(SRCS)
	$(CC) $(SRCS) -o $(TARGET) $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(TARGET)
