CFLAGS=-c -std=c++17 -Wall -Wextra -O3 -pedantic -ggdb -Wno-long-long
LINKER=-lSDL2 -lSDL2_image -lSDL2_ttf
ECS=./src/ECS/
BIN=./weisswal
GAME=src/Game.cpp src/Game.h src/Map.h src/TextureManager.h \
       src/ECS/Components.h src/ECS/ECS.h src/ECS/TransformComponent.h \
       src/ECS/../Vector2D.h src/ECS/SpriteComponent.h \
       src/ECS/../TextureManager.h src/ECS/Animation.h \
       src/ECS/../AssetManager.h src/Vector2D.h src/ECS/KeyboardController.h \
       src/ECS/../Game.h src/ECS/InventoryComponent.h \
       src/ECS/ColliderComponent.h src/ECS/../Collision.h \
       src/ECS/TileComponent.h src/ECS/AttackComponent.h \
       src/ECS/StatComponent.h src/ECS/UILabel.h src/Collision.h \
       src/AssetManager.h
COL=src/Collision.cpp src/Collision.h \
      src/ECS/ColliderComponent.h src/ECS/Components.h src/ECS/ECS.h \
      src/ECS/TransformComponent.h src/ECS/../Vector2D.h \
      src/ECS/SpriteComponent.h src/ECS/../TextureManager.h src/Game.h \
      src/ECS/Animation.h src/ECS/../AssetManager.h src/TextureManager.h \
      src/Vector2D.h src/ECS/KeyboardController.h src/ECS/../Game.h \
      src/ECS/InventoryComponent.h src/ECS/TileComponent.h \
      src/ECS/AttackComponent.h src/ECS/StatComponent.h src/ECS/UILabel.h \
      src/ECS/../Collision.h
MAP=src/Map.cpp src/Map.h src/Game.h src/ECS/ECS.h \
      src/ECS/Components.h src/ECS/TransformComponent.h \
      src/ECS/../Vector2D.h src/ECS/SpriteComponent.h \
      src/ECS/../TextureManager.h src/ECS/Animation.h \
      src/ECS/../AssetManager.h src/TextureManager.h src/Vector2D.h \
      src/ECS/KeyboardController.h src/ECS/../Game.h \
      src/ECS/InventoryComponent.h src/ECS/ColliderComponent.h \
      src/ECS/../Collision.h src/ECS/TileComponent.h \
      src/ECS/AttackComponent.h src/ECS/StatComponent.h src/ECS/UILabel.h
ASSETMANAGER=src/AssetManager.cpp src/AssetManager.h \
               src/TextureManager.h src/Game.h src/Vector2D.h src/ECS/ECS.h \
               src/ECS/Components.h src/ECS/TransformComponent.h \
               src/ECS/../Vector2D.h src/ECS/SpriteComponent.h \
               src/ECS/../TextureManager.h src/ECS/Animation.h \
               src/ECS/../AssetManager.h src/ECS/KeyboardController.h \
               src/ECS/../Game.h src/ECS/InventoryComponent.h \
               src/ECS/ColliderComponent.h src/ECS/../Collision.h \
               src/ECS/TileComponent.h src/ECS/AttackComponent.h \
               src/ECS/StatComponent.h src/ECS/UILabel.h

all:
	make compile; make doc;

compile: $(BIN)

clean:
	rm *.o ./weisswal; rm -Rf doc;

run: $(BIN)
	$(BIN)

doc:
	rm -Rf doc; mkdir doc; doxygen Doxyfile;

$(BIN): main.o Game.o Collision.o Map.o TextureManager.o Vector2D.o ECS.o AssetManager.o
	g++ main.o Game.o Collision.o Map.o TextureManager.o Vector2D.o ECS.o AssetManager.o $(LINKER) -o weisswal;

main.o: src/main.cpp src/Game.h
	g++ $(CFLAGS) ./src/main.cpp

Game.o: $(GAME)
	g++ $(CFLAGS) ./src/Game.cpp

Collision.o: $(COL)
	g++ $(CFLAGS) ./src/Collision.cpp

Map.o: $(MAP)
	g++ $(CFLAGS) ./src/Map.cpp

TextureManager.o: src/TextureManager.cpp src/TextureManager.h src/Game.h
	g++ $(CFLAGS) ./src/TextureManager.cpp

Vector2D.o: src/Vector2D.cpp src/Vector2D.h
	g++ $(CFLAGS) ./src/Vector2D.cpp

ECS.o: $(ECS)ECS.cpp $(ECS)ECS.h
	g++ $(CFLAGS) $(ECS)ECS.cpp

AssetManager.o: $(ASSETMANAGER)
	g++ $(CFLAGS) ./src/AssetManager.cpp

