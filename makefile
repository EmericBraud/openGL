CC = g++
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lglfw -lGL -lGLEW -ldl -lassimp
INCLUDES = -Iinclude

SRCDIR = libs
SRCSRC = src
SRC = $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCSRC)/*.cpp) glad.c
OBJ = $(SRC:.cpp=.o)
OBJ := $(OBJ:.c=.o)
EXEC = program

# Cibles par défaut
all: $(EXEC)

# Création de l'exécutable
$(EXEC): $(OBJ) main.o
	$(CC) $^ -o $@ $(LDFLAGS)

# Compilation des fichiers .cpp en .o
%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Compilation des fichiers .c en .o
%.o: %.c
	gcc $(CFLAGS) $(INCLUDES) -c $< -o $@

# Compilation de main.cpp
main.o: main.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Nettoyage des fichiers compilés
clean:
	rm -f $(OBJ) main.o $(EXEC)

# Cible pour construire puis exécuter
run: clean all
	./$(EXEC)
