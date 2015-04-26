#include <windows.h>  
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <time.h>       /* time */

#include <iostream>
#include "clip.h"

using namespace std;

typedef enum { Left, Right, Bottom, Top } Boundary;
const GLint nClip = 4;

void set_boudaries(GLfloatPoint *pt)
{ 
	if (pt->x < 0) pt->x = 0;
	if (pt->y < 0) pt->y = 0;
	if (pt->x > ImageW) pt->x = ImageW;
	if (pt->y > ImageH) pt->y = ImageH;
}

GLint inside(GLfloatPoint point, GLint b, GLintPoint wMin, GLintPoint wMax)
{
	switch (b) {
	case Left:   if (point.x < wMin.x) return (false); break;
	case Right:  if (point.x > wMax.x) return (false); break;
	case Bottom: if (point.y < wMin.y) return (false); break;
	case Top:    if (point.y > wMax.y) return (false); break;
	}
	return (true);
}

GLint cross(GLfloatPoint p1, GLfloatPoint p2, GLint winEdge, GLintPoint wMin, GLintPoint wMax)
{
	if (inside(p1, winEdge, wMin, wMax) == inside(p2, winEdge, wMin, wMax))
		return (false);
	else return (true);
}

GLfloatPoint intersect(GLfloatPoint p1, GLfloatPoint p2, GLint winEdge,
	GLintPoint wMin, GLintPoint wMax)
{
	GLfloatPoint intersect_point;
	GLfloat m;

	if (p1.x != p2.x) m = (p1.y - p2.y) / (p1.x - p2.x);
	switch (winEdge) {
	case Left:
		intersect_point.x = wMin.x;
		intersect_point.y = p2.y + (wMin.x - p2.x) * m;
		break;
	case Right:
		intersect_point.x = wMax.x;
		intersect_point.y = p2.y + (wMax.x - p2.x) * m;
		break;
	case Bottom:
		intersect_point.y = wMin.y;
		if (p1.x != p2.x) intersect_point.x = p2.x + (wMin.y - p2.y) / m;
		else intersect_point.x = p2.x;
		break;
	case Top:
		intersect_point.y = wMax.y;
		if (p1.x != p2.x) intersect_point.x = p2.x + (wMax.y - p2.y) / m;
		else intersect_point.x = p2.x;
		break;
	}

	return (intersect_point);
}

void clip_point(GLfloatPoint point, GLint winEdge, GLintPoint wMin, GLintPoint wMax,
	GLfloatPoint *vertices_out, int * cnt, GLfloatPoint *first, GLfloatPoint *s)
{
	GLfloatPoint intersect_point;
	//GLfloatPoint *first = *first_s;
	//GLfloatPoint *s = *s_s;

	/* If no previous point exists for this clipping boundary,
	* save this point.
	*/
	if ((first[winEdge].x == 0) && (first[winEdge].y == 0))
		first[winEdge] = point;
	else
		/*  Previous point exists.  If point and previous point cross
		*  this clipping boundary, find intersection.  Clip against
		*  next boundary, if any.  If no more clip boundaries, add
		*  intersection to output list.
		*/
		if (cross(point, s[winEdge], winEdge, wMin, wMax)) {
			intersect_point = intersect(point, s[winEdge], winEdge, wMin, wMax);
			if (winEdge < Top)
				clip_point(intersect_point, winEdge + 1, wMin, wMax, vertices_out, cnt, first, s);
			else {
				set_boudaries(&intersect_point);
				vertices_out[*cnt] = intersect_point;  (*cnt)++;
			}
		}

	/*  Save point as most recent point for this clip boundary.  */
	s[winEdge] = point;

	/*  For all, if point inside, proceed to next boundary, if any.  */
	if (inside(point, winEdge, wMin, wMax))
		if (winEdge < Top)
			clip_point(point, winEdge + 1, wMin, wMax, vertices_out, cnt, first, s);
		else {
			vertices_out[*cnt] = point;  (*cnt)++;
		}
}

void last_round_clip(GLintPoint wMin, GLintPoint wMax, GLfloatPoint * vertices_out,
	GLint * cnt, GLfloatPoint *first, GLfloatPoint *s)
{
	GLfloatPoint pt = { 0, 0 };
	GLint winEdge = 0;

	for (winEdge = Left; winEdge <= Top; winEdge++) {
		if (cross(s[winEdge], first[winEdge], winEdge, wMin, wMax)) {
			pt = intersect(s[winEdge], first[winEdge], winEdge, wMin, wMax);
			if (winEdge < Top)
				clip_point(pt, winEdge + 1, wMin, wMax, vertices_out, cnt, first, s);
			else {
				set_boudaries(&pt);
				vertices_out[*cnt] = pt;  (*cnt)++;
				//cout << " output " << pt.x << " " << pt.y<<endl;
			}
		}
	}

}

/* this works only if all of the following are true:
*   1. poly has no colinear edges;
*   2. poly has no duplicate vertices;
*   3. poly has at least three vertices;
*   4. poly is convex (implying 3).
*/
GLint clipping_polygon_algo(GLintPoint wMin, GLintPoint wMax, GLint n, GLfloatPoint * pIn, GLfloatPoint * vertices_out)
{
	/*  Parameter "first" holds pointer to first point processed for
	*  a boundary; "s" holds most recent point processed for boundary.
	*/
	GLfloatPoint first[nClip], s[nClip];
	memset(first, 0, nClip * sizeof(GLfloatPoint));
	memset(s, 0, nClip * sizeof(GLfloatPoint));

	GLint k, cnt = 0;

	for (k = 0; k < n; k++)
		clip_point(pIn[k], Left, wMin, wMax, vertices_out, &cnt, first, s);


	last_round_clip(wMin, wMax, vertices_out, &cnt, first, s);
	return (cnt);
}
