#include <windows.h>  
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <time.h>       /* time */

#include <iostream>
#include <fstream>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

static int shoulder_angle[3][7]; 
static int elbow_angle[3][7];
static int wrist_angle[3][7];
static int current_i = 0;

static int window_width = 500;
static int window_height = 500;

static GLint xhome = 0;
static GLint yhome = 0;

static GLint lat_xdiff = 0;
static GLint lat_ydiff = 0;
static GLint xdiff = 0;
static GLint ydiff = 0;
static GLfloat xpan = 0;
static GLfloat ypan = 0;
static GLfloat xvec = 0;
static GLfloat yvec = 0;
static GLfloat xvec_p = 0;
static GLfloat yvec_p = 0; 

static GLfloat view_distance = 0;
static GLfloat zoom_factor = 1;

static GLfloat x_offset = 0;
static GLfloat y_offset = 0;
using namespace std;
typedef enum joint
{
	head = 0,
	shoulder,
	elbow ,
	wrist 
} joint;

typedef enum mode
{
	free_move = 0,
	zoom,
	pan,
	eye_move,
	rotat,
	walk,
	go
	
} mode;

typedef enum rotation
{
	xaxis = 0,
	yaxis,
	zaxis,
} rotation_axis;

static rotation_axis rot_axis = zaxis;
static joint current_joint = shoulder;
static mode current_mode = free_move;
int theta_lim = 70;
void load_walk_angles(GLint x, GLfloat y)
{
	shoulder_angle[zaxis][1] = 10 * (x % 3)*(2-(x%2)) + (10 * x % 10);
	shoulder_angle[zaxis][2] = -5 * (x % 3)*(2 + (x % 2)) - (8 * x % 10);
	shoulder_angle[zaxis][3] = 10 * (x % 3)*(2 - (x % 2)) - (10 * x % 10);
	shoulder_angle[zaxis][4] = -10 * (x % 3)*(2 + (x % 2)) + (8 * x % 10);
	shoulder_angle[zaxis][5] = 5 * (x % 3)*(2 - (x % 2)) - (10 * x % 10);
	shoulder_angle[zaxis][6] = -10 * (x % 3)*(2 + (x % 2)) + (15 * x % 10);

	elbow_angle[zaxis][1] = 20 * (2 - (x % 2)) + (12 * x % 6);
	elbow_angle[zaxis][2] = 25 * (2 - (x % 2)) - (10 * x % 5);
	elbow_angle[zaxis][3] = 20 * (2 - (x % 2)) - (20 * x % 6);
	elbow_angle[zaxis][4] = 20 * (2 - (x % 2)) + (12 * x % 6);
	elbow_angle[zaxis][5] = 25 * (2 - (x % 2)) - (10 * x % 6);
	elbow_angle[zaxis][6] = 20 * (2 - (x % 2)) + (20 * x % 6);

	shoulder_angle[yaxis][1] = -30 + (12 * x % 6);
	shoulder_angle[yaxis][2] = -40 - (10 * x % 5);
	shoulder_angle[yaxis][3] = -40 - (20 * x % 6);
	shoulder_angle[yaxis][4] = -30 + (12 * x % 6);
	shoulder_angle[yaxis][5] = -40 - (10 * x % 6);
	shoulder_angle[yaxis][6] = -40 + (20 * x % 6);

	elbow_angle[yaxis][1] = -(90 + shoulder_angle[yaxis][1]) + x % 20;//-20;
	elbow_angle[yaxis][2] = -(90 + shoulder_angle[yaxis][2]) - x % 10;//-25;
	elbow_angle[yaxis][3] = -(90 + shoulder_angle[yaxis][3]) + x % 40;//-40;
	elbow_angle[yaxis][4] = -(90 + shoulder_angle[yaxis][4]) - x % 20;//-20;
	elbow_angle[yaxis][5] = -(90 + shoulder_angle[yaxis][5]) - x % 80;// -25;
	elbow_angle[yaxis][6] = -(90 + shoulder_angle[yaxis][6]) + x % 10;//-20;
}
void load_default_angles(void)
{
	shoulder_angle[zaxis][1] = 20;
	shoulder_angle[zaxis][2] = 0;
	shoulder_angle[zaxis][3] = -20;
	shoulder_angle[zaxis][4] = 20;
	shoulder_angle[zaxis][5] = 0;
	shoulder_angle[zaxis][6] = -20;

	elbow_angle[zaxis][1] = 20;
	elbow_angle[zaxis][2] = 25;
	elbow_angle[zaxis][3] = 20;
	elbow_angle[zaxis][4] = 20;
	elbow_angle[zaxis][5] = 25;
	elbow_angle[zaxis][6] = 20;

	shoulder_angle[yaxis][1] = -30;
	shoulder_angle[yaxis][2] = -40;
	shoulder_angle[yaxis][3] = -40;
	shoulder_angle[yaxis][4] = -30;
	shoulder_angle[yaxis][5] = -40;
	shoulder_angle[yaxis][6] = -40;

	elbow_angle[yaxis][1] = -20;
	elbow_angle[yaxis][2] = -25;
	elbow_angle[yaxis][3] = -40;
	elbow_angle[yaxis][4] = -20;
	elbow_angle[yaxis][5] = -25;
	elbow_angle[yaxis][6] = -20;
}

void WalkDisplay(void)
{
	
		if (x_offset < 100)
		{
			x_offset+=  10;
			//cout << x_offset;
			glutPostRedisplay();
		}
		else
		{
			glutIdleFunc(NULL);
		}
		Sleep(100);

			/*x_offset--;
		if (x_offset < 0)*/ 
}

void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	
	memset(elbow_angle, 0, sizeof(elbow_angle));
	memset(shoulder_angle, 0, sizeof(shoulder_angle));
	memset(wrist_angle, 0, sizeof(wrist_angle));
	load_default_angles();

	glShadeModel(GL_FLAT);
	
	current_i = 1;
	current_joint = shoulder;
}



static void body_draw(void)
{
	GLfloat inner = 0.5, outer = 1;
	GLfloat xscale = 1.0, yscale = 1.2;

	glPushMatrix();
	glScalef(xscale, yscale, 1);
	glColor3f(0.9, 0.9, 0.9);

	glPushMatrix();
	glScalef(1.2,1, 1); /* scale entire torrus mass*/
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.7f, 0.5f, 0.1f);
	glutSolidTorus(inner, outer, 20, 20);

	glPushMatrix;
	glTranslatef(0, 0, 0);
	glColor3f(0.7f, 0.1f, 0.1f);
	glScalef(1/xscale, 1/yscale, 1);
	glutSolidSphere(outer*0.9, 20, 20);
	glPopMatrix;

	glPushMatrix; 
	glColor3f(0.7f, 0.5f, 0.1f);
	glRotatef(-90, 1, 0, 0);
	glRotatef(90, 0, 1, 0);
	glTranslatef(0, outer, 0);
	glutSolidTeapot(1);
	glPopMatrix;

	glPushMatrix; /* z left layerr*/
	glColor3f(0.6f, 0.2f, 0.05f);
	glTranslatef(0.0, 0.0, -0.5);
	glutSolidSphere(inner,20,20);
	glTranslatef(0.0, 0.0, 1.0);
	glColor3f(0.5f, 0.35f, 0.05f);
	glutSolidSphere(inner, 20, 20);
	glPopMatrix;



	glPushMatrix; /* z right layerr*/
	glColor3f(0.6f, 0.2f, 0.15f);
	glTranslatef(0.0, 0.0, -0.5);
	glutWireSphere(inner, 20, 20);
	glTranslatef(0.0, 0.0, 0.5);
	glColor3f(0.5f, 0.35f, 0.05f);
	glutWireSphere(inner, 20, 20);
	glPopMatrix;
	glColor3f(0.9, 0.7, 0.3);

	
	glScalef(2, 0.8, 1);
	glPopMatrix();

	//glPushMatrix(); /* the tail and neck cubes*/
	//glColor3f(0.7, 0.7, 0.7);
	//glTranslatef(0.0, inner + outer, 0.0);
	//glPopMatrix();

	//glPushMatrix();
	//glColor3f(0.7, 0.7, 0.7);
	//glTranslatef(0.0, -(inner+ outer), 0.0);
	//glPopMatrix();


	glPopMatrix();
	
	glPushMatrix(); /* head draw*/
	glTranslatef(0.0, outer+ 2*inner, 0.0);
	glColor3f(0.0, 0.8, 0.9);
	glutSolidSphere(inner, 20, 20);
	glTranslatef(inner/2, inner*0.8, 0.0);
	glColor3f(0.1, 0.1, 0.1);
	glutSolidSphere(inner/5, 20, 20);
	glTranslatef(-inner, 0.0, 0.0);
	glColor3f(0.1, 0.1, 0.1);
	glutSolidSphere(inner/5, 20, 20);


	glPopMatrix();

}

static void arm_draw(GLfloat width, GLfloat height)
{
	GLUquadricObj *quadratic;
	quadratic = gluNewQuadric();
	glPushMatrix();
	//glutWireOctahedron();
	glutSolidSphere((width / 4), 10, 10);

	glTranslatef(height / 4, 0.0, 0.0);
	//glTranslatef((width / 2), 0.0, 0.0);
	glPushMatrix();
	glRotatef(-90.0, 0.0, 1.0, 0.0);
	glutWireCone((width / 2), (height / 4), 10, 10);
	glPopMatrix();
	//glTranslatef((height / 4), 0.0, 0.0);
	glPushMatrix();
	glRotatef(90.0, 0.0, 1.0, 0.0);
	gluCylinder(quadratic, (width / 2), (width / 2), height / 2, 10, 10);
	glPopMatrix();
	//glTranslatef(1, 0.0, 0.0);
	glTranslatef((height / 2), 0.0, 0.0);
	glPushMatrix();
	glRotatef(90.0, 0.0, 1.0, 0.0);
	glutWireCone((width / 2), (height / 4), 20, 20);
	glPopMatrix();
	glTranslatef(height / 4, 0.0, 0.0);
	glutSolidSphere((width / 4), 10, 10);
	glPopMatrix();

}

static void part_draw(GLfloat width, GLfloat height,int number)
{
	/* right side */
	glColor3f(1, 0.7, 0.1);
	glPushMatrix();
	glTranslatef(height / 4, 0.0, 0.0);
	glRotatef((GLfloat)shoulder_angle[yaxis][number], 0.0, 1.0, 0.0);
	glRotatef((GLfloat)shoulder_angle[zaxis][number], 0.0, 0.0, 1.0);
	 glRotatef((GLfloat)shoulder_angle[xaxis][number], 1.0, 0.0, 0.0);



	arm_draw(width, height);

	/*glPushMatrix();
	glScalef(1.0, 0.2, 1.0);
	glutSolidSphere(0.5, 20, 20);
	glPopMatrix() */;

	glColor3f(0.5, 0.3, 0.7);

	glTranslatef(height, 0.0, 0.0);
	 glRotatef((GLfloat)elbow_angle[xaxis][number], 1.0, 0.0, 0.0);
	 glRotatef((GLfloat)elbow_angle[yaxis][number], 0.0, 1.0, 0.0);
	 glRotatef((GLfloat)elbow_angle[zaxis][number], 0.0, 0.0, 1.0);


	/*glPushMatrix();
	glScalef(1.0, 0.2, 1.0);
	glutWireSphere(0.5, 20, 20);
	glPopMatrix(); */
	arm_draw(width, height);

	glColor3f(0.5, 0.3, 0.7);

	glTranslatef(height, 0.0, 0.0);
	glRotatef((GLfloat)wrist_angle[xaxis][number], 1.0, 0.0, 0.0);
	glRotatef((GLfloat)wrist_angle[yaxis][number], 0.0, 1.0, 0.0);	
	glRotatef((GLfloat)wrist_angle[zaxis][number], 0.0, 0.0, 1.0);
	
	/*glPushMatrix();
	glScalef(1.0, 0.2, 1.0);
	glutWireSphere(0.5, 20, 20);
	glPopMatrix(); */
	//glutSolidSphere(0.1, 0.1, 0.1);
	glPushMatrix();
	glTranslatef(height/10, 0.0, 0.0);
	glScalef(0.4, 0.4, 0.4);
	glRotatef(180, 1.0, 0.0, 0.0);
	glColor3f(0.9, 0.9, 0.9);
	glutSolidTetrahedron();	
	//glutSolidCone(0.4,0.8,10,10);

	glPopMatrix();

	glPopMatrix();

}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1, 0.1, 1);

	glViewport(0, 0, (GLsizei)window_width, (GLsizei)window_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//glOrtho(-250.0, 250.0, -250.0, 250.0, -1.0, 1.0);
	//zoom_factor = MIN(MAX(zoom_factor, 0.1), 10);
	gluPerspective(65.0 * zoom_factor, (GLfloat)window_width / (GLfloat)window_height, 1.0, 51.0);

	/* model view matrix operations*/
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(0.0, 0.0, -10);

	view_distance = MAX(MIN(view_distance, 100), -100);
	GLfloat rad = (-10 + view_distance);
	//cout << "\n xvec yvec \t";
	//cout << xvec<<" ";
	//cout << yvec<<")";
	GLfloat zvec = rad *(cos(sqrt(xvec *xvec + yvec*yvec)));


	gluLookAt(rad * sin(xvec) , rad * sin(yvec) , zvec, xpan, ypan, 0, 0, 1, 0);


	GLfloat width = 0.3;
	GLfloat height = 1;

	//gluLookAt(0, 0, -view_distance, 0, 0, -1, 0, 1, 0);



	glTranslatef(0, x_offset/100, 0);
	if (current_mode == walk)
	{
		load_walk_angles(x_offset, 0);
	}
	glPushMatrix();
	
	glTranslatef(1.1, 0.0, 0.0);
	part_draw(width, height, 2);

	glTranslatef(-0.25, 1.1, 0.0);
	part_draw(width, height *1.2, 1);

	glTranslatef(0.1, -2.2, 0.0);
	part_draw(width, height,3);

	glPopMatrix();


	glPushMatrix();
	glScalef(-1, 1, 1);
	glTranslatef(1.1, 0.0, 0.0);
	part_draw(width, height, 5);

	glTranslatef(-0.25, 1.1, 0.0);
	part_draw(width , height*1.2,4);

	glTranslatef(0.1, -2.2, 0.0);
	part_draw(width, height,6);

	glPopMatrix();

	body_draw();

	/*glTranslatef(0.0, -2.0, 0.0);
	part_draw(width / 2, height / 2);
*/
	glutSwapBuffers();
}

void reshape(int w, int h)
{
	window_width = w;
	window_height = h;
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//glOrtho(-250.0, 250.0, -250.0, 250.0, -1.0, 1.0);

	gluPerspective(65.0 * zoom_factor, (GLfloat)w / (GLfloat)h, 1.0, 20.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -5);

	/*glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-125.0, 125.0, -125.0, 125.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); *//**/




}

//void processSpecialKeys(unsigned char key, int xin, int yin)
//{
//	switch (key) {
//	case GLUT_KEY_RIGHT:{
//	
//	}
//	}
//}

void keyboard(unsigned char key, int xin, int yin)
{
	switch (key) {
	case 's':   /*  s key rotates at shoulder_angle[current_i]  */
		current_joint = shoulder;
		if ((shoulder_angle[rot_axis][current_i] < theta_lim))
		{
			shoulder_angle[rot_axis][current_i] = (shoulder_angle[rot_axis][current_i] + 5) % 360;
			glutPostRedisplay();
		}
		break;
	case 'S':
		current_joint = shoulder;
		if ((shoulder_angle[rot_axis][current_i] > -theta_lim))
		{
			shoulder_angle[rot_axis][current_i] = (shoulder_angle[rot_axis][current_i] - 5) % 360;

			glutPostRedisplay();
		}
		break;
	case 'e':  /*  e key rotates at elbow_angle[current_i]  */
		current_joint = elbow;
		if ((elbow_angle[rot_axis][current_i] < theta_lim))
		{
			elbow_angle[rot_axis][current_i] = (elbow_angle[rot_axis][current_i] + 5) % 360;
			glutPostRedisplay();

		}

		break;
	case 'E':
		current_joint = elbow;
		if ((elbow_angle[rot_axis][current_i] > -theta_lim))
		{
			elbow_angle[rot_axis][current_i] = (elbow_angle[rot_axis][current_i] - 5) % 360;
			glutPostRedisplay();

		}
		break;
	case 'h':
		current_joint = head;

		break;
	case 'd':  /*  e key rotates at elbow_angle[current_i]  */
		current_mode = eye_move;
		break;
	case 'm':  /*  e key rotates at elbow_angle[current_i]  */
		current_mode = zoom;
		break;
	case 'w':
		current_joint = wrist;
		if ((wrist_angle[rot_axis][current_i] < theta_lim))
		{
			wrist_angle[rot_axis][current_i] = (wrist_angle[rot_axis][current_i] + 5) % 360;
			glutPostRedisplay();
		}
		break;
	case 'W':
		current_joint = wrist;
		if ((wrist_angle[rot_axis][current_i] > -theta_lim))
		{
			wrist_angle[rot_axis][current_i] = (wrist_angle[rot_axis][current_i] - 5) % 360;
			glutPostRedisplay();
		}
		break;
	case '1':
		current_i = 1;
		break;
	case '2':
		current_i = 2;
		break;
	case '3':
		current_i = 3;
		break;

	case '4':
		current_i = 4;
		break;

	case '5':
		current_i = 5;
		break;

	case '6':
		current_i = 6;
		break;

	case '0':
		current_i = 0;
		break;
	case 'x':
		rot_axis = xaxis; /* axis name*/
		break;
	case 'y':
		rot_axis = yaxis;
		break;
	case 'z':
		rot_axis = zaxis;
		break;
	case 'r':
		current_mode = rotat;
		break;
	case 'p':
		current_mode = pan;
		break;
	case 'q':
		current_mode = walk;
		glutIdleFunc(WalkDisplay);

		break;
	case 'g':
		current_mode = go;
		break;

	case 'o':
		memset(elbow_angle, 0, sizeof(elbow_angle));
		memset(shoulder_angle, 0, sizeof(shoulder_angle));
		memset(wrist_angle, 0, sizeof(wrist_angle));
		load_default_angles();
		x_offset = 0;
		 view_distance = 0;
		 zoom_factor = 1;
		 current_mode = free_move;
		current_i = 1;
		lat_xdiff = 0;
		lat_ydiff = 0;
		xvec = 0;
		yvec = 0;
		xvec_p = 0;
		yvec_p = 0;
		xpan = 0;
		ypan = 0;

		glutPostRedisplay();

		break;
	default:

		if (current_i > 7) current_i = 1;
		break;
	}
}


void mouseCallBack(int button, int state, int x, int y)
{
	{
		if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
		{
			yhome = y;
			xhome = x;
			xdiff = 0;
			ydiff = 0;
			
			glutPostRedisplay();
		}
		else if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN))
		{
			yhome = y;
			xhome = x;
			xdiff = 0;
			ydiff = 0;

			glutPostRedisplay();
		}
		else if (((button == GLUT_MIDDLE_BUTTON) && (state == GLUT_DOWN)))
		{
			current_mode = walk;
			glutIdleFunc(WalkDisplay);
		}
		else if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_UP))
		{

			//cout << "adding to lat diff" <<endl;
			xdiff = -(x - xhome);
			ydiff = -(y - yhome);

			if (current_mode == zoom)
			{
				lat_xdiff += xdiff;
				lat_ydiff += ydiff;
			}
			

			if (current_mode == rotat)
			{
				//cout << "stuff;";
			/*	cout << xdiff<<endl;
				cout << ydiff << endl << endl << endl;*/

				xvec_p += ((GLfloat)xdiff * 5) / window_width;
				yvec_p += ((GLfloat)ydiff * 5) / window_height;
				//glutIdleFunc(display);
			}

			xdiff = 0;
			ydiff = 0;
			glutPostRedisplay();
		}
		else if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_UP))
		{
			xdiff = (x - xhome);
			ydiff = (y - yhome);

			lat_xdiff += xdiff;
			lat_ydiff += ydiff;

			xdiff = 0;
			ydiff = 0;

			glutPostRedisplay();
		}
		else
		{
			glutIdleFunc(NULL);

		}
	}
}
GLfloat sigm(GLint diff)
{

	GLfloat t = (GLfloat)diff / (window_width*5);
	return (t / sqrt(1 + t*t));
}


void motionCallBack(int x, int y)
{
	//cout << "( " << x << " " << y << " )\t";
	//cout << "( " << x_home << " " << y_home << " )\t";
	//cout << "( " << x_other << " " << y_other << " )\n";
	{
		/*	xdiff = x - xhome;
			ydiff = y - yhome;*/
		xdiff = -(x - xhome);
		ydiff = -(y - yhome);
	//	cout << current_mode<<endl;
		if (current_mode == eye_move)
		{
			view_distance =((GLfloat)xdiff) / 50;
			/*cout << "xdiff,ydiff - ( " << xdiff << " " << xdiff << " )\t";
			cout << "lat_xdiff, - ( " << lat_xdiff << " )\n";*/

		}
		if (current_mode == zoom)
		{
			zoom_factor = zoom_factor * (1 + sigm(xdiff));
			zoom_factor = MIN(MAX(zoom_factor, 0.001), 2.5);
		//	cout << zoom_factor << endl;
			//glutIdleFunc(display);
		}
		if (current_mode == rotat)
		{
	
			xvec = xvec_p+ ((GLfloat)xdiff * 5)/window_width;
			yvec = yvec_p + ((GLfloat)ydiff * 5) / window_height;
			//glutIdleFunc(display);
		}
		if (current_mode == pan)
		{

			xpan = (GLfloat)xdiff/abs(view_distance -10) ;
			ypan = (GLfloat)ydiff/abs(view_distance - 10);
			//glutIdleFunc(display);
		}

		glutPostRedisplay();


	}
}
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA |GLUT_DEPTH);
	glutInitWindowSize(window_width, window_height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Tharun Battula - Homework 4	");

	init();
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glCullFace(GL_CULL_FACE);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouseCallBack);
	glutMotionFunc(motionCallBack);

	//glutSpecialFunc(processSpecialKeys);
	glutMainLoop();
	return 0;
}