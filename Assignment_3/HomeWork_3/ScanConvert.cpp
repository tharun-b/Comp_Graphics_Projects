#include <windows.h>  
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <time.h>       /* time */

#include <iostream>
#include <fstream>
#include <vector>
#include <list>

#include "clip.h"
/******************************************************************
	Notes:
	Image size is 400 by 400 by default.  You may adjust this if
		you want to.
	You can assume the window will NOT be resized.
	Call clear_frame_buffer to clear the entire framebuffer.
	Call set_frame_buffer to set a pixel.  This should be the only
		routine you use to set the color (other than clearing the
		entire framebuffer).  drawit() will cause the current
		framebuffer to be displayed.
	As is, your scan conversion should probably be called from
		within the display function.  There is a very short sample
		of code there now.
	You may add code to any of the subroutines here,  You probably
		want to leave the drawit, clear_frame_buffer, and
		set_frame_buffer commands alone, though.
  *****************************************************************/
using namespace std;

int x_other, y_other;
int x_home, y_home;

int clip_mode = 0; // 1 once 'c' key pressed
typedef GLfloat colour[3];
colour fill_colour = { 1, 1, 1 };

class GLintPointArray
{
public:
	vector<GLfloatPoint>vertices;
	colour poly_colour;
};

float framebuffer[ImageH][ImageW][3];
vector<GLintPointArray> polygons;
GLintPointArray temp_array; 
int polygon_num = 0;


class Node
{
public:
	Node() :yUpper(-1), xIntersect(0.0), dxPerScan(0.0)
	{ };
	int yUpper;
	float xIntersect, dxPerScan;
};

Node empty_node;  // an emverticesy node

class edge_table
{
public:
	void build_table(const GLintPointArray &);
	int y_next(int, vector<GLfloatPoint>);
	void make_edge_record(GLintPoint, GLintPoint, int);
	//void printEdgeTable();

	vector<list<Node> > Edges;
};

list<Node> temp_list;  // an emverticesy list

//***********
void insert_edge(list<Node>& ordered_list, const Node& item)
{
	list<Node>::iterator curr = ordered_list.begin(),
		stop = ordered_list.end();
	while ((curr != stop) && ((*curr).xIntersect < item.xIntersect))
		curr++;
	ordered_list.insert(curr, item);
}
int edge_table::y_next(int k, vector<GLfloatPoint> p)
{
	int j;
	// next subscripts in polygon
	if ((k + 1) >(p.size() - 1))
		j = 0;
	else
		j = k + 1;
	while (p[k].y == p[j].y)
		if ((j + 1) > (p.size() - 1))
			j = 0;
		else
			j++;
	return (p[j].y);
}

void set_frame_buffer(int x, int y, float R, float G, float B)
{
	// changes the origin from the lower-left corner to the upper-left corner
	y = ImageH - 1 - y;

	if (R <= 1.0)
		if (R >= 0.0)
			framebuffer[y][x][0] = R;
		else
			framebuffer[y][x][0] = 0.0;
	else
		framebuffer[y][x][0] = 1.0;
	if (G <= 1.0)
		if (G >= 0.0)
			framebuffer[y][x][1] = G;
		else
			framebuffer[y][x][1] = 0.0;
	else
		framebuffer[y][x][1] = 1.0;
	if (B <= 1.0)
		if (B >= 0.0)
			framebuffer[y][x][2] = B;
		else
			framebuffer[y][x][2] = 0.0;
	else
		framebuffer[y][x][2] = 1.0;
}

void edge_table::make_edge_record(GLintPoint lower, GLintPoint upper,
	int yComp)
{
	Node n;

	n.dxPerScan = (float)(upper.x - lower.x) / (upper.y - lower.y);
	n.xIntersect = lower.x;
	if (upper.y < yComp)
		n.yUpper = upper.y - 1;
	else
		n.yUpper = upper.y;
	insert_edge(Edges[lower.y], n);
}

void edge_table::build_table(const GLintPointArray& polygons)
{
	GLintPoint v1, v2;
	int i, yPrev;

	yPrev = polygons.vertices[polygons.vertices.size() - 2].y;
	v1.x = polygons.vertices[polygons.vertices.size() - 1].x;
	v1.y = polygons.vertices[polygons.vertices.size() - 1].y;
	for (i = 0; i < polygons.vertices.size(); i++)
	{
		v2.x = polygons.vertices[i].x;
		v2.y = polygons.vertices[i].y;

		if (v1.y != v2.y)
		{ // non horizontal edge
			if (v1.y < v2.y)
				make_edge_record(v1, v2, y_next(i, polygons.vertices)); //up edge
			else
				make_edge_record(v2, v1, yPrev); // down edge
		}
		yPrev = v1.y;
		v1 = v2;
	}
}

//
// ************** act_edge_list ROUTINES 
void buildAEL(list<Node> &act_edge_list, list<Node> ET)
{
	list<Node>::iterator iter;

	iter = ET.begin();
	// every Edge table list has a "empty" node at front
	iter++;
	while (iter != ET.end())
	{
		insert_edge(act_edge_list, *iter);
		iter++;
	}
}

void fillScan(int y, list<Node> L, colour C)
{    // want to pull off pairs of x values from adjacent
	// nodes in the list - the y value = scan line
	list<Node>::iterator iter1 = L.begin(), iter2 = L.begin();
	int x1, x_other;
	while (iter1 != L.end())
	{
		iter2 = iter1;
		iter2++;
		if (iter2 == L.end()) break;
		x1 = (int)(*iter1).xIntersect;
		x_other = (int)(*iter2).xIntersect;

		for (int x = x1; x < x_other; x++)
		{
			set_frame_buffer(x, y, C[0], C[1], C[2]);
		}
		
		// move on to next pair of nodes
		iter1 = iter2;
		iter1++;
	}
}

void updateAEL(int y, list<Node>& L)
{   // delete completed edges
	// update the xIntersect field
	list<Node>::iterator iter = L.begin();
	while (iter != L.end())
		if (y >= (*iter).yUpper)
			L.erase(iter++);
		else
		{
			(*iter).xIntersect += (*iter).dxPerScan;
			iter++;
		}
}

void re_sort_AEL(list<Node>& L)
{
	Node n;
	list<Node> L1;
	list<Node>::iterator iter = L.begin();
	/* re sorting */
	while (iter != L.end())
	{
		insert_edge(L1, *iter);
		L.erase(iter++);
	}
	L = L1;
}
// Draws the scene
void drawit(void)
{
   glDrawPixels(ImageW,ImageH,GL_RGB,GL_FLOAT,framebuffer);
   glFlush();
}

// Clears framebuffer to black
void clear_frame_buffer()
{
	int i,j;

	for(i=0;i<ImageH;i++) {
		for (j=0;j<ImageW;j++) {
			framebuffer[i][j][0] = 0.0;
			framebuffer[i][j][1] = 0.0;
			framebuffer[i][j][2] = 0.0;
		}
	}
}

void scan_convert(GLintPointArray P, colour c)
{   // need an edge table and act_edge_list	
	edge_table EdgeTable;
	list<Node> act_edge_list;

	if (P.vertices.size() == 0) return;
	temp_list.clear();
	temp_list.push_front(empty_node); 
	// build the edge table - need the window size
	for (int i = 0; i < ImageH; i++)
	EdgeTable.Edges.push_back(temp_list);
	EdgeTable.build_table(P);
	// if needed - print the table here
	// EdgeTable.printEdgeTable();
	// filling requires building and using act_edge_list

	for (int scanLine = 0; scanLine < ImageH; scanLine++)
	{    // could add output data on table
		//outFile <<"Scan line: " << scanLine << endl;
		buildAEL(act_edge_list, EdgeTable.Edges[scanLine]);
		if (!act_edge_list.empty())
		{    // if needed print the table
			//writeListInfo(act_edge_list);
			fillScan(scanLine, act_edge_list,P.poly_colour);
			updateAEL(scanLine, act_edge_list);
			re_sort_AEL(act_edge_list);
		}
	}
}


void display(void)
{
	//The next two lines of code are for demonstration only.
	//They show how to draw a line from (0,0) to (100,100)
	int i;
	//for (i = 0; i <= 100; i++) set_frame_buffer(i, i, 1.0, 1.0, 1.0);
	clear_frame_buffer();
	vector<GLintPointArray>::iterator poly_it;
	vector<GLfloatPoint>::iterator vert_it;

	GLintPoint wMin, wMax;

	wMin.x = MIN(x_home, x_other);
	wMin.y = MIN(y_home, y_other);
	wMax.x = MAX(x_home, x_other);
	wMax.y = MAX(y_home, y_other);

	wMin.x = MAX(0, wMin.x);
	wMin.y = MAX(0, wMin.y);
	wMax.x = MIN(wMax.x, ImageW);
	wMax.y = MIN(wMax.y, ImageH);

	vector<GLintPointArray> polygons_new;
	GLfloatPoint temp_out[40];
	GLint out_count;
	GLintPointArray temp_1;

	/* clipping works only if all of the following are true:
	*   1. poly has no colinear edges;
	*   2. poly has no duplicate vertices;
	*   3. poly has at least three vertices;
	*   4. poly is convex (implying 3).
	*/
	/*******************************************************/
	/*						 Main clipping wrapper          */
	/*******************************************************/

	memset(temp_out, 0, 40 * sizeof(GLfloatPoint));
	if (clip_mode == 2) /* clip polygons to the decided window */
	{
		for (poly_it = polygons.begin(), i = 0; poly_it != polygons.end(); ++poly_it, i++)
		{
			memset(temp_out, 0, 40 * sizeof(GLfloatPoint));
			out_count = clipping_polygon_algo(wMin, wMax, poly_it->vertices.size(), &(poly_it->vertices.at(0)), temp_out);
			temp_1.vertices.clear();
			for (int j = 0; j < out_count; j++)
			{
				temp_1.vertices.push_back(temp_out[j]);
			}
			//temp_1.poly_colour[0] = poly_it->poly_colour[0];
			//temp_1.poly_colour[1] = poly_it->poly_colour[1];
			//temp_1.poly_colour[2] = poly_it->poly_colour[2];
			polygons_new.push_back(temp_1);

			polygons_new.back().poly_colour[0] = poly_it->poly_colour[0];
			polygons_new.back().poly_colour[1] = poly_it->poly_colour[1];
			polygons_new.back().poly_colour[2] = poly_it->poly_colour[2];

		}
	}
	/*                       Clip Mode based     */
	vector<GLintPointArray> *final;
	if (clip_mode == 2)
	{
		final = &polygons_new;
		/* for clipping mode new polygons( clipped ) are used*/
	}
	else
	{
		final = &polygons;
		/* non clipping mode original polygons are used */
	}

	/* scan conversion algorithm */
	/*******************************************************/
	/*	 Main scan conversiona and colour filling  wrapper */
	/*******************************************************/
	for (poly_it = final->begin(), i = 0; poly_it != final->end(); ++poly_it, i++)
		{

			for (vert_it = poly_it->vertices.begin(); vert_it != poly_it->vertices.end(); ++vert_it)
			{
				//cout << "( " << vert_it->x << " , " << vert_it->y << " ) \t";
				set_frame_buffer(vert_it->x, vert_it->y, fill_colour[0], fill_colour[1], fill_colour[2]);
			}
			//cout << endl;
			scan_convert(*poly_it, fill_colour);
		};

	/*******************************************************/
	/*	 Rectangle drawing for clipping area selection     */
	/*******************************************************/
	//cout << "set flag = " << set_flag << endl;
	if (clip_mode == 1)
	{

		int x, y;
		for (x = wMin.x; x < wMax.x; x++)
		{
			set_frame_buffer(x, wMin.y, fill_colour[0], fill_colour[1], fill_colour[2]);
			set_frame_buffer(x, wMax.y, fill_colour[0], fill_colour[1], fill_colour[2]);
		}
		for (y = wMin.y; y < wMax.y; y++)
		{
			set_frame_buffer(wMin.x, y, fill_colour[0], fill_colour[1], fill_colour[2]);
			set_frame_buffer(wMax.x, y, fill_colour[0], fill_colour[1], fill_colour[2]);
		}
	}
	//cout<<endl<<i<<" ]";
	drawit();
}


void init(void)
{
	fill_colour[0] = 1.0f;
	fill_colour[1] = 1.0f;
	fill_colour[2] = 1.0f;
	x_home = 0;
	y_home = 0;
	x_other = ImageW;
	y_other = ImageH;
	clear_frame_buffer();

}
void keyboardCallBack(unsigned char key, int x, int y)
{
	switch (key) {
	case 'c':
		/*colors[0] = 0.0f;
		colors[1] = 1.0f;
		colors[2] = 1.0f;
		glutpostredisplay();*/
		clip_mode = 1; 
		cout << " Entered clipping mode " << endl;
		break;
	case 'r': /* can be added for reset functionality */
		/*colors[0] = 1.0f;
		colors[1] = 0.0f;
		colors[2] = 1.0f;
		glutPostRedisplay();*/
		clip_mode = 0;
		cout << " Entered reset  " << endl;
		init();
		glutIdleFunc(display);
		break;
	default:
		break;
	}
}

void motionCallBack(int x, int y)
{
	//cout << "( " << x << " " << y << " )\t";
	//cout << "( " << x_home << " " << y_home << " )\t";
	//cout << "( " << x_other << " " << y_other << " )\n";

	if (clip_mode == 1)
	{
		x_other = x;
		y_other = y; 
		glutIdleFunc(display);
	}
}

void mouseCallBack(int button, int state, int x, int y)
{
	if (clip_mode == 0 )
	{
		if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
		{
			GLfloatPoint point;
			point.x = x;
			point.y = y;

			//cout << "[ " << x << " , " << y << " ]----";

			if (polygon_num > 10)
			{
				cout << "The polygons count exceeded 10,\nEnter key 'c' to start clipping" << endl;
				clip_mode = -1; // do not accept anything 
				glutIdleFunc(NULL);
				return;
			}

			temp_array.vertices.push_back(point);
			glutIdleFunc(NULL);
		}
		else if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN))
		{
			GLfloatPoint point;
			point.x = x;
			point.y = y;

			//cout << "[ " << x << " , " <<y << " ]"<<endl;

			temp_array.vertices.push_back(point);
			//cout << "entered right down "<<endl;
			temp_array.poly_colour[0] = (float)rand() / RAND_MAX;
			temp_array.poly_colour[1] = (float)rand() / RAND_MAX;
			temp_array.poly_colour[2] = (float)rand() / RAND_MAX;

			polygons.push_back(temp_array);
			temp_array.vertices.clear();

			polygon_num++;																		
			glutIdleFunc(display);
		}
		else
		{
			glutIdleFunc(NULL);

		}
	}
	else if (clip_mode > 0)
	{
		if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
		{
			clip_mode = 1; 
			//cout << "Area Selection Started" << endl;
			{
				x_home = x;
				y_home = y;
			}
		}
		else if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_UP))
		{
			//cout << "Area Selection Ended" << endl;
			{
				x_other = x;
				y_other = y;
				glutIdleFunc(display);
				clip_mode = 2; 
			}
		}
	}
	else
	{
		glutIdleFunc(NULL);
	}
}


int main(int argc, char** argv)
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
	glutInitWindowSize(ImageW,ImageH);
	glutInitWindowPosition(100,100);
	glutCreateWindow("Tharun Battula - Homework 3");
	init();	
	glutMouseFunc(mouseCallBack);
	glutKeyboardFunc(keyboardCallBack);
	glutMotionFunc(motionCallBack);
	glutDisplayFunc(display);
	glutMainLoop();
	return 0;
}
