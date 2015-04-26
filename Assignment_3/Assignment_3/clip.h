#ifndef CLIP_H
#define CLIP_H

#define ImageW 400
#define ImageH 400
#define MIN(X,Y) ((X<Y)?X:Y)
#define MAX(X,Y) ((X>Y)?X:Y)

struct GLintPoint
{
	GLint x, y;
};


struct GLfloatPoint
{
	GLfloat x, y;
};

GLint clipping_polygon_algo(GLintPoint wMin, GLintPoint wMax, GLint n, GLfloatPoint * pIn, GLfloatPoint * pOut);

#endif
