#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <windows.h>  
#include <GL/gl.h>
#include <GL/glut.h>

static GLfloat spin = 0.0;
static GLfloat default_rate = 1;
static int direction = 1;
static int ydiff = 0;
static int yhome = 0;
static int xdiff = 0;
static int xhome = 0;
static GLfloat colors[3] = {1.0f, 0.5f, 0.1f };
static int type = 2;
static int local_x = 0;
static int local_y = 0;
static bool shift_flag = 0;

void init(void)
{
	glClearColor(colors[0]*0.5, colors[1]*0.5, colors[2]*0.5, 0.0);
	glShadeModel(GL_FLAT);
}

static void polygon_draw(void)
{
	if (type == 1)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		glBegin(GL_POLYGON);
	}
	else if (type == 2)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glBegin(GL_POLYGON);
	}
	else if (type == 3)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBegin(GL_POLYGON);
	}
	glColor3d(0.2, 0.8, 0.9);

	glVertex3f( - 50.0, -50.0, 0.0);
	glVertex3f( - 50.0,  +50.0, 0.0);
	glVertex3f( + 0.0,  + 50.0, 0.0);
	glVertex3f( + 40,  + 0, 0.0);
	glVertex3f( + 0.0,  - 50.0, 0.0);
	glEnd();
	
}

void keyboardCallBack(unsigned char key, int x, int y)
{
	switch (key) {
	case 'c':
		colors[0] = 0.0f;
		colors[1] = 1.0f;
		colors[2] = 1.0f;
		glutPostRedisplay();
		break;
	case 'm':
		colors[0] = 1.0f;
		colors[1] = 0.0f;
		colors[2] = 1.0f;
		glutPostRedisplay();
		break;
	case 'y':
		colors[0] = 1.0f;
		colors[1] = 1.0f;
		colors[2] = 0.0f;
		glutPostRedisplay();
		break;
	case 'w':
		colors[0] = 1.0f;
		colors[1] = 1.0f;
		colors[2] = 1.0f;
		glutPostRedisplay();
		break;
	case '1':
		type = 1;
		glutPostRedisplay();
		break;
	case '2':
		type = 2;
		glutPostRedisplay();
		break;
	case '3':
		type = 3;
		glutPostRedisplay();
		break;
	/* case 'r': // reset
		  spin = 0.0;
		  default_rate = 1;
		  direction = 1;
		  ydiff = 0;
		  yhome = 0;
		  xdiff = 0;
		  xhome = 0;
		  type = 2;
		  local_x = 0;
		  local_y = 0;
		  break; */
	default:
		break;
	}
}


void display(void)
{
	GLfloat intensity = 0.5;
	if (!shift_flag)
	{
		if (xdiff)
		{
			intensity = 0.5 - ((GLfloat)xdiff / 500);
		}
		glClearColor(colors[0] * intensity, colors[1] * intensity, colors[2] * intensity, 0.0);
	}

	glClear(GL_COLOR_BUFFER_BIT);

	glPushMatrix();
	glLoadIdentity();

	if (shift_flag)
	{
	/*	printf("local shift = %d\t%d\n", local_x, local_y); */
		glTranslatef(local_x - xdiff, local_y + ydiff, 0);
	}
	else
	{
		glTranslatef(local_x, local_y, 0);
	}
	glRotatef(spin, 0.0, 0.0, 1.0);

	glColor3f(1, 0.7, 0.1);
	polygon_draw();

	glPopMatrix();

	glutSwapBuffers();
}

void spinDisplay(void)
{
	GLfloat offset;

		offset = default_rate +(ydiff * 0.004);
		if (offset < 0)
			offset = 0;


	spin = spin + (direction * offset);
	
	if (spin > 360.0)
		spin = spin - 360.0;
	if (spin < -360.0)
		spin = spin + 360.0;
	glutPostRedisplay();
}
void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-125.0, 125.0, -125.0, 125.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
void mouse(int button, int state, int x, int y)
{
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
	{
		direction = 1;
		yhome = y;
		xhome = x;
		xdiff = 0;
		ydiff = 0;
		shift_flag = 0;

		/*
		printf("diff y --> %d, %d)\n", ydiff, ydiff);
		printf("diff x --> %d, %d)\n", xdiff, xdiff);
		*/
		glutIdleFunc(spinDisplay);
	}
	else if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN))
	{
		direction = -1;
		yhome = y;
		xhome = x;
		ydiff = 0;
		xdiff = 0;
		shift_flag = 0;

		glutIdleFunc(spinDisplay);

	}
	else if ((button == GLUT_MIDDLE_BUTTON) && (state == GLUT_DOWN))
	{
		yhome = y;
		xhome = x;
		direction = 0;
		ydiff = 0;
		xdiff = 0;
		shift_flag = 1;
		glutIdleFunc(spinDisplay);
	}
	else if ((button == GLUT_MIDDLE_BUTTON) && (state == GLUT_UP))
	{
		xdiff = -(x - xhome);
		ydiff = -(y - yhome);
		local_x += -xdiff;
		local_y += ydiff;

		shift_flag = 0;
		glutIdleFunc(NULL);
	}
	else
	{
		xhome = 0;
		xdiff = 0;
		yhome = 0;
		ydiff = 0;
		glutIdleFunc(NULL);
	}
}
//======================================================
// MOUSE MOTION CALLBACK ROUTINE 
//======================================================
void motionCallBack(int x, int y)
{
	/* printf("x , y  %d, %d)\n", x, y); */

	//Set square's location to current mouse position
	xdiff = -(x - xhome);
	ydiff = -(y - yhome);

	/*printf("diff %d, %d)\n", ydiff, xdiff); */

	glutIdleFunc(spinDisplay);
}


/*
* Request double buffer display mode.
* Register mouse input callback functions
*/
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(250, 250);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Tharun Battula - Assignement 2");

	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboardCallBack);
	glutMotionFunc(motionCallBack);

	glutMainLoop();
	return 0;
}