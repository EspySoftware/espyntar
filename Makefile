# Variables
RAYLIB_PATH ?= C:/raylib/raylib
PROJECT_NAME ?= main
OBJS ?= main.cpp
BUILD_MODE ?= RELEASE

# Compiler and flags  
CXX = g++
CXXFLAGS = -Wall -std=c++14 -D_DEFAULT_SOURCE -Wno-missing-braces

# Include paths
INCLUDE_PATHS = -I. -I$(RAYLIB_PATH)/src -I$(RAYLIB_PATH)/src/external -I$(RAYLIB_PATH)/src/external/glfw/include

# Library paths
LDFLAGS = -L. -L$(RAYLIB_PATH)/src

# Libraries to link
LDLIBS = -lraylib -lopengl32 -lgdi32 -lwinmm -lws2_32

# Source files
SOURCES = src/main.cpp \
          src/components/Client.cpp \
          src/components/Games.cpp \
          src/components/Canvas.cpp \
          src/components/ColorPalette.cpp \
          src/components/Painter.cpp \
          src/components/Partida.cpp \
          src/components/WordServiceClient.cpp \
          src/components/ThemeSelector.cpp

# Default target
all: game

# Build the main game
game:
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(PROJECT_NAME).exe $(INCLUDE_PATHS) $(LDFLAGS) $(LDLIBS)

# Build the server  
server:
	$(CXX) src/server.cpp src/components/Client.cpp -o EspyntarServer.exe -lws2_32 
	
# Run server
run-server: server
	./EspyntarServer.exe

# Run game
run-game: game
	./$(PROJECT_NAME).exe

# Start Python word service
start-word-service:
	python word_service.py

# Test word service
test-word-service:
	python test_word_service.py

# Clean build files
clean:
	rm -f *.exe *.o

# Install Python dependencies  
install-deps:
	pip install -r requirements.txt

# Setup complete environment
setup: install-deps
	@echo "✅ Python dependencies installed"
	@echo "⚠️  Don't forget to configure your OpenAI API key in .env"
	@echo "🚀 Run 'make start-word-service' to start the AI service"
	@echo "🎮 Run 'make game' to build the game"

# Help
help:
	@echo "Espyntar with AI Words - Build System"
	@echo ""
	@echo "Available targets:"
	@echo "  game              - Build the main game"
	@echo "  server            - Build the server" 
	@echo "  run-server        - Build and run server"
	@echo "  run-game          - Build and run game"
	@echo "  start-word-service - Start Python AI word service"
	@echo "  test-word-service  - Test Python AI word service"
	@echo "  setup             - Install Python dependencies"
	@echo "  clean             - Remove build files"
	@echo "  help              - Show this help"
	@echo ""
	@echo "Example workflow:"
	@echo "  1. make setup"
	@echo "  2. Configure .env with OpenAI API key"
	@echo "  3. make start-word-service (in one terminal)"
	@echo "  4. make run-server (in another terminal)" 
	@echo "  5. make run-game (for each player)"

.PHONY: all game server run-server run-game start-word-service test-word-service clean install-deps setup help