#include <windows.h>  
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <time.h>       /* time */

#include <iostream>
#include <fstream>

static int shoulder_angle = 0, elbow_angle = 0;

int x_other, y_other;
int x_home, y_home;

void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
}



static void body_draw(void)
{
	GLfloat i = 0.5, o = 1;

	glPushMatrix();

	glutWireTorus(i, o, 20, 20);
	glTranslatef(0.0, 1, 0.0);
	glutWireCube(0.5);
	glTranslatef(0.0, -2, 0.0);
	glutWireCube(0.5);
	glPopMatrix();


}

static void arm_draw(GLfloat width, GLfloat height)
{
	GLUquadricObj *quadratic;
	quadratic = gluNewQuadric();
	glPushMatrix();
	//glutWireOctahedron();
	glTranslatef(height/4, 0.0, 0.0);
	//glutSolidSphere(1.0);
	//glTranslatef((width / 2), 0.0, 0.0);
	glPushMatrix();
	glRotatef(-90.0, 0.0, 1.0, 0.0);
	glutWireCone((width/2), (height/4), 10, 10);
	glPopMatrix();
	//glTranslatef((height / 4), 0.0, 0.0);
	glPushMatrix();
	glRotatef(90.0, 0.0, 1.0, 0.0);
	gluCylinder(quadratic, (width / 2), (width / 2), height/2, 10, 10);
	glPopMatrix();
	//glTranslatef(1, 0.0, 0.0);
	glTranslatef((height / 2), 0.0, 0.0);
	glPushMatrix();
	glRotatef(90.0, 0.0, 1.0, 0.0);
	glutWireCone((width / 2), (height / 4), 20, 20);
	glPopMatrix();
	glPopMatrix();

}

static void part_draw(GLfloat width, GLfloat height)
{
	/* right side */
	glColor3f(1, 0.7, 0.1);
	glPushMatrix();
	glTranslatef(height/4, 0.0, 0.0);
	glRotatef((GLfloat)shoulder_angle, 0.0, 0.0, 1.0);

	arm_draw(width, height);

	/*glPushMatrix();
	glScalef(1.0, 0.2, 1.0);
	glutSolidSphere(0.5, 20, 20);
	glPopMatrix() */;

	glColor3f(0.5, 0.3, 0.7);

	glTranslatef(height, 0.0, 0.0);
	glRotatef((GLfloat)elbow_angle, 0.0, 0.0, 1.0);
	/*glPushMatrix();
	glScalef(1.0, 0.2, 1.0);
	glutWireSphere(0.5, 20, 20);
	glPopMatrix(); */
	arm_draw(width, height);
	glPopMatrix();

}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1, 0.1, 1);

	glPushMatrix();
	glScalef(1.0, 0.8, 1.0);
	body_draw();
	glPopMatrix();

	GLfloat width = 0.3;
	GLfloat height = 2;

	
	glPushMatrix();
	part_draw(width, height);

	glTranslatef(0.0, 1.0, 0.0);
	part_draw(width, height);

	glTranslatef(0.0, -2.0, 0.0);
	part_draw(width, height);

	glPopMatrix;
	glutSwapBuffers();
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(65.0, (GLfloat)w / (GLfloat)h, 1.0, 20.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -5.0);
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
		case 's':   /*  s key rotates at shoulder_angle  */
			shoulder_angle = (shoulder_angle + 5) % 360;
			glutPostRedisplay();
			break;
			case 'S':
				shoulder_angle = (shoulder_angle - 5) % 360;
				glutPostRedisplay();
				break;
				case 'e':  /*  e key rotates at elbow_angle  */
					elbow_angle = (elbow_angle + 5) % 360;
					glutPostRedisplay();
					break;
					case 'E':
						elbow_angle = (elbow_angle - 5) % 360;
						glutPostRedisplay();
						break;
					default:
						break;
	}
}


void mouseCallBack(int button, int state, int x, int y)
{
	{
		if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
		{
			glutIdleFunc(NULL);
		}
		else if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN))
		{
			
			glutIdleFunc(display);
		}
		else
		{
			glutIdleFunc(NULL);

		}
	}
}


void motionCallBack(int x, int y)
{
	//cout << "( " << x << " " << y << " )\t";
	//cout << "( " << x_home << " " << y_home << " )\t";
	//cout << "( " << x_other << " " << y_other << " )\n";

	{
		x_other = x;
		y_other = y;
		glutIdleFunc(display);
	}
}
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Tharun Battula - Homework 4	");

	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouseCallBack);
	glutMotionFunc(motionCallBack);
	glutMainLoop();
	return 0;
}