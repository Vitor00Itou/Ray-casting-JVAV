# Diretórios
SRC_DIR = src
BUILD_DIR = build
BIN = $(BUILD_DIR)/raycasting

# Compilador e flags
CXX = g++
CXXFLAGS = -Wall -std=c++11
LDFLAGS = -lGL -lGLU -lglut

# Fontes
SRC = $(SRC_DIR)/main.cpp

# Regra padrão
all: $(BUILD_DIR) $(BIN)

# Compilação
$(BIN): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(BIN) $(LDFLAGS)

# Criação do diretório build se não existir
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Rodar o programa
run: all
	./$(BIN)

# Limpeza
clean:
	rm -rf $(BUILD_DIR)
