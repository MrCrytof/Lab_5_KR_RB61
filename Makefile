
# Шлях до компілятора C
CC = D:/CLion 2026.2.2/bin/mingw/bin/gcc.exe

# Параметри компіляції: увімкнення попереджень
CFLAGS = -Wall -Wextra

# Назва кінцевого виконуваного файлу
TARGET = lab5

# Список об'єктних файлів для компонування
OBJECTS = main.o functions.o

# Головна ціль: збирання всього проєкту
all: $(TARGET)

# Компонування об'єктних файлів у виконувану програму
$(TARGET): $(OBJECTS)
	"$(CC)" -o $(TARGET) $(OBJECTS) -lm

# Компіляція файлу main.c в об'єктний файл main.o
# Перекомпіляція також виконується при зміні functions.h
main.o: main.c functions.h
	"$(CC)" $(CFLAGS) -c main.c

# Компіляція файлу functions.c в об'єктний файл functions.o
# Перекомпіляція також виконується при зміні functions.h
functions.o: functions.c functions.h
	"$(CC)" $(CFLAGS) -c functions.c

# Очищення проєкту від об'єктних і виконуваного файлів
clean:
	del /Q *.o $(TARGET).exe 2>NUL
