#include <stdlib.h>
#include <windows.h>  
#include <GL/gl.h>
#include <GL/glut.h>


void display(void)
{
	/*  clear all pixels  */
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);


	/*  draw color polygon (hexagon) with corners at
	*/

	
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3d(1, 1, 1);
	glBegin(GL_POLYGON);
	glVertex3f(0.375, 0.5, 0.0);
	glVertex3f(0.4375, 0.71650, 0.0);

	/* currently placing evey vertex twice to get the edge in solid color
	Optimized solution might exist
	*/

	glColor3d(0, 1, 0);
	glVertex3f(0.4375, 0.71650, 0.0);
	glVertex3f(0.5625, 0.71650, 0.0);
	
	glColor3d(1, 0.5, 0);
	glVertex3f(0.5625, 0.71650, 0.0);
	glVertex3f(0.625, 0.5, 0.0);
	
	glColor3f(0.5f, 0.35f,0.05f );
	glVertex3f(0.625, 0.5, 0.0);
	glVertex3f(0.5625, 0.28349, 0.0);
	glColor3d(1, 1, 0);
	glVertex3f(0.5625, 0.28349, 0.0);
	glVertex3f(0.4375, 0.28349, 0.0);
	glColor3d(1, 0, 1);
	glVertex3f(0.4375, 0.28349, 0.0);
	glVertex3f(0.375, 0.5, 0.0);
	glEnd();

	/*  don't wait!
	*  start processing buffered OpenGL routines
	*/
	glFlush();
	glutSwapBuffers();

}

void init(void)
{
	/*  select clearing (background) color       */
	glClearColor(0.0, 0.0, 0.0, 0.0);

	/*  initialize viewing values  */
	glMatrixMode(GL_PROJECTION);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_LIGHTING);
	//glEnable(GL_BLEND);
	//glEnable(GL_TEXTURE_2D);

	glLoadIdentity();
	glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);

}

/*
*  Declare initial window size, position, and display mode
*  (single buffer and RGBA).  Open window with "hello"
*  in its title bar.  Call initialization routines.
*  Register callback function to display graphics.
*  Enter main loop and process events.
*/
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(800, 400);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Tharun Battula - Assignement 1");
	init();
	glutDisplayFunc(display);
	glutMainLoop();
	return 0;   /* ISO C requires main to return int. */
}