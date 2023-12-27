#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include <unordered_map>
const int TILE_SIZE = 32;



class GameMap {
public:
    // Constructor to initialize the map with the provided data
    GameMap(const std::vector<std::vector<int>>& data) : mapData(data) {}
    size_t getNumRows() const {
        return mapData.size();
    }

    size_t getNumCols() const {
        return (mapData.empty()) ? 0 : mapData[0].size();
    }
    // Method to get the value at a specific position in the map
    int getValueAt(int row, int col) const {
        // Check if the provided indices are within bounds
        if (row >= 0 && row < mapData.size() && col >= 0 && col < mapData[0].size()) {
            return mapData[row][col];
        }
        return -1; // Return a special value (you might want to handle this differently)
    }

    // Method to set the value at a specific position in the map
    void setValueAt(int row, int col, int value) {
        // Check if the provided indices are within bounds
        if (row >= 0 && row < mapData.size() && col >= 0 && col < mapData[0].size()) {
            mapData[row][col] = value;
        }
        // You might want to add error handling for out-of-bounds access
    }

private:
    std::vector<std::vector<int>> mapData;
};

class GameObject {
public:
    virtual ~GameObject() = default;
};

class UpdatableObject : virtual public GameObject {
public:
    virtual void update() = 0;
};

class DrawableObject : virtual public GameObject {
public:
    virtual void draw(SDL_Renderer* renderer) const = 0;
};

class TransformableObject : virtual public GameObject {
public:
    virtual void translate(float tx, float ty) = 0;
    virtual void rotate(float angle) = 0;
    virtual void scale(float factor) = 0;
};

class ShapeObject : public DrawableObject, public TransformableObject {
public:
    virtual void setSourceRect(int x, int y, int w, int h) = 0;  // Added missing method
    virtual void update() = 0;
};

class Circle : public ShapeObject {
public:
    SDL_Texture* texture;
    float x, y, radius;

    Circle(SDL_Renderer* renderer, const char* imagePath, float _x, float _y, float _radius)
        : x(_x), y(_y), radius(_radius) {
        texture = loadTexture(renderer, imagePath);
    }

    void draw(SDL_Renderer* renderer) const override {
        SDL_Rect destRect = { static_cast<int>(x - radius), static_cast<int>(y - radius),
                              static_cast<int>(radius * 2), static_cast<int>(radius * 2) };

        SDL_RenderCopy(renderer, texture, nullptr, &destRect);
    }

    void translate(float tx, float ty) override {
        x += tx;
        y += ty;
    }

    void rotate(float angle) override {
        // Implement rotation for a circle if needed
    }

    void scale(float factor) override {
        radius *= factor;
    }

    void setSourceRect(int x, int y, int w, int h) override {
        // Implement setSourceRect for Circle
    }

private:
    SDL_Texture* loadTexture(SDL_Renderer* renderer, const char* imagePath) {
        SDL_Surface* surface = IMG_Load(imagePath);
        if (!surface) {
            std::cerr << "IMG_Load Error: " << IMG_GetError() << std::endl;
            return nullptr;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        if (!texture) {
            std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
            return nullptr;
        }

        return texture;
    }
};
class MyImage : public DrawableObject {
private:
    SDL_Texture* texture;
    float x, y;

public:
    MyImage(SDL_Renderer* renderer, const char* imagePath, float _x, float _y)
        : x(_x), y(_y) {
        texture = loadTexture(renderer, imagePath);
    }

    void draw(SDL_Renderer* renderer) const override {
        SDL_Rect destRect = { static_cast<int>(x), static_cast<int>(y), /* Adjust size as needed */ };

        SDL_RenderCopy(renderer, texture, nullptr, &destRect);
    }

private:
    SDL_Texture* loadTexture(SDL_Renderer* renderer, const char* imagePath) const {
        SDL_Surface* surface = IMG_Load(imagePath);
        if (!surface) {
            std::cerr << "IMG_Load Error: " << IMG_GetError() << std::endl;
            return nullptr; // Return a value in case of an error
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        if (!texture) {
            std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
            return nullptr; // Return a value in case of an error
        }

        return texture; // Return the texture on success
    }
};

class Player : public Circle {
private:
    static const int numLeftAnimationFrames = 9;
    static const int numRightAnimationFrames = 9;
    static const int numUpAnimationFrames = 9;
    static const int numDownAnimationFrames = 9;
    GameMap& gameMap;
public:
    Player(SDL_Renderer* renderer, const char* imagePath, float _x, float _y, float _radius, GameMap& _gameMap)
        : Circle(renderer, imagePath, _x, _y, _radius), gameMap(_gameMap) {
        loadAnimations(renderer);
    }

    void draw(SDL_Renderer* renderer) const override {
        SDL_Rect destRect = { static_cast<int>(x - radius), static_cast<int>(y - radius),
                              static_cast<int>(radius * 2), static_cast<int>(radius * 2) };
        SDL_RenderCopy(renderer, currentAnimation->getCurrentFrame(), nullptr, &destRect);
    }

    void update() override {
        const float moveDistance = 5.0f;
        float newX = x;
        float newY = y;

        if (movingLeft) {
            newX = x - moveDistance;
            currentAnimation = &animations["left"];
        }
        else if (movingRight) {
            newX = x + moveDistance;
            currentAnimation = &animations["right"];
        }
        else if (movingUp) {
            newY = y - moveDistance;
            currentAnimation = &animations["up"];
        }
        else if (movingDown) {
            newY = y + moveDistance;
            currentAnimation = &animations["down"];
        }
        else {
            currentAnimation = &animations["idle"];
        }

        // Check if the new position is valid (not colliding with a wall)
        if (!isCollidingWithWalls(newX, newY)) {
            x = newX;
            y = newY;
        }

        currentAnimation->update();
    }

    bool isCollidingWithWalls(float newX, float newY) {
        // Calculate the player's bounding box
        SDL_Rect playerRect = {
            static_cast<int>(newX - radius),
            static_cast<int>(newY - radius),
            static_cast<int>(radius * 2),
            static_cast<int>(radius * 2)
        };

        // Iterate over each wall in the game map
        for (int row = 0; row < gameMap.getNumRows(); ++row) {
            for (int col = 0; col < gameMap.getNumCols(); ++col) {
                int tileValue = gameMap.getValueAt(row, col);
                if (tileValue == 1) {
                    SDL_Rect wallRect = { col * TILE_SIZE, row * TILE_SIZE, TILE_SIZE, TILE_SIZE };

                    // Check for collision
                    if (SDL_HasIntersection(&playerRect, &wallRect)) {
                        return true; // Collision detected with this wall
                    }
                }
            }
        }

        return false; // No collision detected with any wall
    }
    bool movingLeft = false;
    bool movingRight = false;
    bool movingUp = false;
    bool movingDown = false;

private:
    struct Animation {
        std::vector<SDL_Texture*> frames;
        size_t currentFrame = 0;
        size_t frameDelay;
        size_t delayCounter = 0;

        void update() {
            if (delayCounter < frameDelay) {
                delayCounter++;
            }
            else {
                delayCounter = 0;
                currentFrame = (currentFrame + 1) % frames.size();
            }
        }

        SDL_Texture* getCurrentFrame() const {
            return frames[currentFrame];
        }
    };

    std::unordered_map<std::string, Animation> animations;
    Animation* currentAnimation = nullptr;

    void loadAnimations(SDL_Renderer* renderer) {
        animations["idle"].frames.push_back(loadTexture(renderer, "assets/player.png"));

        // Load left animation frames
        for (int i = 0; i < numLeftAnimationFrames; ++i) {
            animations["left"].frames.push_back(loadTexture(renderer, "assets/player_walk_left.png"));
        }

        // Load right animation frames
        for (int i = 0; i < numRightAnimationFrames; ++i) {
            animations["right"].frames.push_back(loadTexture(renderer, "assets/player_walk_right.png"));
        }

        // Load up animation frames
        for (int i = 0; i < numUpAnimationFrames; ++i) {
            animations["up"].frames.push_back(loadTexture(renderer, "assets/player_walk_up.png"));
        }

        // Load down animation frames
        for (int i = 0; i < numDownAnimationFrames; ++i) {
            animations["down"].frames.push_back(loadTexture(renderer, "assets/player_walk_down.png"));
        }
    }

    SDL_Texture* loadTexture(SDL_Renderer* renderer, const char* imagePath) const {
        SDL_Surface* surface = IMG_Load(imagePath);
        if (!surface) {
            std::cerr << "IMG_Load Error: " << IMG_GetError() << std::endl;
            return nullptr;
        }

        // Slice the loaded texture based on animation direction
        SDL_Texture* texture = nullptr;
        if (std::strstr(imagePath, "left") != nullptr &&
            std::strstr(imagePath, "right") != nullptr &&
            std::strstr(imagePath, "up") != nullptr &&
            std::strstr(imagePath, "down") != nullptr) {
            int numFrames = 0;
            if (std::strstr(imagePath, "left") != nullptr) {
                numFrames = numLeftAnimationFrames;
            }
            else if (std::strstr(imagePath, "right") != nullptr) {
                numFrames = numRightAnimationFrames;
            }
            else if (std::strstr(imagePath, "up") != nullptr) {
                numFrames = numUpAnimationFrames;
            }
            else if (std::strstr(imagePath, "down") != nullptr) {
                numFrames = numDownAnimationFrames;
            }

            SDL_Rect sliceRect = { 0, 0, surface->w / numFrames, surface->h };
            SDL_Surface* slicedSurface = SDL_CreateRGBSurface(0, sliceRect.w, sliceRect.h, 32, 0, 0, 0, 0);
            SDL_BlitSurface(surface, &sliceRect, slicedSurface, nullptr);

            texture = SDL_CreateTextureFromSurface(renderer, slicedSurface);
            SDL_FreeSurface(slicedSurface);
        }
        else {
            texture = SDL_CreateTextureFromSurface(renderer, surface);
        }

        SDL_FreeSurface(surface);

        if (!texture) {
            std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
            return nullptr;
        }

        return texture;
    }
};


class Engine {
public:
    SDL_Window* window;
    SDL_Renderer* renderer;
    Player* player;
    MyImage* myImage;
    GameMap gameMap; // Add an instance of the GameMap

    Engine() : player(nullptr), gameMap({}), myImage(nullptr) {
        if (!initGraphics()) {
            std::cerr << "Failed to initialize graphics library" << std::endl;
        }

        // Initialize the GameMap with the provided map data
        gameMap = GameMap({
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
            { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } });

        player = new Player(renderer, "assets/player.png", 100.0f, 100.0f, 20.0f, gameMap);
        myImage = new MyImage(renderer, "assets/circle .png", 200.0f, 200.0f, gameMap);

    }

    ~Engine() {
        delete player;
        delete myImage;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    bool initGraphics() {
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
            return false;
        }

        int imgFlags = IMG_INIT_PNG;
        if (!(IMG_Init(imgFlags) & imgFlags)) {
            std::cerr << "SDL_image Init Error: " << IMG_GetError() << std::endl;
            return false;
        }

        window = SDL_CreateWindow("Game Window", SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);

        if (window == nullptr) {
            std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
            return false;
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

        if (renderer == nullptr) {
            std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
            return false;
        }

        return true;
    }

    void run() {
        SDL_Event event;
        bool isRunning = true;
        while (isRunning) {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    isRunning = false;
                }
                else if (event.type == SDL_KEYDOWN) {
                    handleKeyPress(event.key.keysym.sym);
                }
                else if (event.type == SDL_KEYUP) {
                    handleKeyRelease(event.key.keysym.sym);
                }
            }
            update();
            drawObjects();
        }
    }

    void update() {
        player->update();
        // Additional game logic update if needed
    }

    void drawObjects() {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw the game map
        for (int row = 0; row < gameMap.getNumRows(); ++row) {
            for (int col = 0; col < gameMap.getNumCols(); ++col) {
                int tileValue = gameMap.getValueAt(row, col);
                if (tileValue == 1) {
                    // Draw a wall or obstacle
                    SDL_Rect destRect = { col * TILE_SIZE, row * TILE_SIZE, TILE_SIZE, TILE_SIZE };
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    SDL_RenderFillRect(renderer, &destRect);
                }
                // Add more conditions for different tile values if needed
            }
        }

        player->draw(renderer);
        myImage->draw(renderer); // Draw your additional image
        SDL_RenderPresent(renderer);
    }

    void handleKeyPress(SDL_Keycode key) {
        handleKeyState(key, true);
    }

    void handleKeyRelease(SDL_Keycode key) {
        handleKeyState(key, false);
    }

    void handleKeyState(SDL_Keycode key, bool pressed) {
        switch (key) {
        case SDLK_LEFT:
            player->movingLeft = pressed;
            break;
        case SDLK_RIGHT:
            player->movingRight = pressed;
            break;
        case SDLK_UP:
            player->movingUp = pressed;
            break;
        case SDLK_DOWN:
            player->movingDown = pressed;
            break;
        }
    }
};

int main(int argc, char* argv[]) {
    Engine engine;
    engine.run();
    return 0;
}



