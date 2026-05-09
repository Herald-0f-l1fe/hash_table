CC = g++
CFLAGS = -D_DEBUG -g -ggdb3 -std=c++17 -O0 -Wall -Wextra -I/usr/include/ \
           -Weffc++ -Wcast-align -Wcast-qual -Wchar-subscripts \
           -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal \
           -Wformat-nonliteral -Wformat-security -Wformat=2 -Winline \
           -Wlogical-op -Wnon-virtual-dtor -Woverloaded-virtual \
           -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow \
           -Wsign-conversion -Wstrict-null-sentinel -Wstrict-overflow=2 \
           -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override \
           -Wswitch-default -Wundef -Wunreachable-code \
           -Wunused -Wuseless-cast -Wvariadic-macros \
           -Wno-missing-field-initializers -Wno-narrowing \
           -Wno-old-style-cast -Wstack-protector -fcheck-new \
           -fsized-deallocation -fstack-protector -fstrict-overflow \
           -fno-omit-frame-pointer -pie -fPIE -Werror=vla \
           -fsanitize=address,undefined,leak,bounds \
           -fsanitize-address-use-after-scope \
           -fno-optimize-sibling-calls


TARGET = hash_table

SRCS = list/LIST.cpp \
       list/list_func.cpp \
       list/list_file_work.cpp \
       read_from_file_to_buffer.cpp \
       hash_function.cpp \
       hash_table.cpp \
       table_function.cpp

OBJS = $(SRCS:.cpp=.o)
HDRS = Array.h hash_table.h hash_function.h

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)
	@echo "Сборка завершена: $(TARGET)"

%.o: %.cpp $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
	@echo "Очищено."

rebuild: clean all

run: $(TARGET)
	./$(TARGET)

valgrind: $(TARGET)
	valgrind --leak-check=full ./$(TARGET)

.PHONY: all clean rebuild run valgrind