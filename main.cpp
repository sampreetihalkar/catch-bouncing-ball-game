#include <windows.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <iostream>

int timer = 0; //for displaying "+1" when balloon is hit
int score = 0; //number of balloons hit
int level = 0, size = 1;
GLfloat color_array[][3] = {
    {1, 0, 0},    // Red
    {0, 0, 1},    // Blue
    {0, 1, 0},    // Green
    {1, 0, 1},    // Magenta
    {1, 1, 0},    // Yellow
    {0, 1, 1},    // Cyan
    {1, 0.5, 0},  // Orange
    {0.5, 0, 0.5}, // Purple
    {0.5,1,0.5},
    {0,0,0}

};

int game_level[] = {60, 40, 20};
float play_rate = game_level[level];
GLfloat paddle_size[] = {2, 4, 6,8,10};
//the grid size. fills from bottom to top.
int rows = 4;
int columns = 10;

// to help store the coordinates of each balloon
struct balloon_coords
{
    GLfloat x;
    GLfloat y;
};

//Array to store the balloons
balloon_coords balloon_matrix[50][50];
GLfloat x_paddle, x_ball = 0, y_ball = -8, speed = 0, x_direction = 0, y_direction = 0, start = 0;

//change the pace of game
void change_difficulty(int action)
{
    level = action - 1;
}

//handle menu
void handle_menu(int action)
{
}

//change paddle size
void change_paddle_size(int action)
{
    size = action +8;
}

void addMenu()
{ //for difficulty
    int submenu1 = glutCreateMenu(change_difficulty);
    glutAddMenuEntry("Level 1(easy)", 1);
    glutAddMenuEntry("Level 2(medium)", 2);
    glutAddMenuEntry("Level 3(hard)", 3);
    //for paddle size
    int submenu2 = glutCreateMenu(change_paddle_size);
    glutAddMenuEntry("Small", 1);
    glutAddMenuEntry("Medium", 2);
    glutAddMenuEntry("Large", 3);
    //main menu
    glutCreateMenu(handle_menu);
    glutAddSubMenu("Game Level", submenu1);
    glutAddSubMenu("Paddle Size", submenu2);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

//printing stuff on the screen
void display_text(int sc)
{
    glDisable(GL_LIGHTING);
    char text[40];
    char nextline[40];
    char difficulty[10];

    sprintf(nextline, "press S to start");
    if (level == 0)
        sprintf(difficulty, "Level 1");
    if (level == 1)
        sprintf(difficulty, "Level 2");
    if (level == 2)
        sprintf(difficulty, "Level 3");

    if (sc < 40)
        //sprintf(text, "Game Level: %s    Your Score: %d", difficulty, sc);
        sprintf(text, "         Your Score: %d",sc);

    else
    {
        sprintf(text, "Congratulations !!! You won !!");
        start = 0;
        y_ball = -12.8;
        x_ball = 0;
        x_direction = 0;
        y_direction = 0;
        x_paddle = 0;
    }
    //text to display when a balloon is popped
    char hit[10];
    sprintf(hit, "+1");

    // The color

    glColor3f(0, 0, 0);
    // Position of the text to the printer
    glPushMatrix();
    glTranslatef(-4, 0, 0);
    glRasterPos3f(0, 0, 0);
    for (int i = 0; text[i] != '\0'; i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);

    if (sc == 0 && x_direction == 0 && y_direction == 0)
    {
        glTranslatef(2.5, -2, 0);
        glRasterPos3f(0, 0, 0);
        for (int i = 0; nextline[i] != '\0'; i++)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, nextline[i]);
    }

    if (timer > 0)
    {
        glColor3f(1, 0, 0);
        glTranslatef(x_ball + 3.6, y_ball, 0);
        glRasterPos3f(0, 0, 1);
        for (int k = 0; hit[k] != '\0'; k++)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, hit[k]);
        timer--;
    }
    glEnable(GL_LIGHTING);
    glPopMatrix();
}

//to visibly see the broder
void draw_border()
{
    //glDisable(GL_LIGHTING);
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_POLYGON);
    //vertices for the paddle polygon
    glVertex3f(17, 15, 0);
    glVertex3f(16.5, 15, 0);
    glVertex3f(16.5, -15, 0);
    glVertex3f(17, -15, 0);
    glEnd();

    glBegin(GL_POLYGON);
    //vertices for the paddle polygon
    glVertex3f(-17, 15, 0);
    glVertex3f(-17.5, 15, 0);
    glVertex3f(-17.5, -15, 0);
    glVertex3f(-17, -15, 0);
    glEnd();
    //glEnable(GL_LIGHTING);
}

// paddle display
//basically a slim rectangle
void draw_paddle()
{
    glDisable(GL_LIGHTING);
    glColor3f(1, 1, 0);
    glBegin(GL_POLYGON);
    glVertex3f(-paddle_size[size] + x_paddle, -15, 0);
    glVertex3f(paddle_size[size] + x_paddle, -15, 0);
    glVertex3f(paddle_size[size] + x_paddle, -14, 0);
    glVertex3f(-paddle_size[size] + x_paddle, -14, 0);
    glEnd();
    glEnable(GL_LIGHTING);
}

//to draw balloon at given location
void balloon(GLfloat x, GLfloat y, GLfloat z, GLint col)
{
    //using triangle fan here to create a circle
    int i;
    int triangleAmount = 20; //# of triangles used to draw circle
    glDisable(GL_LIGHTING);
    //GLfloat radius = 0.8f; //radius
    GLfloat twicePi = 2.0f * 3.1415926;
    GLfloat radius = 1.0f;
    glBegin(GL_TRIANGLE_FAN);
    glColor3fv(color_array[col]);
    glVertex2f(x, y); // center of circle
    for (i = 0; i <= triangleAmount; i++)
    {

        glVertex2f(
            x + (radius * cos(i * twicePi / triangleAmount)),
            y + (radius * sin(i * twicePi / triangleAmount)));
    }

    glEnd();
    glEnable(GL_LIGHTING);
}

//now draw a grid of balloons
void draw_balloons()
{
    int i, j;
    if (start == 0) //initilize when game not started
    {
        for (i = 1; i <= rows; i++)
        {
            for (j = 1; j <= columns; j++)
            {
                balloon_matrix[i][j].x = (GLfloat)(j * 4 * 0.84 + 0.5f);
                balloon_matrix[i][j].y = (GLfloat)(i * 2 * 1.2 - 1.5f);
            }
        }
    }

    glPushMatrix();
    glTranslatef(-19.5, 5, 0);

    for (i = 1; i <= rows; i += 1)
    {
        for (j = 1; j <= columns; j += 1)
        {
            if (balloon_matrix[i][j].x != 0 || balloon_matrix[i][j].y != 0) //i.e. balloon not popped yet
            {
                balloon(balloon_matrix[i][j].x, balloon_matrix[i][j].y, 0, ((i - 1) * 4 + (j - 1)) % 6);
            }
        }
    }
    glPopMatrix();
}

//the moving object
void draw_ball()
{
    glDisable(GL_LIGHTING);

    GLint triangleAmount = 20;
    GLfloat twicePi = 2.0f * 3.1415926;
    GLfloat radius = 1.0f;
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0.0, 0.0, 0.0);
    glVertex2f(x_ball, y_ball);
    for (int i = 0; i <= triangleAmount; i++)
    {

        glVertex2f(
            x_ball + (radius * cos(i * twicePi / triangleAmount)),
            y_ball + (radius * sin(i * twicePi / triangleAmount)));
    }

    glEnd();
    glEnable(GL_LIGHTING);
}

//keyboard function
void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'd':
        x_paddle += 3;
        break;
    case 'a':
        x_paddle -= 3;
        break;
    case 'q':
        exit(0);
        break;
    case 's':
        if (!start)
        {
            x_direction = y_direction = 1;
            play_rate = game_level[level];
            start = 1;
            score = 0;
            glutSetCursor(GLUT_CURSOR_NONE);
        }
        break;
    }
    //boundaries for movement
    if (x_paddle > 15)
        x_paddle = 15;
    else if (x_paddle < -15)
        x_paddle = -15;

    //dont move if game didnt start
    if (start == 0)
        x_paddle = 0;
    glutPostRedisplay();
}

//mouse function
void mousemotion(int x, int y)
{
    if (start == 1)
    {
        x_paddle = (x - glutGet(GLUT_WINDOW_WIDTH) / 2) / 20;

        //boundaries for movement
        if (x_paddle > 15)
        {
            x_paddle = 15;
        }
        if (x_paddle < -15)
        {
            x_paddle = -15;
        }
    }

    else
        glutSetCursor(GLUT_CURSOR_INHERIT);
}

// display function called infinitely
void display() {
    glClearColor(0.1, 0.0,0.45, 1.0); // Black color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(0, 0, 0, 0, 0, -25, 0, 1, 0);
    glTranslatef(0, 0, -25);
    draw_border();
    draw_paddle();
    draw_balloons();
    draw_ball();
    display_text(score);
    glutSwapBuffers();
}


void reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (GLfloat)w / (GLfloat)h, 1.0, 1000.0);
    glMatrixMode(GL_MODELVIEW);
}

//detect and handle hits
void balloon_hit()
{
    int i, j;
    //check if any balloon hit
    for (i = 1; i <= rows; i++)
        for (j = 1; j <= columns; j++)
        {
            if ((x_ball >= balloon_matrix[i][j].x - 21) && (x_ball <= balloon_matrix[i][j].x - 18)) //checking along x axis
            {
                if (y_ball >= balloon_matrix[i][j].y + 4 && y_ball <= balloon_matrix[i][j].y + 6) //checking along y axis
                {
                    timer = 100;
                    balloon_matrix[i][j].x = 0;
                    balloon_matrix[i][j].y = 0;
                    score++;
                }
            }
        }
}

//The idle function. Handles the motion of the ball along with rebounding from various surfaces
void idle()
{ //the wall bouncing stuff
    balloon_hit();
    if (y_ball > 14 && start == 1) //top wall
    {
        y_direction *= -1;
    }
    if (x_ball < -16 || x_ball > 16 && start == 1) //left and right walls
    {
        x_direction *= -1;
    }
    x_ball += x_direction / (play_rate);
    y_ball += y_direction / (play_rate);
    play_rate -= 0.001; // increase speed as game progress
    float x = paddle_size[size];

    //five cases. veryleft, left,middle,right and very right of paddle
    //<12.8 checked here so ball doesnt bounces at anywhere along y axis
    if (y_ball <= -12.8 && x_ball < (x_paddle - x / 3) && x_ball > (x_paddle - x * 2 / 3) && start == 1) //left
    {
        x_direction = -1;
        y_direction = 1;
    }

    else if (y_ball <= -12.8 && x_ball < (x_paddle + x / 3) && x_ball > (x_paddle - x / 3) && start == 1) //middle
    {
        x_direction = x_direction;
        y_direction = 1;
    }

    else if (y_ball <= -12.8 && x_ball < (x_paddle + x * 2 / 3) && x_ball > (x_paddle + x / 3) && start == 1) //right
    {
        x_direction = 1;
        y_direction = 1;
    }

    else if (y_ball <= -12.8 && x_ball < (x_paddle - (x * 2 / 3)) && x_ball > (x_paddle - (x + 0.3)) && start == 1) //very left
    {
        x_direction = -1.5;
        y_direction = 0.8;
    }

    else if (y_ball <= -12.8 && x_ball < (x_paddle + (x + 0.3)) && x_ball > (x_paddle + x / 3) && start == 1) //very right
    {
        x_direction = 1.5;
        y_direction = 0.8;
    }

    else if (y_ball < -13) //if touches the bottom the game must stop. player lost the game
    {
        start = 0;
        y_ball = -12.8;
        x_ball = 0;
        x_direction = 0;
        y_direction = 0;
        x_paddle = 0;
    }
    glutPostRedisplay();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(1200, 1000);
    glutCreateWindow("Balloon Popper Game");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glEnable(GL_DEPTH_TEST);
    glutIdleFunc(idle);
    glutPassiveMotionFunc(mousemotion);
    glutKeyboardFunc(keyboard);

    addMenu();

    //opens window maximized. uses windows api.
    HWND win_handle = FindWindow(0, "Balloon Popper Game");
    if (!win_handle)
    {
        printf("Failed FindWindow\n");
        return -1;
    }
    SetWindowLong(win_handle, GWL_STYLE, (GetWindowLong(win_handle, GWL_STYLE) | WS_MAXIMIZE));
    ShowWindowAsync(win_handle, SW_SHOWMAXIMIZED);
    glutMainLoop();
    return 0;
}
