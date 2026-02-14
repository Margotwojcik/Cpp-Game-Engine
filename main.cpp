#include <GLFW/glfw3.h> 
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace std;

const int WIDTH = 20;
const int HEIGHT = 20;
const float CELL_SIZE = 0.1f; 

struct Point {
    int x, y;
};

enum Direction { STOP = 0, LEFT, RIGHT, UP, DOWN };

vector<Point> snake;
Point fruit;
Direction dir = STOP;
bool gameOver = false;

void spawnFruit() {
    fruit.x = rand() % WIDTH;
    fruit.y = rand() % HEIGHT;
}

void initGame() {
    srand(static_cast<unsigned int>(time(0)));
    snake.clear();
    // Startujemy na środku
    snake.push_back({WIDTH / 2, HEIGHT / 2});
    dir = STOP;
    spawnFruit();
}

void update() {
    if (dir == STOP || gameOver) return;

    // Zapamiętujemy pozycję głowy przed ruchem
    Point prev = snake[0];
    
    // Ruch głowy
    switch(dir) {
        case LEFT:  snake[0].x--; break;
        case RIGHT: snake[0].x++; break;
        case UP:    snake[0].y++; break;
        case DOWN:  snake[0].y--; break;
        default: break;
    }

    // Kolizja ze ścianami
    if (snake[0].x < 0 || snake[0].x >= WIDTH || snake[0].y < 0 || snake[0].y >= HEIGHT) {
        gameOver = true;
        return;
    }

    // Ruch ogona
    for (size_t i = 1; i < snake.size(); i++) {
        Point temp = snake[i];
        snake[i] = prev;
        prev = temp;
    }

    // Kolizja z samym sobą
    for (size_t i = 1; i < snake.size(); i++) {
        if (snake[i].x == snake[0].x && snake[i].y == snake[0].y) {
            gameOver = true;
        }
    }

    // Zjadanie owocu
    if (snake[0].x == fruit.x && snake[0].y == fruit.y) {
        // Dodajemy nowy segment na końcu (pozycja prev po pętli to ostatni ogon)
        snake.push_back(prev); 
        spawnFruit();
    }
}

void drawCell(int x, int y) {
    // Przeliczenie współrzędnych siatki (0-20) na układ OpenGL (-1.0 do 1.0)
    float xf = -1.0f + (x * (2.0f / WIDTH));
    float yf = -1.0f + (y * (2.0f / HEIGHT));
    float w = 2.0f / WIDTH;
    float h = 2.0f / HEIGHT;

    glBegin(GL_QUADS);
        glVertex2f(xf, yf);
        glVertex2f(xf + w, yf);
        glVertex2f(xf + w, yf + h);
        glVertex2f(xf, yf + h);
    glEnd();
}

void draw() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Rysuj owoc (Czerwony)
    glColor3f(1.0f, 0.0f, 0.0f);
    drawCell(fruit.x, fruit.y);

    // Rysuj węża (Zielony)
    glColor3f(0.0f, 1.0f, 0.0f);
    for (auto const& s : snake)
        drawCell(s.x, s.y);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch(key) {
            // Dodana blokada zawracania o 180 stopni
            case GLFW_KEY_W: if(dir != DOWN) dir = UP; break;
            case GLFW_KEY_S: if(dir != UP) dir = DOWN; break;
            case GLFW_KEY_A: if(dir != RIGHT) dir = LEFT; break;
            case GLFW_KEY_D: if(dir != LEFT) dir = RIGHT; break;
            case GLFW_KEY_Q: gameOver = true; break;
        }
    }
}

int main() {
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(600, 600, "Snake OpenGL - Sterowanie WSAD", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyCallback);

    initGame();

    double lastTime = glfwGetTime();
    const double frameDelay = 0.15; 

    while (!glfwWindowShouldClose(window) && !gameOver) {
        double currentTime = glfwGetTime();
        if (currentTime - lastTime >= frameDelay) {
            update();
            lastTime = currentTime;
        }

        draw();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    if (gameOver) cout << "GAME OVER! Wynik: " << snake.size() << endl;

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}