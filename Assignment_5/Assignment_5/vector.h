#ifndef VECTOR_H
#define VECTOR_H
#include <GL/gl.h>
#include "vector.h"
#include "math.h"

struct GLfloatPoint3
{
	GLfloat x, y, z;
};

GLfloat dotProduct(GLfloatPoint3 vec1, GLfloatPoint3 vec2)
{
	return(vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z);
}

GLfloatPoint3 Normalize(GLfloatPoint3 vec1)
{
	GLfloatPoint3 out = vec1;
	GLfloat denom = (dotProduct(vec1, vec1));
	if ((denom != 0) && (denom !=1))
	{
		GLfloat invdenom = sqrt(1 /denom);
		out.x = vec1.x * invdenom;
		out.y = vec1.y * invdenom;
		out.z = vec1.z * invdenom;
	}
	return out;
}

GLfloatPoint3 add(GLfloatPoint3 vec1, GLfloatPoint3 vec2)
{
	GLfloatPoint3 out;
	out.x = vec1.x + vec2.x;
	out.y = vec1.y + vec2.y;
	out.z = vec1.z + vec2.z;
	return out;
}

GLfloatPoint3 sub(GLfloatPoint3 vec1, GLfloatPoint3 vec2)
{
	GLfloatPoint3 out;
	out.x = vec1.x - vec2.x;
	out.y = vec1.y - vec2.y;
	out.z = vec1.z - vec2.z;
	return out;
}

GLfloatPoint3 sprod(GLfloatPoint3 vec1, GLfloat scalar)
{
	GLfloatPoint3 out;
	out.x = vec1.x * scalar;
	out.y = vec1.y * scalar;
	out.z = vec1.z * scalar;
	return out;
}

#endif

