# Компилятор и флаги
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic
LDFLAGS = -pthread
TARGET = lab2
SOURCES = lab2.c

# Сборка по умолчанию
all: $(TARGET)

# Компиляция основной программы
$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) $(LDFLAGS)



# Запуск с тестовыми параметрами
test: $(TARGET)
	./$(TARGET) 3 5 4 10

# Помощь
help:
	@echo "Доступные команды:"
	@echo "  make all      - сборка программы"
	@echo "  make test     - запуск с тестовыми параметрами"
	@echo "  make help     - эта справка"




#Несколько тестов
##./lab2 15 50 2 95
##./lab2 25 150 2 270
