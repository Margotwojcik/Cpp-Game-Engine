#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

// Definicja dla biblioteki ładowania obrazków
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// --- KLASA TEKSTURY ---
class Texture {
public:
    GLuint id;
    int width, height, nrChannels;

    Texture(const char* filePath) {
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);

        // Parametry wyświetlania
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_set_flip_vertically_on_load(true); 
        unsigned char *data = stbi_load(filePath, &width, &height, &nrChannels, 0);
        
        if (data) {
            GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            // glGenerateMipmap usunięte dla uniknięcia błędów kompilacji bez GLEW
            std::cout << "Zaladowano teksture: " << filePath << std::endl;
        } else {
            std::cerr << "Blad: Nie znaleziono pliku " << filePath << std::endl;
        }
        stbi_image_free(data);
    }

    void bind() { glBindTexture(GL_TEXTURE_2D, id); }
};

// --- KLASA RENDERERA ---
class Renderer {
public:
    void drawSprite(float x, float y, float size, Texture& tex) {
        glEnable(GL_TEXTURE_2D);
        tex.bind();
        glColor3f(1.0f, 1.0f, 1.0f); 

        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f); glVertex2f(x, y);
            glTexCoord2f(1.0f, 0.0f); glVertex2f(x + size, y);
            glTexCoord2f(1.0f, 1.0f); glVertex2f(x + size, y + size);
            glTexCoord2f(0.0f, 1.0f); glVertex2f(x, y + size);
        glEnd();
        
        glDisable(GL_TEXTURE_2D);
    }
};

// --- GŁÓWNA KLASA SILNIKA ---
class GameEngine {
private:
    GLFWwindow* window;
    Renderer renderer;
    Texture* playerTex;
    bool running;

    // Pozycja gracza
    float playerX = -0.25f;
    float playerY = -0.25f;
    float speed = 0.02f;

public:
    GameEngine() : window(nullptr), playerTex(nullptr), running(true) {}

    bool init(int width, int height, const char* title) {
        if (!glfwInit()) return false;
        window = glfwCreateWindow(width, height, title, NULL, NULL);
        if (!window) {
            glfwTerminate();
            return false;
        }
        glfwMakeContextCurrent(window);

        // Ładowanie tekstury
        playerTex = new Texture("player.png"); 

        return true;
    }

    void handleInput() {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            running = false;
        
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) playerY += speed;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) playerY -= speed;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) playerX -= speed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) playerX += speed;
    }

    void update() {
        // Tutaj można dodać logikę gry, np. kolizje
    }

    void render() {
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f); // Ciemne tło
        glClear(GL_COLOR_BUFFER_BIT);
        
        if(playerTex) {
            renderer.drawSprite(playerX, playerY, 0.5f, *playerTex);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    void run() {
        while (running && !glfwWindowShouldClose(window)) {
            handleInput();
            update();
            render();
        }
    }

    ~GameEngine() {
        if (playerTex) delete playerTex;
        glfwDestroyWindow(window);
        glfwTerminate();
    }
};

int main() {
    GameEngine engine;
    if (engine.init(800, 800, "C++ Engine - Sterowanie WSAD")) {
        engine.run();
    }
    return 0;
}