CXX = g++
CXXFLAGS = -Wall -std=c++17 `pkg-config --cflags gtk+-3.0`
LDFLAGS = `pkg-config --libs gtk+-3.0` -lsodium -lgmp
SRC_DIR = src
OBJ_DIR = build

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

TARGET = $(OBJ_DIR)/cryptofrog

.PHONY: all clean deps upx

all: deps $(TARGET) upx

$(TARGET): $(OBJECTS)
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Automatically install required dependencies (Debian/Ubuntu)
deps:
	@echo "[*] Installing dependencies..."
	sudo apt update
	sudo apt install -y g++ libgtk-3-dev libsodium-dev libgmp-dev upx

# Compress the binary using UPX
upx:
	@echo "[*] Compressing binary with UPX..."
	upx --best --lzma $(TARGET)

clean:
	rm -f $(OBJ_DIR)/*.o $(TARGET)
