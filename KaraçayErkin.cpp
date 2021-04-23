/*********
   CTIS164 - Template Source Program
----------
STUDENT : Suhpi Erkin Karaçay
SECTION : 002
HOMEWORK: #3
----------
PROBLEMS: 
----------
ADDITIONAL FEATURES: 
o F: Switch between fire modes: Singular or Double
|-> Double has a little problem: Does not follow guns' angles on axis
o W/S: Increase/Decrease fire rate
o O/P: Increase/Decrease fire speed
o [1-4]: Switch between preset difficulty modes
*********/

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>

#define WINDOW_WIDTH  1200
#define WINDOW_HEIGHT  800

#define TIMER_PERIOD    16 // Period for the timer.
#define TIMER_ON         1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532	   // Degree to Radian conversion

#define MAX_FIRE 100
#define SINGULAR 1
#define DOUBLE   2

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height

typedef struct {
    double r, g, b;
}color_t;
typedef struct {
    double x, y;
}point_t;
typedef struct {
    point_t pos;      //Player's position on Y-Axis
    double angle;        //Player's angle
}player_t;
typedef struct {
    point_t pos;
    color_t color;
    double radius;
    bool hit;
    double angle;
}ufo_t;
typedef struct {
    double x;
    bool hit;
}enemy2_t;
typedef struct {
    point_t pos;
    float angle;
    bool active;
}fire_t;

player_t p = { {0,-250}, 0 };
ufo_t ufo = { {-150, 150}, {}, 20, false, 0 };
enemy2_t ship = { -240, false };
fire_t fr[MAX_FIRE];
point_t c;
bool spacebar = false,
     extraComs = false;
int fire_rate = 0,
    mode = SINGULAR,
    FIRERATE = 15,
    difficultyU = 3,
    difficultyS = 7,
    fSpeed = 8;

//
// to draw circle, center at (x,y)
// radius r
//
void circle(int x, int y, int r)
{
#define PI 3.1415
    float angle;
    glBegin(GL_POLYGON);
    for (int i = 0; i < 100; i++)
    {
        angle = 2 * PI * i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
    float angle;

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 100; i++)
    {
        angle = 2 * PI * i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

void print(int x, int y, const char* string, void* font)
{
    int len, i;

    glRasterPos2f(x, y);
    len = (int)strlen(string);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, string[i]);
    }
}

// display text with variables.
// vprint(-winWidth / 2 + 10, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "ERROR: %d", numClicks);
void vprint(int x, int y, void* font, const char* string, ...)
{
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf_s(str, string, ap);
    va_end(ap);

    int len, i;
    glRasterPos2f(x, y);
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, str[i]);
    }
}

// vprint2(-50, 0, 0.35, "00:%02d", timeCounter);
void vprint2(int x, int y, float size, const char* string, ...) {
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf_s(str, string, ap);
    va_end(ap);
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(size, size, 1);

    int len, i;
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
    {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
    }
    glPopMatrix();
}

void vertex(point_t P, point_t Tr, double angle) {
    float xp = (P.x * cos(angle) - P.y * sin(angle)) + Tr.x;
    float yp = (P.x * sin(angle) + P.y * cos(angle)) + Tr.y;
    glVertex2f(xp, yp);
}

void drawBG() {
    glColor3ub(0, 0, 50);
    glBegin(GL_POLYGON);
    glVertex2f(-winWidth / 2, winHeight / 2);
    glVertex2f(winWidth / 2, winHeight / 2);
    glColor3ub(71, 0, 122);
    glVertex2f(winWidth / 2, -winHeight / 2);
    glVertex2f(-winWidth / 2, -winHeight / 2);
    glEnd();

    glColor3ub(91, 0, 122);
    for (int i = -winWidth / 2 + 45; i <= winWidth / 2 - 50; i += 150)
        vprint2(i, 350, 0.3, "*");
    for (int i = -winWidth / 2 + 55; i <= winWidth / 2 - 50; i += 120)
        vprint2(i, 307.5, .2, "*");
    for (int i = -winWidth / 2 + 25; i <= winWidth / 2 - 50; i += 120)
        vprint2(i, 250, .3, "*");
    for (int i = -winWidth / 2 + 45; i <= winWidth / 2 - 50; i += 90)
        vprint2(i, 200, .2, "*");
    for (int i = -winWidth / 2 + 55; i <= winWidth / 2 - 50; i += 125)
        vprint2(i, 150, .3, "*");
    for (int i = -winWidth / 2 + 25; i <= winWidth / 2 - 50; i += 125)
        vprint2(i, 100, .2, "*");
}

void infoBar() {
    if (!extraComs) { //Base Commands
        glColor3ub(25, 25, 25);
        glRectf(-winWidth / 2, -winHeight / 2 + 25, winWidth / 2, -winHeight / 2);

        glColor3f(1, 1, 1);
        vprint(-540, -winHeight / 2 + 8, GLUT_BITMAP_9_BY_15, "Base Commands:");
        vprint(-345, -winHeight / 2 + 8, GLUT_BITMAP_9_BY_15, "<Spacebar> Fire");
        vprint(-155, -winHeight / 2 + 8, GLUT_BITMAP_9_BY_15, "<F1> Reset");
        vprint(-15, -winHeight / 2 + 8, GLUT_BITMAP_9_BY_15, "<Up/Down> Aim Gun");
        vprint(185, -winHeight / 2 + 8, GLUT_BITMAP_9_BY_15, "<Left/Right> Move Ship");
    }
    else { //With Extra Commands
        glColor3ub(25, 25, 25);
        glRectf(-winWidth / 2, -winHeight / 2 + 50, winWidth / 2, -winHeight / 2);

        glColor3f(1, 1, 1);
        vprint(-540, -winHeight / 2 + 33, GLUT_BITMAP_9_BY_15, "Base Commands:");
        vprint(-345, -winHeight / 2 + 33, GLUT_BITMAP_9_BY_15, "<Spacebar> Fire");
        vprint(-155, -winHeight / 2 + 33, GLUT_BITMAP_9_BY_15, "<F1> Reset");
        vprint(-15, -winHeight / 2 + 33, GLUT_BITMAP_9_BY_15, "<Up/Down> Aim Gun");
        vprint(185, -winHeight / 2 + 33, GLUT_BITMAP_9_BY_15, "<Left/Right> Move Ship");

        vprint(-580, -winHeight / 2 + 8, GLUT_BITMAP_9_BY_15, "<F> Switch Fire Modes");
        vprint(-375, -winHeight / 2 + 8, GLUT_BITMAP_9_BY_15, "<W/S> Increase/Decrease Fire Rate");
        vprint(-55, -winHeight / 2 + 8, GLUT_BITMAP_9_BY_15, "<O/P> Increase/Decrease Fire Speed");
        vprint(270, -winHeight / 2 + 8, GLUT_BITMAP_9_BY_15, "<1-4> Switch Between Difficulties");
    }
}
void drawPlayer() {
    float angle = p.angle * D2R;

    //Side Guns
    glLineWidth(4);
    glColor3f(.85, 0.1, 0.16);
    glBegin(GL_LINES);
    vertex({ 20, -10 }, p.pos, angle);
    vertex({ 20,  25 }, p.pos, angle);
    vertex({ -20,-10 }, p.pos, angle);
    vertex({ -20, 25 }, p.pos, angle);
    glEnd();
    glLineWidth(1);

    //Thrusters
    glColor3ub(0, 224, 224);
    glColor3f(1, .36, 0);
    glBegin(GL_POLYGON);
    vertex({ -18, -26.5 }, p.pos, angle);
    vertex({ -18, -10 }, p.pos, angle);
    vertex({ -13, -10 }, p.pos, angle);
    vertex({ -13, -26.5 }, p.pos, angle);
    glEnd();
    glBegin(GL_POLYGON);
    vertex({ 18, -26.5 }, p.pos, angle);
    vertex({ 18, -10 }, p.pos, angle);
    vertex({ 13, -10 }, p.pos, angle);
    vertex({ 13, -26.5 }, p.pos, angle);
    glEnd();

    //Main Body
    glColor3ub(55, 55, 55);
    glBegin(GL_TRIANGLES);
    vertex({ 0  ,  30 }, p.pos, angle);
    vertex({ -40, -20 }, p.pos, angle);
    vertex({ +40, -20 }, p.pos, angle);
    glColor3ub(0, 224, 224);
    vertex({ 0  ,  20 }, p.pos, angle);
    vertex({ -15 ,  0 }, p.pos, angle);
    vertex({ +15 ,  0 }, p.pos, angle);
    glEnd();
}

void drawUFO() {
    c.x = ufo.pos.x * cos(ufo.angle * D2R) - 250;
    c.y = ufo.pos.y * sin(ufo.angle * D2R) + 100;

    glColor4ub(204, 255, 255, 200);
    circle(c.x, c.y, ufo.radius);

    glColor3ub(76, 153, 76);
    glBegin(GL_POLYGON);
    glVertex2f(c.x - 18, c.y - 3);
    glVertex2f(c.x - 40, c.y - 25);
    glVertex2f(c.x + 40, c.y - 25);
    glVertex2f(c.x + 18, c.y - 3);
    glEnd();
}

void drawShip() {
    glColor3ub(50, 50, 50);
    //Main Body
    glBegin(GL_POLYGON);
    glVertex2f(ship.x + 135, 315);
    glVertex2f(ship.x, 300);
    glVertex2f(ship.x + 80, 300);
    glVertex2f(ship.x + 80, 330);
    glVertex2f(ship.x, 330);
    glEnd();
    //Decor 1
    glColor4ub(153, 255, 255, 195);
    glBegin(GL_TRIANGLE_STRIP);
    glVertex2f(ship.x + 90, 322.5);
    glVertex2f(ship.x + 120, 315);
    glVertex2f(ship.x + 90, 307.5);
    glEnd();
    //Decor 2
    glBegin(GL_POLYGON);
    glVertex2f(ship.x + 85, 322.5);
    glVertex2f(ship.x + 65, 315);
    glVertex2f(ship.x + 85, 307.5);
    glEnd();
}

void drawFires() {
    for (int i = 0; i < MAX_FIRE; i++) {
        if (fr[i].active && mode == SINGULAR) {
            glColor3ub(255, 178, 102);
            circle(fr[i].pos.x, fr[i].pos.y, 7);
        }
        else if (fr[i].active && mode == DOUBLE) {
            glColor3ub(255, 72, 72);
            circle(fr[i].pos.x - 20, fr[i].pos.y, 4);
            circle(fr[i].pos.x + 20, fr[i].pos.y, 4);
        }
    }
}

int findAvailableFire() {
    for (int i = 0; i < MAX_FIRE; i++) {
        if (fr[i].active == false) return i;
    }
    return -1;
}

bool UFOCollision(fire_t fire, ufo_t ufo) {
    float dx = c.x - fire.pos.x;
    float dy = c.y - fire.pos.y;
    float d = sqrt(dx * dx + dy * dy);
    return d <= ufo.radius + 7;
}

bool shipCollision(fire_t fire, enemy2_t ship) {
    if (fire.pos.x <= ship.x + 90 && fire.pos.x >= ship.x &&
        fire.pos.y >= 307.5 && fire.pos.y <= 330)
        return true;
    else
        return false;
}

//
// To display onto window using OpenGL commands
//
void display() {
    //
    // clear window to black
    //
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    drawBG();
    drawFires();
    drawPlayer();
    infoBar();
    if (!ship.hit) drawShip();
    if (!ufo.hit) drawUFO();

    glColor3ub(180, 255, 180);
    if (!extraComs)
        vprint(winWidth / 2 - 170, 50, GLUT_BITMAP_9_BY_15, "<X> Extra Commands");
    else
        vprint(winWidth / 2 - 225, 50, GLUT_BITMAP_9_BY_15, "<X> Close Extra Commands");


    glutSwapBuffers();
}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);

    switch (key) {
    case ' ': spacebar = true; break;
    case 'x':
    case 'X': extraComs = !extraComs; break;
    case 'f':
    case 'F': if (mode == SINGULAR) mode++;
              else mode--; break;
    case 'w':
    case 'W': if (FIRERATE >= 1) FIRERATE--; break;
    case 's':
    case 'S': if (FIRERATE <= 60) FIRERATE++; break;
    case 'o':
    case 'O': if (fSpeed <= 30) fSpeed += 2;  break;
    case 'p':
    case 'P': if (fSpeed > 2) fSpeed -= 2;
        break;
    case '1': difficultyS = 7; difficultyU = 3; fSpeed = 8; FIRERATE = 15; 
        break;
    case '2': difficultyS = 11; difficultyU = 5; fSpeed = 7; FIRERATE = 25;
        break;
    case '3': difficultyS = 14; difficultyU = 7; fSpeed = 6; FIRERATE = 35;
        break;
    case '4': difficultyS = 20; difficultyU = 10; fSpeed = 5; FIRERATE = 50;
        break;
    }

    // to refresh the window it calls display() function
    //glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);

    if (key == ' ')
        spacebar = false;
    // to refresh the window it calls display() function
    //glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y) {
    // Write your codes here.
    switch (key) {
    case GLUT_KEY_UP: up = true; break;
    case GLUT_KEY_DOWN: down = true; break;
    case GLUT_KEY_LEFT: left = true; break;
    case GLUT_KEY_RIGHT: right = true; break;
    case GLUT_KEY_F1: 
        if(ship.hit == true) ship.x = -240; 
        ufo.hit = false; ship.hit = false;
        break;
    }

    // to refresh the window it calls display() function
    //glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y)
{
    // Write your codes here.
    switch (key) {
    case GLUT_KEY_UP: up = false; break;
    case GLUT_KEY_DOWN: down = false; break;
    case GLUT_KEY_LEFT: left = false; break;
    case GLUT_KEY_RIGHT: right = false; break;
    }

    // to refresh the window it calls display() function
    //glutPostRedisplay();
}

//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int stat, int x, int y)
{
    // Write your codes here.



    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
    winWidth = w;
    winHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    display(); // refresh window.
}

void onMoveDown(int x, int y) {
    // Write your codes here.



    // to refresh the window it calls display() function   
    glutPostRedisplay();
}

// GLUT to OpenGL coordinate conversion:
//   mx = x - winWidth / 2;
//   my = winHeight / 2 - y;
void onMove(int x, int y) {
    // Write your codes here.



    // to refresh the window it calls display() function
    glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {

    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
    // Write your codes here.

    if (right) p.pos.x += 5;
    if (left) p.pos.x -= 5;
    if (down) p.angle += 3;
    if (up) p.angle -= 3;

    if (p.pos.x + 40 >= winWidth / 2) p.pos.x -= 5;
    if (p.pos.x - 40 <= -winWidth / 2) p.pos.x += 5;
    if (p.angle > 45)p.angle -= 3;
    if (p.angle < -45)p.angle += 3;

    ship.x += difficultyS;
    if (ship.x >= winWidth / 2)
        ship.x = -winWidth / 2 - 120;

    ufo.angle += difficultyU;

    if (spacebar && fire_rate == 0) {
        int availFire = findAvailableFire();
        if (availFire != -1) {
            fr[availFire].pos = p.pos;
            fr[availFire].angle = p.angle + 90;
            fr[availFire].active = true;
            fire_rate = FIRERATE;
        }
    }

    if (fire_rate > 0) fire_rate--;

    for (int i = 0; i < MAX_FIRE; i++) {
        if (fr[i].active) {
            fr[i].pos.x += fSpeed * cos(fr[i].angle * D2R);
            fr[i].pos.y += fSpeed * sin(fr[i].angle * D2R);

            if (fr[i].pos.x > winWidth / 2 || fr[i].pos.x < -winWidth / 2 || fr[i].pos.y > winHeight / 2 || fr[i].pos.y < -winHeight / 2)
                fr[i].active = false;

            if (UFOCollision(fr[i], ufo))
                ufo.hit = true;

            if (shipCollision(fr[i], ship))
                ship.hit = true;
        }
    }
    
    // to refresh the window it calls display() function
    glutPostRedisplay(); // display()

}
#endif

void Init() {

    // Smoothing shapes
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(250, 120);
    glutCreateWindow("HW3: \"Budget Space Invaders\" by Suphi Erkin Karaçay");

    glutDisplayFunc(display);
    glutReshapeFunc(onResize);

    //
    // keyboard registration
    //
    glutKeyboardFunc(onKeyDown);
    glutSpecialFunc(onSpecialKeyDown);

    glutKeyboardUpFunc(onKeyUp);
    glutSpecialUpFunc(onSpecialKeyUp);

    //
    // mouse registration
    //
    glutMouseFunc(onClick);
    glutMotionFunc(onMoveDown);
    glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
    // timer event
    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

    Init();

    glutMainLoop();
}