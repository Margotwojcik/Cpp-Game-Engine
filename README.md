# Simple 2D C++ Game Engine

A lightweight, educational 2D game engine built from scratch using **C++** and **OpenGL (GLFW)**. This project demonstrates core game engine concepts such as state management, texture loading, VSync synchronization, and basic collision physics.



## Features

* **Multi-Game Support:** A built-in state machine allowing users to switch between games (Snake and Pong) from a central menu.
* **Custom Renderer:** A simplified OpenGL wrapper for drawing primitives (quads) and textures.
* **VSync Synchronization:** Smooth gameplay fixed to the monitor's refresh rate (60 FPS) to prevent high CPU usage and physics glitches.
* **Texture Loading:** Integrated `stb_image` for loading external assets like menu backgrounds.
* **Real-time Input:** Responsive controls for both single-player (Snake) and local multiplayer (Pong).

## Included Games

### 1. Snake
* **Objective:** Eat the red food to grow and increase your score.
* **Controls:** Arrow Keys to move.
* **Lose Condition:** Hitting the screen boundaries or yourself.

### 2. Pong
* **Objective:** A classic 2-player game of hitting the ball past the opponent.
* **Controls:** * Left Player: `W` / `S`
    * Right Player: `UP` / `DOWN`
* **Score:** Tracked in the window title bar.

## Requirements & Setup

### Prerequisites
* **Compiler:** MinGW (g++) or any C++ compiler.
* **Libraries:** [GLFW](https://www.glfw.org/) (included or linked).
* **Headers:** `stb_image.h` (place in the project root).

### Assets
Ensure the following files are in your project directory:
* `menu.png` (800x600 background for the main menu).
* `stb_image.h` (for image processing).

### Compilation (Windows/PowerShell)
```powershell
g++ main.cpp -o game_engine.exe -lglfw3 -lopengl32 -lgdi32 -static
