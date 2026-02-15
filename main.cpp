#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <string>

enum GameState { MENU, SNAKE_GAME, PONG_GAME, GAME_OVER };
GameState currentState = MENU;

// --- LOGIKA SNAKE ---
struct Point { int x, y; };
std::vector<Point> snake = {{10, 10}, {10, 11}, {10, 12}};
Point food = {5, 5};
int dirX = 0, dirY = -1;
int snakeScore = 0;

// --- LOGIKA PONG ---
float paddleL_Y = 0.0f;
float paddleR_Y = 0.0f;
float ballX = 0.0f, ballY = 0.0f;
float ballSpeedX = 0.015f; // Zmniejszona prędkość bazowa
float ballSpeedY = 0.012f;
int scoreL = 0, scoreR = 0;

class Renderer {
public:
    void drawQuad(float x, float y, float w, float h, float r, float g, float b) {
        glColor3f(r, g, b);
        glBegin(GL_QUADS);
            glVertex2f(x, y);
            glVertex2f(x + w, y);
            glVertex2f(x + w, y + h);
            glVertex2f(x, y + h);
        glEnd();
    }
};

class GameEngine {
private:
    GLFWwindow* window;
    Renderer renderer;
    bool running;
    double lastUpdate;

public:
    GameEngine() : window(nullptr), running(true), lastUpdate(0.0) {}

    bool init(int w, int h, const char* t) {
        if (!glfwInit()) return false;
        window = glfwCreateWindow(w, h, t, NULL, NULL);
        if (!window) return false;
        glfwMakeContextCurrent(window);

        // KLUCZOWA LINIA: Włącza VSync (limit klatek do odświeżania monitora)
        // To sprawi, że pętla nie będzie lecieć "jak szalona"
        glfwSwapInterval(1); 

        lastUpdate = glfwGetTime();
        return true;
    }

    void updateTitle() {
        std::string title;
        if (currentState == SNAKE_GAME) 
            title = "Snake - Score: " + std::to_string(snakeScore);
        else if (currentState == PONG_GAME) 
            title = "Pong - Left: " + std::to_string(scoreL) + " | Right: " + std::to_string(scoreR);
        else 
            title = "Multigame Engine";
        
        glfwSetWindowTitle(window, title.c_str());
    }

    void handleInput() {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) running = false;
        if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) currentState = MENU;

        if (currentState == MENU || currentState == GAME_OVER) {
            if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
                snake = {{10, 10}, {10, 11}, {10, 12}};
                dirX = 0; dirY = -1; snakeScore = 0;
                currentState = SNAKE_GAME;
            }
            if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
                scoreL = 0; scoreR = 0; ballX = 0; ballY = 0;
                currentState = PONG_GAME;
            }
        } 
        else if (currentState == SNAKE_GAME) {
            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && dirY != -1) { dirX = 0; dirY = 1; }
            if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && dirY != 1) { dirX = 0; dirY = -1; }
            if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && dirX != 1) { dirX = -1; dirY = 0; }
            if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && dirX != -1) { dirX = 1; dirY = 0; }
        }
        else if (currentState == PONG_GAME) {
            // Bardzo mała wartość ruchu, teraz przy VSync będzie płynna
            float pSpeed = 0.02f; 
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && paddleL_Y < 0.75f) paddleL_Y += pSpeed;
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && paddleL_Y > -0.75f) paddleL_Y -= pSpeed;
            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && paddleR_Y < 0.75f) paddleR_Y += pSpeed;
            if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && paddleR_Y > -0.75f) paddleR_Y -= pSpeed;
        }
    }

   void updateSnake() {
        Point newHead = {snake[0].x + dirX, snake[0].y + dirY};
        if (newHead.x < 0 || newHead.x >= 20 || newHead.y < 0 || newHead.y >= 20) {
            currentState = GAME_OVER; return;
        }
        for (auto& p : snake) {
            if (newHead.x == p.x && newHead.y == p.y) { currentState = GAME_OVER; return; }
        }
        snake.insert(snake.begin(), newHead);
        if (newHead.x == food.x && newHead.y == food.y) {
            snakeScore += 10;
            food = { rand() % 20, rand() % 20 };
            updateTitle();
        } else {
            snake.pop_back();
        }
    }

    void updatePong() {
        ballX += ballSpeedX;
        ballY += ballSpeedY;

        if (ballY > 0.95f || ballY < -0.95f) ballSpeedY = -ballSpeedY;

        // Kolizje z paletkami
        if (ballX < -0.85f && ballY < paddleL_Y + 0.25f && ballY > paddleL_Y - 0.25f) ballSpeedX = std::abs(ballSpeedX);
        if (ballX > 0.85f && ballY < paddleR_Y + 0.25f && ballY > paddleR_Y - 0.25f) ballSpeedX = -std::abs(ballSpeedX);

        if (ballX < -1.0f) { scoreR++; ballX = 0; ballY = 0; }
        if (ballX > 1.0f) { scoreL++; ballX = 0; ballY = 0; }

        std::string t = "Pong - L: " + std::to_string(scoreL) + " R: " + std::to_string(scoreR);
        glfwSetWindowTitle(window, t.c_str());
    }

    void render() {
        glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        if (currentState == MENU) {
            renderer.drawQuad(-0.5f, -0.2f, 0.4f, 0.4f, 0.0f, 0.8f, 0.0f); 
            renderer.drawQuad(0.1f, -0.2f, 0.4f, 0.4f, 0.0f, 0.4f, 0.8f);
        } 
        else if (currentState == SNAKE_GAME) {
            renderer.drawQuad(food.x/10.0f - 1.0f, food.y/10.0f - 1.0f, 0.09f, 0.09f, 1.0f, 0.0f, 0.0f);
            for (auto& p : snake) renderer.drawQuad(p.x/10.0f - 1.0f, p.y/10.0f - 1.0f, 0.09f, 0.09f, 0.0f, 1.0f, 0.0f);
        }
        else if (currentState == PONG_GAME) {
            renderer.drawQuad(-0.9f, paddleL_Y - 0.2f, 0.05f, 0.4f, 1.0f, 1.0f, 1.0f);
            renderer.drawQuad(0.85f, paddleR_Y - 0.2f, 0.05f, 0.4f, 1.0f, 1.0f, 1.0f);
            renderer.drawQuad(ballX, ballY, 0.04f, 0.04f, 1.0f, 1.0f, 0.0f);
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    void run() {
        while (running && !glfwWindowShouldClose(window)) {
            handleInput();
            double currentTime = glfwGetTime();
            if (currentState == SNAKE_GAME) {
                if (currentTime - lastUpdate >= 0.12) { updateSnake(); lastUpdate = currentTime; }
            } else if (currentState == PONG_GAME) {
                updatePong(); 
            }
            render();
        }
    }
};

int main() {
    GameEngine engine;
    if (engine.init(800, 600, "Multigame Engine")) {
        std::cout << "1 - Snake, 2 - Pong, M - Menu" << std::endl;
        engine.run();
    }
    return 0;
}