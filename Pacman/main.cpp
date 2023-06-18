#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#define MAZE_WIDTH 20
#define MAZE_HEIGHT 15
#define CELL_SIZE 40
#define PLAYER_SPEED 0.3
#define PLAYER_START_X (MAZE_WIDTH / 2)
#define PLAYER_START_Y (MAZE_HEIGHT / 2)
#define MOUTH_ANGLE_MIN 45
#define MOUTH_ANGLE_MAX 315
int score = 0;

// Delay in milliseconds for dot disappearance
#define DOT_DISAPPEAR_DELAY 5000

int maze[MAZE_HEIGHT][MAZE_WIDTH] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1},
    {1,0,1,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,1,0,1,0,1,1,1,1,0,1,1,0,1},
    {1,0,1,0,1,1,0,1,0,1,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,0,1,1,0,1,0,0,0,1,1,1,1,0,0,1,0,1},
    {1,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0,1},
    {1,0,1,1,1,1,1,0,1,1,0,1,0,1,1,1,0,1,0,1},
    {1,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,1,0,1},
    {1,1,1,1,1,0,1,1,1,1,0,1,1,1,0,0,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,1,1,0,1,1,1,1,1,1,0,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};


int playerX = PLAYER_START_X;
int playerY = PLAYER_START_Y;
int ghostX=1 ;
int ghostY=1;
int moveDirection = 0;
bool moving = false;
int mouthAngle = MOUTH_ANGLE_MIN;

void init() {
    glClearColor(0, 0, 0, 0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, MAZE_WIDTH * CELL_SIZE, 0, MAZE_HEIGHT * CELL_SIZE);
    srand(time(NULL));
}

void drawMaze() {
    int i, j;
    for (i = 0; i < MAZE_HEIGHT; i++) {
        for (j = 0; j < MAZE_WIDTH; j++) {
            if (maze[i][j] == 1) {
                glColor3f(0.2, 0.2, 0.8);
                glBegin(GL_QUADS);
                glVertex2f(j * CELL_SIZE, i * CELL_SIZE);
                glVertex2f(j * CELL_SIZE + CELL_SIZE, i * CELL_SIZE);
                glVertex2f(j * CELL_SIZE + CELL_SIZE, i * CELL_SIZE + CELL_SIZE);
                glVertex2f(j * CELL_SIZE, i * CELL_SIZE + CELL_SIZE);
                glEnd();
            } else if (maze[i][j] == 2) {
                glColor3f(1.0, 0.0, 0.0);
                glPointSize(3.0);
                glBegin(GL_POINTS);
                glVertex2f(j * CELL_SIZE + CELL_SIZE / 2, i * CELL_SIZE + CELL_SIZE / 2);
                glEnd();
            } else if (maze[i][j] == 0) {
                glColor3f(1.0, 0.0, 0.0);
                glPointSize(3.0);
                glBegin(GL_POINTS);
                glVertex2f(j * CELL_SIZE + CELL_SIZE / 2, i * CELL_SIZE + CELL_SIZE / 2);
                glEnd();
            }
        }
        printf("\n");
    }
}


void drawPlayer() {
    float x = playerX * CELL_SIZE + CELL_SIZE / 2.0;
    float y = playerY * CELL_SIZE + CELL_SIZE / 2.0;
    float radius = CELL_SIZE / 2.0 - 2.0;
    float mouthAngle = (playerX % 2 == 0) ? MOUTH_ANGLE_MIN : MOUTH_ANGLE_MAX;

    glColor3f(1.0, 1.0, 0.0);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);

    for (int angle = mouthAngle; angle <= 360 - mouthAngle; angle += 5) {
        float radians = angle * 3.14159 / 180.0;
        float dx = radius * cos(radians);
        float dy = radius * sin(radians);
        glVertex2f(x + dx, y + dy);
    }

    glEnd();

    // Draw the eye
    float eyeRadius = radius * 0.2;
    float eyeX = x + radius * 0.35;
    float eyeY = y + radius * 0.7;
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(eyeX, eyeY);

    for (int angle = 0; angle <= 360; angle += 5) {
        float radians = angle * 3.14159 / 180.0;
        float dx = eyeRadius * cos(radians);
        float dy = eyeRadius * sin(radians);
        glVertex2f(eyeX + dx, eyeY + dy);
    }

    glEnd();
}

void drawGhost(float px,float py) {

    float x = ghostX * CELL_SIZE + CELL_SIZE / 2.0;
    float y = ghostY * CELL_SIZE + CELL_SIZE / 2.0;
    float radius = CELL_SIZE / 2.0 - 2.0;

    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);

    for (int angle = 0; angle <= 360; angle += 5) {
        float radians = angle * 3.14159 / 180.0;
        float dx = radius * cos(radians);
        float dy = radius * sin(radians);
        glVertex2f(x + dx, y + dy);
    }

    glEnd();

    // Draw the eye
    float eyeRadius = radius * 0.2;
    float eyeX = x + radius * 0.35;
    float eyeY = y - radius * 0.4;
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(eyeX, eyeY);

    for (int angle = 0; angle <= 360; angle += 5) {
        float radians = angle * 3.14159 / 180.0;
        float dx = eyeRadius * cos(radians);
        float dy = eyeRadius * sin(radians);
        glVertex2f(eyeX + dx, eyeY + dy);
    }

    glEnd();
}

void draw() {
    glClear(GL_COLOR_BUFFER_BIT);

    drawMaze();
    drawPlayer();
    drawGhost(1,1);

    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 's':
            moveDirection = 0; // Down
            moving = true;
            break;
        case 'w':
            moveDirection = 1; // Up
            moving = true;
            break;
        case 'a':
            moveDirection = 2; // Left
            moving = true;
            break;
        case 'd':
            moveDirection = 3; // Right
            moving = true;
            break;
    }
}

void keyboardUp(unsigned char key, int x, int y) {
    moving = false;
}

void timer(int value) {
    static int counter = 0;
    counter++;

    if (counter % (int)(1.0 / PLAYER_SPEED) == 0) {
        if (moving) {
            int nextX = playerX;
            int nextY = playerY;

            switch (moveDirection) {
                case 0: // Up
                    nextY -= 1;
                    break;
                case 1: // Down
                    nextY += 1;
                    break;
                case 2: // Left
                    nextX -= 1;
                    break;
                case 3: // Right
                    nextX += 1;
                    break;
            }

            if (maze[nextY][nextX] == 0) {
                playerX = nextX;
                playerY = nextY;
            }
        }

        // Update ghost position
        int ghostMoveDirection = rand() % 4; // Random direction
        int nextGhostX = ghostX;
        int nextGhostY = ghostY;

        switch (ghostMoveDirection) {
            case 0: // Up
                nextGhostY -= 1;
                break;
            case 1: // Down
                nextGhostY += 1;
                break;
            case 2: // Left
                nextGhostX -= 1;
                break;
            case 3: // Right
                nextGhostX += 1;
                break;
        }

        if (maze[nextGhostY][nextGhostX] == 0) {
            ghostX = nextGhostX;
            ghostY = nextGhostY;
        }

        counter = 0;
    }

    glutPostRedisplay();
    glutTimerFunc(1000 / 60, timer, 0);
}


void reshape(int width, int height) {
    glViewport(0, 0, width, height);
}

bool checkCollision() {
    if (playerX == ghostX && playerY == ghostY)
        return true;
    return false;
}

void displayGameOver() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1, 1, 1);
    glRasterPos2f(MAZE_WIDTH * CELL_SIZE / 2 - 50, MAZE_HEIGHT * CELL_SIZE / 2);
    char* gameOverText = "Game Over!";
    for (int i = 0; i < 10; i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, gameOverText[i]);
    }
    glutSwapBuffers();
}

void checkGameStatus() {
    if (checkCollision()) {
        glutDisplayFunc(displayGameOver);
        glutPostRedisplay();
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(MAZE_WIDTH * CELL_SIZE, MAZE_HEIGHT * CELL_SIZE);
    glutCreateWindow("Pacman Game");
    init();
    glutDisplayFunc(draw);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();
    return 0;
}
