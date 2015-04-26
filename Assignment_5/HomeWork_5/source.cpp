#include <windows.h>  
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <time.h>       /* time */

#include <vector>
#include <list>
#include <iostream>
#include <fstream>
#include "vector.h"

using namespace std;

/*******************************************************%***********
Notes:
This is the same utility as in the earlier homework assignment.
Image size is 400 by 400 by default.  You may adjust this if
you want to.
You can assume the window will NOT be resized.
Call clearFramebuffer to clear the entire framebuffer.
Call setFramebuffer to set a pixel.  This should be the only
routine you use to set the color (other than clearing the
entire framebuffer).  drawit() will cause the current
framebuffer to be displayed.
As is, your ray tracer should probably be called from
within the display function.  There is a very short sample
of code there now.  You can replace that with a call to a
function that raytraces your scene (or whatever other
interaction you provide.
You may add code to any of the subroutines here,  You probably
want to leave the drawit, clearFramebuffer, and
setFramebuffer commands alone, though.
*****************************************************************/

#define ImageW 400
#define ImageH 400


#define SceneW 800
#define SceneH 800
#define SceneT 800

#define AMBIENT_LIGHT   0.1
#define SPECTRAL_PARAM  10 
#define TRACE_LIMIT 8
#define REFLECTION_ENABLE 1

BOOL isImageCalculated = FALSE;
int scene_num = 0;
struct Colour {
	GLfloat r, g, b;		// Color (R,G,B values)
};

struct coeffcients {
	GLfloat kAMB = 0.3;
	GLfloat kDIFF = 0.4;
	GLfloat kSPEC = 0.3;
};

struct constraints {
	GLfloatPoint3 min[3];
	GLfloatPoint3 max[3];
};

struct surface_properties
{
	coeffcients coeffs;
	GLint shininess = 50;
	GLfloat gamma_e = 0.3;
	BOOL isLight = FALSE;

};



class Plane
{
public:
	GLfloatPoint3 Normal;
	GLfloatPoint3 Centre;
	Colour color;
	surface_properties properties;

};

class Sphere
{
public:
	GLfloat radius;
	GLfloatPoint3 Centre;
	Colour color;
	surface_properties properties;
};

class Light
{
public:
	GLfloatPoint3 Centre;
	Colour color;
};



class Shapes_3D {
public:
	Plane Faces;
};

vector<Plane> *Plane_list = new vector<Plane>;
vector<Sphere>*Sphere_list = new vector<Sphere>;
vector<Light> *Light_list = new vector<Light>;

struct Ray
{
	GLfloatPoint3 srcPoint;
	GLfloatPoint3 dir_diff;
};


float framebuffer[ImageH][ImageW][3];

// Draws the scene
void createScene(int scen)
{
	Plane temp_plane;
	Light temp_light;
	Sphere temp_sphere;

	Plane_list->clear();
	Sphere_list->clear();
	Light_list->clear();

	temp_sphere.properties.isLight = FALSE;

	switch (scen)
	{
	case 1:
		temp_plane.properties.gamma_e = 0.2;
		/* 0 Back ground plane */
		//1

		/* Upper Face plane */
		temp_plane.Normal.x = 0;
		temp_plane.Normal.y = -1;
		temp_plane.Normal.z = 0;
		temp_plane.Normal = Normalize(temp_plane.Normal);

		temp_plane.Centre.x = 0;
		temp_plane.Centre.y = (SceneH >> 1);
		temp_plane.Centre.z = 0;

		temp_plane.color.r = 0.8;
		temp_plane.color.g = 0.8;
		temp_plane.color.b = 0.9;

		Plane_list->push_back(temp_plane);

		//2
		/* lower Face plane */
		temp_plane.properties.gamma_e = 0;

		temp_plane.Normal.x = 0;
		temp_plane.Normal.y = 1;
		temp_plane.Normal.z = 0;
		temp_plane.Normal = Normalize(temp_plane.Normal);

		temp_plane.Centre.x = 0;
		temp_plane.Centre.y = -(SceneH >> 1);
		temp_plane.Centre.z = 0;

		temp_plane.color.r = 0.7;
		temp_plane.color.g = 0.35;
		temp_plane.color.b = 0.0;
		/*temp_plane.properties.coeffs.kAMB = 0.2;
		temp_plane.properties.coeffs.kDIFF = 0.2;
		temp_plane.properties.coeffs.kSPEC = 0.5;*/

		Plane_list->push_back(temp_plane);

		temp_plane.properties.coeffs.kAMB = 0.2;
		temp_plane.properties.coeffs.kDIFF = 0.4;
		temp_plane.properties.coeffs.kSPEC = 0.6;
		//3


		/* sphere setup */
		temp_sphere.radius = (SceneW / 6);

		temp_sphere.Centre.x = -100;
		temp_sphere.Centre.y = 0;
		temp_sphere.Centre.z = 0;

		temp_sphere.color.r = 0.0;
		temp_sphere.color.g = 0.25;
		temp_sphere.color.b = 0.1;
		temp_sphere.properties.gamma_e = 0.2;

		temp_sphere.properties.coeffs.kDIFF = 0.5;
		temp_sphere.properties.coeffs.kSPEC = 0.1;

		temp_sphere.color.g = 0.25;
		temp_sphere.color.b = 0.1;
		Sphere_list->push_back(temp_sphere);

		temp_sphere.radius = (SceneW / 5);

		temp_sphere.Centre.x = +(SceneW / 2) - 90;
		temp_sphere.Centre.y = 0;
		temp_sphere.Centre.z = (ImageW / 2);

		temp_sphere.color.r = 0.8;
		temp_sphere.color.g = 0.9;
		temp_sphere.color.b = 0.1;
		temp_sphere.properties.coeffs.kDIFF = 0.2;
		temp_sphere.properties.coeffs.kSPEC = 0.9;
		Sphere_list->push_back(temp_sphere);

		temp_sphere.properties.gamma_e = 0.5;
		temp_sphere.radius = (SceneW / 4);
		temp_sphere.properties.gamma_e = 0.2;

		temp_sphere.Centre.x = -(SceneW / 4);
		temp_sphere.Centre.y = -(SceneH / 4);
		temp_sphere.Centre.z = +(SceneT >> 3);

		temp_sphere.color.r = 0.1;
		temp_sphere.color.g = 0.05;
		temp_sphere.color.b = 0.15;
		temp_sphere.properties.coeffs.kDIFF = 0.1;
		temp_sphere.properties.coeffs.kSPEC = 0.5;
		Sphere_list->push_back(temp_sphere);




		temp_sphere.radius = (SceneW / 10);
		temp_sphere.properties.gamma_e = 0.05;

		temp_sphere.Centre.x = 0;
		temp_sphere.Centre.y = -(SceneH / 4);
		temp_sphere.Centre.z = +(SceneT >> 3) + 10;

		temp_sphere.color.r = 1;
		temp_sphere.color.g = 0;
		temp_sphere.color.b = 0;
		temp_sphere.properties.coeffs.kDIFF = 0.2;
		temp_sphere.properties.coeffs.kSPEC = 0.6;
		Sphere_list->push_back(temp_sphere);


		/* Lights*/
		temp_sphere.properties.gamma_e = 0;
		temp_sphere.properties.isLight = TRUE;
		temp_light.Centre.x = (SceneW >> 2);
		temp_light.Centre.y = (SceneH >> 1);
		temp_light.Centre.z = (SceneT);

		temp_light.color.r = 1;
		temp_light.color.g = 1;
		temp_light.color.b = 1;

		temp_sphere.radius = (SceneW >> 5);
		temp_sphere.Centre.x = temp_light.Centre.x;
		temp_sphere.Centre.y = temp_light.Centre.y;
		temp_sphere.Centre.z = temp_light.Centre.z;
		temp_sphere.properties.coeffs.kAMB = 1;
		temp_sphere.properties.coeffs.kDIFF = 1;
		temp_sphere.properties.coeffs.kSPEC = 0;

		temp_sphere.color.r = 1;
		temp_sphere.color.g = 1;
		temp_sphere.color.b = 1;


		Light_list->push_back(temp_light);
		Sphere_list->push_back(temp_sphere);

		temp_sphere.properties.gamma_e = 0;
		temp_sphere.properties.isLight = TRUE;
		temp_light.Centre.x = (SceneW);
		temp_light.Centre.y = (SceneH);
		temp_light.Centre.z = (SceneT);

		temp_light.color.r = 1;
		temp_light.color.g = 1;
		temp_light.color.b = 1;

		temp_sphere.radius = (SceneW >> 5);
		temp_sphere.Centre.x = temp_light.Centre.x;
		temp_sphere.Centre.y = temp_light.Centre.y;
		temp_sphere.Centre.z = temp_light.Centre.z;
		temp_sphere.properties.coeffs.kAMB = 1;
		temp_sphere.properties.coeffs.kDIFF = 1;
		temp_sphere.properties.coeffs.kSPEC = 0;

		temp_sphere.color.r = 1;
		temp_sphere.color.g = 1;
		temp_sphere.color.b = 1;


		Light_list->push_back(temp_light);
		Sphere_list->push_back(temp_sphere);

		temp_light.Centre.x = -(SceneW >> 2);
		temp_light.Centre.y = (SceneH >> 1)-400;
		temp_light.Centre.z = (SceneT -120);

		temp_light.color.r = 1;
		temp_light.color.g = 1;
		temp_light.color.b = 1;

		temp_sphere.radius = (SceneW >> 5);
		temp_sphere.Centre.x = temp_light.Centre.x;
		temp_sphere.Centre.y = temp_light.Centre.y;
		temp_sphere.Centre.z = temp_light.Centre.z;
		temp_sphere.properties.coeffs.kAMB = 1;
		temp_sphere.properties.coeffs.kDIFF = 1;
		temp_sphere.properties.coeffs.kSPEC = 0;

		temp_sphere.color.r = 1;
		temp_sphere.color.g = 1;
		temp_sphere.color.b = 1;

		Light_list->push_back(temp_light);
		Sphere_list->push_back(temp_sphere);
		break;
	case 0:
	default:
		temp_plane.properties.gamma_e = 0.2;
		/* 0 Back ground plane */
		temp_plane.Normal.x = 0;
		temp_plane.Normal.y = 0;
		temp_plane.Normal.z = 1;
		temp_plane.Normal = Normalize(temp_plane.Normal);

		temp_plane.Centre.x = 0;
		temp_plane.Centre.y = 0;
		temp_plane.Centre.z = -(SceneT / 2);

		temp_plane.color.r = 0.1;
		temp_plane.color.g = 0.7;
		temp_plane.color.b = 0.6;


		Plane_list->push_back(temp_plane);
		//1
		temp_plane.properties.gamma_e = 0.05;

		/* Upper Face plane */
		temp_plane.Normal.x = 0;
		temp_plane.Normal.y = -1;
		temp_plane.Normal.z = 0;
		temp_plane.Normal = Normalize(temp_plane.Normal);

		temp_plane.Centre.x = 0;
		temp_plane.Centre.y = (SceneH >> 1);
		temp_plane.Centre.z = 0;

		temp_plane.color.r = 0.35;
		temp_plane.color.g = 0.75;
		temp_plane.color.b = 0.9;

		Plane_list->push_back(temp_plane);

		//2
		/* lower Face plane */
		temp_plane.properties.gamma_e = 0;

		temp_plane.Normal.x = 0;
		temp_plane.Normal.y = 1;
		temp_plane.Normal.z = 0;
		temp_plane.Normal = Normalize(temp_plane.Normal);

		temp_plane.Centre.x = 0;
		temp_plane.Centre.y = -(SceneH >> 1);
		temp_plane.Centre.z = 0;

		temp_plane.color.r = 0.7;
		temp_plane.color.g = 0.35;
		temp_plane.color.b = 0.0;
		/*temp_plane.properties.coeffs.kAMB = 0.2;
		temp_plane.properties.coeffs.kDIFF = 0.2;
		temp_plane.properties.coeffs.kSPEC = 0.5;*/

		Plane_list->push_back(temp_plane);

		temp_plane.properties.coeffs.kAMB = 0.2;
		temp_plane.properties.coeffs.kDIFF = 0.2;
		temp_plane.properties.coeffs.kSPEC = 0.6;
		//3
		/* right side Face plane */
		temp_plane.properties.gamma_e = 1;

		temp_plane.Normal.x = -1;
		temp_plane.Normal.y = 0;
		temp_plane.Normal.z = 0;
		temp_plane.Normal = Normalize(temp_plane.Normal);

		temp_plane.Centre.x = (SceneW >> 1);
		temp_plane.Centre.y = 0;
		temp_plane.Centre.z = 0;

		temp_plane.color.r = 0.75;
		temp_plane.color.g = 0.9;
		temp_plane.color.b = 0.8;

		Plane_list->push_back(temp_plane);
		//4
		temp_plane.properties.gamma_e = 0.15;
		temp_plane.properties.coeffs.kAMB = 0.3;
		temp_plane.properties.coeffs.kDIFF = 0.4;
		temp_plane.properties.coeffs.kSPEC = 0.1;

		temp_plane.Normal.x = 1;
		temp_plane.Normal.y = 0;
		temp_plane.Normal.z = 0;
		temp_plane.Normal = Normalize(temp_plane.Normal);

		temp_plane.Centre.x = -(SceneW / 2);
		temp_plane.Centre.y = 0;
		temp_plane.Centre.z = 0;

		temp_plane.color.r = 0.9;
		temp_plane.color.g = 0.85;
		temp_plane.color.b = 0.9;

		Plane_list->push_back(temp_plane);


		/* sphere setup */
		temp_sphere.properties.isLight = FALSE;
		temp_sphere.radius = (SceneW / 4);

		temp_sphere.Centre.x = -(SceneW / 2);
		temp_sphere.Centre.y = 0;
		temp_sphere.Centre.z = 0;

		temp_sphere.color.r = 0.0;
		temp_sphere.color.g = 0.25;
		temp_sphere.color.b = 0.1;
		temp_sphere.properties.gamma_e = 0.2;

		temp_sphere.properties.coeffs.kDIFF = 0.5;
		temp_sphere.properties.coeffs.kSPEC = 0.1;

		temp_sphere.color.g = 0.25;
		temp_sphere.color.b = 0.1;
		Sphere_list->push_back(temp_sphere);

		temp_sphere.radius = (SceneW / 5);

		temp_sphere.Centre.x = +(SceneW / 2) - 90;
		temp_sphere.Centre.y = 0;
		temp_sphere.Centre.z = (ImageW / 2);

		temp_sphere.color.r = 0.1;
		temp_sphere.color.g = 0.9;
		temp_sphere.color.b = 0.1;
		temp_sphere.properties.coeffs.kDIFF = 0.2;
		temp_sphere.properties.coeffs.kSPEC = 0.9;
		Sphere_list->push_back(temp_sphere);

		temp_sphere.radius = (SceneW >> 3);
		temp_sphere.properties.gamma_e = 0.2;

		temp_sphere.Centre.x = -(SceneW / 4);
		temp_sphere.Centre.y = -(SceneH / 4);
		temp_sphere.Centre.z = +(SceneT >> 3);

		temp_sphere.color.r = 0.5;
		temp_sphere.color.g = 0.25;
		temp_sphere.color.b = 0.65;
		temp_sphere.properties.coeffs.kDIFF = 0.1;
		temp_sphere.properties.coeffs.kSPEC = 0.5;
		Sphere_list->push_back(temp_sphere);




		temp_sphere.radius = (SceneW / 10);
		temp_sphere.properties.gamma_e = 0.05;

		temp_sphere.Centre.x = 0;
		temp_sphere.Centre.y = -(SceneH / 4);
		temp_sphere.Centre.z = +(SceneT >> 3) + 10;

		temp_sphere.color.r = 1;
		temp_sphere.color.g = 0;
		temp_sphere.color.b = 0;
		temp_sphere.properties.coeffs.kDIFF = 0.2;
		temp_sphere.properties.coeffs.kSPEC = 0.6;
		Sphere_list->push_back(temp_sphere);





		/* Lights*/
		temp_sphere.properties.gamma_e = 0;
		temp_sphere.properties.isLight = TRUE;
		temp_light.Centre.x = (SceneW >> 2);
		temp_light.Centre.y = (SceneH >> 1);
		temp_light.Centre.z = (SceneT / 3);

		temp_light.color.r = 1;
		temp_light.color.g = 1;
		temp_light.color.b = 1;

		temp_sphere.radius = (SceneW >> 5);
		temp_sphere.Centre.x = temp_light.Centre.x;
		temp_sphere.Centre.y = temp_light.Centre.y;
		temp_sphere.Centre.z = temp_light.Centre.z;
		temp_sphere.properties.coeffs.kAMB = 1;
		temp_sphere.properties.coeffs.kDIFF = 1;
		temp_sphere.properties.coeffs.kSPEC = 0;

		temp_sphere.color.r = 1;
		temp_sphere.color.g = 1;
		temp_sphere.color.b = 1;


		Light_list->push_back(temp_light);
		Sphere_list->push_back(temp_sphere);

		temp_light.Centre.x = -(SceneW >> 2);
		temp_light.Centre.y = (SceneH >> 1);
		temp_light.Centre.z = (SceneT / 3);

		temp_light.color.r = 1;
		temp_light.color.g = 1;
		temp_light.color.b = 1;

		temp_sphere.radius = (SceneW >> 5);
		temp_sphere.Centre.x = temp_light.Centre.x;
		temp_sphere.Centre.y = temp_light.Centre.y;
		temp_sphere.Centre.z = temp_light.Centre.z;
		temp_sphere.properties.coeffs.kAMB = 1;
		temp_sphere.properties.coeffs.kDIFF = 1;
		temp_sphere.properties.coeffs.kSPEC = 0;

		temp_sphere.color.r = 1;
		temp_sphere.color.g = 1;
		temp_sphere.color.b = 1;

		Light_list->push_back(temp_light);
		Sphere_list->push_back(temp_sphere);

		break;
	}

	glFlush();
}


// Draws the scene
void drawit(void)
{
	//cout << "drawing " << endl;
	glDrawPixels(ImageW, ImageH, GL_RGB, GL_FLOAT, framebuffer);
	glFlush();
}

// Clears framebuffer to black
void clearFramebuffer()
{
	int i, j;

	for (i = 0; i<ImageH; i++) {
		for (j = 0; j<ImageW; j++) {
			framebuffer[i][j][0] = 0.0;
			framebuffer[i][j][1] = 0.0;
			framebuffer[i][j][2] = 0.0;
		}
	}
}

// Sets pixel x,y to the color RGB
void setFramebuffer(int x, int y, float R, float G, float B)
{
	if (R <= 1.0)
		if (R >= 0.0)
			framebuffer[x][y][0] = R;
		else
			framebuffer[x][y][0] = 0.0;
	else
		framebuffer[x][y][0] = 1.0;
	if (G <= 1.0)
		if (G >= 0.0)
			framebuffer[x][y][1] = G;
		else
			framebuffer[x][y][1] = 0.0;
	else
		framebuffer[x][y][1] = 1.0;
	if (B <= 1.0)
		if (B >= 0.0)
			framebuffer[x][y][2] = B;
		else
			framebuffer[x][y][2] = 0.0;
	else
		framebuffer[x][y][2] = 1.0;
}


BOOL plane_intersect(Plane plane, Ray ray, GLfloatPoint3 *poutPoint)
{
	GLfloatPoint3 nhat = (plane.Normal);
	GLfloat denom = dotProduct(nhat, ray.dir_diff);

	//cout << denom<<endl;
	if (denom)
	{
		GLfloatPoint3 diff = sub(plane.Centre, ray.srcPoint);
		GLfloat paramt = dotProduct(diff, nhat) / denom;

		if (paramt > 1e-4)
		{
			*poutPoint = add(ray.srcPoint, sprod(ray.dir_diff, paramt));
			return TRUE;
		}

	}

	return FALSE;
}

BOOL Sphere_intersect(Sphere sphere, Ray ray, GLfloatPoint3 *poutPoint)
{

	/* formulae used */
	/*a = dx*dx + dy*dy + dz*dz;
	b = 2 * dx*(x0 - cx) + 2 * dy*(y0 - cy) + 2 * dz*(z0 - cz);
	c = cx*cx + cy*cy + cz*cz + x0*x0 + y0*y0 + z0*z0 +
	-2 * (cx*x0 + cy*y0 + cz*z0) - R*R;*/

	//GLfloat a = dotProduct(ray.dir_diff, ray.dir_diff);
	GLfloat a = 1;
	GLfloat b = 2 * dotProduct(ray.dir_diff, sub(ray.srcPoint, sphere.Centre));

	GLfloat c = dotProduct(sphere.Centre, sphere.Centre)
		+ dotProduct(ray.srcPoint, ray.srcPoint)
		- (2 * dotProduct(ray.srcPoint, sphere.Centre))
		- (sphere.radius * sphere.radius);

	GLfloat discr = (b * b - 4 * a * c);
	//cout << denom<<endl;
	if (discr > 1e-8)
	{
		GLfloat paramt = (-b - sqrt(discr)) * 0.5;
		if (paramt > 1e-4)
		{
			*poutPoint = add(ray.srcPoint, sprod(ray.dir_diff, paramt));
			return TRUE;
		}
	}

	return FALSE;
}



GLint winner_plane(vector<Plane> *Plane_list, GLfloatPoint3 *pPoint, int *pindex, Ray ray)
{
	int count_intersec = 0;
	{
		vector<Plane>::iterator it = Plane_list->begin();
		/* shooting planes */
		GLfloatPoint3 Point;
		for (it = Plane_list->begin(); it != Plane_list->end(); ++it)
		{
			int i = (it - Plane_list->begin());
			if (plane_intersect(*it, ray, &Point))
			{
				/*Colour out;
				out.r = (Point.x + 1200) / 2400;
				out.g = (Point.y + 1200) / 2400;
				out.b = (Point.z + 1200) / 2400;
				*/
				//if (((!i) && Point.x < -10) || i) /* special case for background */
				{
					/*
					{
					Best_point = Point;
					Front_colour = it->color;
					}*/
					GLfloatPoint3 diff1 = sub(*pPoint, ray.srcPoint);
					GLfloatPoint3 diff2 = sub(Point, ray.srcPoint);
					GLfloat current_d = dotProduct(diff1, diff1);
					GLfloat new_d = dotProduct(diff2, diff2);

					if ((new_d > 0) && (current_d > new_d))
					{
						count_intersec++;
						*pPoint = Point;
						*pindex = i;
						/*best_normal = it->Normal;
						p_color = &(it->color);
						p_properties = &(it->properties);*/

					}

				}
			}
		}
	}
	return count_intersec;
}


BOOL is_visible_p(vector<Plane> *Plane_list, GLfloatPoint3 BestPoint, Ray ray)
{

	vector<Plane>::iterator it = Plane_list->begin();
	/* shooting planes */
	GLfloatPoint3 Point;
	for (it = Plane_list->begin(); it != Plane_list->end(); ++it)
	{
		int i = (it - Plane_list->begin());
		if (plane_intersect(*it, ray, &Point))
		{
			/*Colour out;
			out.r = (Point.x + 1200) / 2400;
			out.g = (Point.y + 1200) / 2400;
			out.b = (Point.z + 1200) / 2400;
			*/
			//if (((!i) && Point.x < -10) || i) /* special case for background */
			{
				/*
				{
				Best_point = Point;
				Front_colour = it->color;
				}*/
				GLfloatPoint3 diff1 = sub(BestPoint, ray.srcPoint);
				GLfloatPoint3 diff2 = sub(Point, ray.srcPoint);
				GLfloat current_d = dotProduct(diff1, diff1);
				GLfloat new_d = dotProduct(diff2, diff2);

				//if ((new_d > 0) && (fabs(current_d - new_d) > 1))
				if ((new_d > 0) && ((current_d - new_d)>(1e-2 * new_d)))
				{
					if (((Point.x - BestPoint.x) < 2) && ((Point.y - BestPoint.y) < 2) && ((Point.z - BestPoint.z) < 2))
						return TRUE;
					else
						FALSE;
				}

			}
		}
	}/* for loop */

	return TRUE;
}

BOOL is_visible_s(vector<Sphere> *SphereList, GLfloatPoint3 BestPoint, Ray ray)
{

	vector<Sphere>::iterator it = SphereList->begin();
	/* shooting planes */
	GLfloatPoint3 Point;
	for (it = SphereList->begin(); it != SphereList->end(); ++it)
	{
		int i = (it - SphereList->begin());
		if (Sphere_intersect(*it, ray, &Point))
		{
			/*Colour out;
			out.r = (Point.x + 1200) / 2400;
			out.g = (Point.y + 1200) / 2400;
			out.b = (Point.z + 1200) / 2400;
			*/
			//if (((!i) && Point.x < -10) || i) /* special case for background */
			{
				/*
				{
				Best_point = Point;
				Front_colour = it->color;
				}*/
				GLfloatPoint3 diff1 = sub(BestPoint, ray.srcPoint);
				GLfloatPoint3 diff2 = sub(Point, ray.srcPoint);
				GLfloat current_d = dotProduct(diff1, diff1);
				GLfloat new_d = dotProduct(diff2, diff2);

				//if ((new_d > 0) && (fabs(current_d - new_d) > 1))
				if ((new_d > 0) && ((current_d - new_d)>(1e-2 * new_d)))
				{
					if (((Point.x - BestPoint.x) < 2) && ((Point.y - BestPoint.y) < 2) && ((Point.z - BestPoint.z) < 2))
						return TRUE;
					else
						FALSE;
				}

			}
		}
	} /* for loop */

	return TRUE;
}

GLint winner_sphere(vector<Sphere> *Sphere_list, GLfloatPoint3 *pPoint, int *pindex, Ray ray)
{
	int count_intersec = 0;
	{
		vector<Sphere>::iterator it = Sphere_list->begin();
		/* shooting planes */
		GLfloatPoint3 Point;
		for (it = Sphere_list->begin(); it != Sphere_list->end(); ++it)
		{
			int i = (it - Sphere_list->begin());
			if (Sphere_intersect(*it, ray, &Point))
			{
				/*Colour out;
				out.r = (Point.x + 1200) / 2400;
				out.g = (Point.y + 1200) / 2400;
				out.b = (Point.z + 1200) / 2400;
				*/
				{
					/*
					{
					Best_point = Point;
					Front_colour = it->color;
					}*/
					GLfloatPoint3 diff1 = sub(*pPoint, ray.srcPoint);
					GLfloatPoint3 diff2 = sub(Point, ray.srcPoint);
					GLfloat current_d = dotProduct(diff1, diff1);
					GLfloat new_d = dotProduct(diff2, diff2);

					if ((new_d > 0) && (current_d > new_d))
					{
						count_intersec++;
						*pPoint = Point;
						*pindex = i;
						/*best_normal = it->Normal;
						p_color = &(it->color);
						p_properties = &(it->properties);*/

					}

				}
			}
		}
	}
	return count_intersec;
}



Colour ShootRay(Ray ray, int count)
{

	Colour Front_colour;

	Front_colour.r = 0;
	Front_colour.g = 0;
	Front_colour.b = 0;

	if (count > TRACE_LIMIT)
		return Front_colour;

	GLfloatPoint3 Best_point;
	GLfloatPoint3 best_normal;
	surface_properties *p_properties = NULL;
	Colour *p_color = NULL;
	BOOL intersecting = FALSE;
	Best_point.x = INT_MIN;
	Best_point.y = INT_MIN;
	Best_point.z = INT_MIN;
	int count_intersec = 0;
	int ret;
	{
		{
			int index;
			ret = winner_plane(Plane_list, &Best_point, &index, ray);
			if (ret)
			{
				best_normal = (*Plane_list)[index].Normal;
				p_color = &(*Plane_list)[index].color;
				p_properties = &(*Plane_list)[index].properties;
			}
			count_intersec += ret;
		}

		{
			int index;
			ret = winner_sphere(Sphere_list, &Best_point, &index, ray);
			if (ret)
			{
				best_normal = Normalize(sub(Best_point, (*Sphere_list)[index].Centre));
				p_color = &((*Sphere_list)[index].color);
				p_properties = &((*Sphere_list)[index].properties);
			}
			count_intersec += ret;
		}
	}
	/* intersection completed */
	if ((count_intersec))	//Light sources illumination 
	{
		Colour current;
		current.r = 1;
		current.g = 1;
		current.b = 1;

		if (p_properties->isLight)
		{
			return current;
		}
		/* sphere intersections*/
		std::vector<Light>::iterator it = Light_list->begin();
		current.r = p_color->r * p_properties->coeffs.kAMB;
		current.g = p_color->g * p_properties->coeffs.kAMB;
		current.b = p_color->b * p_properties->coeffs.kAMB;

		GLfloatPoint3 eye_ray = (sub(ray.srcPoint, Best_point));
		eye_ray = Normalize(eye_ray);

		for (it = Light_list->begin(); it != Light_list->end(); ++it)
		{
			BOOL visibility = FALSE;

			GLfloatPoint3 inci_ray = sub(it->Centre, Best_point);
			inci_ray = Normalize(inci_ray);
			{
				Ray light_inci;
				light_inci.srcPoint = it->Centre;
				light_inci.dir_diff = Normalize(sub(Best_point, it->Centre));

				{
					BOOL planevis = is_visible_p(Plane_list, Best_point, light_inci);
					BOOL spherevis = is_visible_s(Sphere_list, Best_point, light_inci);
					visibility = (planevis && spherevis);
				}

			}

			GLfloat costheta = dotProduct(inci_ray, best_normal);
			if (visibility && (costheta > 0))
			{
				GLfloat costhalpha = dotProduct(best_normal, eye_ray);
				costheta = costheta * p_properties->coeffs.kDIFF;

				if (costhalpha > 0)
				{
					GLfloatPoint3 reflect_ray = sprod(best_normal, 2 * (dotProduct(best_normal, inci_ray)));
					reflect_ray = sub(reflect_ray, inci_ray);
					GLfloat cosaplha = dotProduct(reflect_ray, eye_ray);
					cosaplha = pow(cosaplha, p_properties->shininess);
					cosaplha = cosaplha * p_properties->coeffs.kSPEC;
					costheta += cosaplha;
				}
				current.r += it->color.r  *  costheta;
				current.g += it->color.g  *  costheta;
				current.b += it->color.b  *  costheta;
			}
		}
#if REFLECTION_ENABLE
		{
			Ray next_ray;
			GLfloatPoint3 inc_dir;
			inc_dir.x = -ray.dir_diff.x;
			inc_dir.y = -ray.dir_diff.y;
			inc_dir.z = -ray.dir_diff.z;

			GLfloatPoint3 reflect_ray = sprod(best_normal, 2 * (dotProduct(best_normal, inc_dir)));
			//	reflect_ray = Normalize(sub(reflect_ray, ray.dir_diff));
			reflect_ray = (sub(reflect_ray, inc_dir));
			next_ray.srcPoint = Best_point;
			next_ray.dir_diff = reflect_ray;

			Colour reflection = ShootRay(next_ray, ++count);
			//cout << current.r << ' ' << current.g << ' ' << current.b << ' ' << endl;
			current.r += p_properties->gamma_e *  reflection.r;
			current.g += p_properties->gamma_e *  reflection.g;
			current.b += p_properties->gamma_e *  reflection.b;

		}
#endif
		return current;
	}
	else
	{
		return Front_colour;
	}


}

void display(void)
{
	//The next two lines of code are for demonstration only.
	//They show how to draw a line from (0,0) to (100,100)
	int i, j;

	Ray ray;
	GLfloatPoint3 eyePoint;
	//eyePoint.z = (SceneT/2 + ImageW);
	//eyePoint.x = (ImageW>>5);
	//eyePoint.y = -ImageH/4;

	eyePoint.z = ((SceneT >> 1) + ImageW);
	eyePoint.x = 0;// (ImageW / 4);
	eyePoint.y = 0;// -ImageH / 4;

	//cout << "calculating " << endl;

	ray.srcPoint = eyePoint;

	if (isImageCalculated == FALSE)
	{
		for (j = 0; j < ImageH; j++)
		{
			for (i = 0; i < ImageW; i++)
			{
				//cout << i << " , "<< j << endl;
				ray.dir_diff.x = (i - (ImageW >> 1));
				ray.dir_diff.y = (j - (ImageH >> 1));
				ray.dir_diff.z = ((SceneT >> 1) + (ImageW >> 1));

				ray.dir_diff = Normalize(sub(ray.dir_diff, ray.srcPoint));

				Colour c = ShootRay(ray, 0);
				// cout <<i<<j<<"\t "<< endl;
				// cout << c.r << " " << c.g << " " <<c.b<< endl;
				setFramebuffer(j, i, c.r, c.g, c.b);
			}
		}
		isImageCalculated = TRUE;
	}
	/*for (j = 24; j < 230; j++)
	setFramebuffer(50, j, 0, 1, 4);*/

	drawit();
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case '1':   /*  s key rotates at shoulder_angle  */
		scene_num = 1;
		cout << "changing to scene 1, Please wait !!" << endl;
		clearFramebuffer();
		drawit();

		createScene(scene_num);
		isImageCalculated = FALSE;
		glutIdleFunc(display);

		createScene(1);
		break;
	case '0':
		scene_num = 0;
		cout << "changing to scene 0, Please wait !!" << endl;
		clearFramebuffer();
		drawit();


		createScene(scene_num);
		isImageCalculated = FALSE;
		glutIdleFunc(display);


		break;
	default:
		break;
	}
}

void init(void)
{
	clearFramebuffer();
	createScene(scene_num);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(ImageW, ImageH);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Tharun Battula - Homework 5");
	init();
	///*glutMouseFunc(mouseCallBack);
	glutKeyboardFunc(keyboard);
	//glutMotionFunc(motionCallBack);*/
	glutDisplayFunc(display);
	glutMainLoop();
	return 0;
}
