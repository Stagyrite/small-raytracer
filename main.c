#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#define epsilon 0.000000001

typedef struct {
	double  x;
	double 	y;
	double 	z;
} Vector;
typedef Vector Point;
typedef Vector Color;

typedef struct {
	Point origin;
	Vector direction;
} Ray;

typedef struct {
	Vector normal;
	double distance;
	Color color;
} Plane;

typedef struct {
	Point point;
	double lambda;
	Plane* plane;
} Intersection;

Ray getRay(int x, int y) {
	Ray r;
	r.origin.x = 0;
	r.origin.y = 0;
	r.origin.z = 0;
	r.direction.x = -320 + x + 0.5;
	r.direction.y = 240 - y + 0.5;
	r.direction.z = -120;
	return r;
}

Plane* createBox() {
	Plane* box = (Plane*)malloc(5*sizeof(Plane));
	int size = 5;
	Plane p0 = {-1, 0, 0, size, 255, 0, 100};//links
	box[0] = p0;
	Plane p1 = {1, 0, 0, size, 0, 255, 0};//rechts
	box[1] = p1;
	Plane p2 = {0, 0, -1, size, 255,255,255};//hinten
	box[2] = p2;
	Plane p3 = {0, 1, 0, size, 255, 255, 255};//oben
	box[3] = p3;
	Plane p4 = {0, -1, 0, size, 255, 255, 255};//unten
	box[4] = p4;
	return box;
}	

Intersection rayHitsPlane(Ray* r, Plane* p)  {
	double gamma = r->direction.x * p->normal.x + r->direction.y * p->normal.y + r->direction.z * p->normal.z;
	double side = p->distance - (p->normal.x * r->origin.x + p->normal.y * r->origin.y + p->normal.z * r->origin.z);
	Intersection intersect  = {0, 0, 0, 0, p};
	if( gamma * side > epsilon) {
		double lambda = side / gamma;
		intersect.point.x = r->origin.x + r->direction.x * lambda;
		intersect.point.y = r->origin.y + r->direction.y * lambda;
		intersect.point.z = r->origin.z + r->direction.z * lambda;
		intersect.lambda = lambda;
	} else {
	}

	return intersect;
}

double vectorLength(Vector v) {
	return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

Vector pointDifference(Point p1, Point p2) {
	Vector v = {p1.x - p2.x, p1.y - p2.y, p1.z - p2.z};
	return v; 
}

Intersection getFirstIntersection(Ray* ray, Plane* box) {
	Intersection nearestIntersect; double minDouble = DBL_MAX;
	int planeCounter; 
	for(planeCounter = 0; planeCounter < 5; planeCounter++) {
		Intersection intersect = rayHitsPlane(ray, &box[planeCounter]);
		if(intersect.lambda > epsilon && intersect.lambda < minDouble) {
			nearestIntersect = intersect;
			minDouble = intersect.lambda;
		}
	}
	return nearestIntersect;
}

Color traceRay(Ray* ray, Plane* box) {
	Color color = {0, 0, 0};
	Point light = {-3, -2, -3};
	Intersection intersect = getFirstIntersection(ray, box);
	if(intersect.lambda > epsilon) {
		Vector directionToLight = pointDifference(light, intersect.point); 
		Ray rayToLight = {intersect.point, directionToLight};
		Intersection tempIntersect = getFirstIntersection(&rayToLight, box);
		if(tempIntersect.lambda >= 1 || tempIntersect.lambda == 0) {
			double length = vectorLength(directionToLight);
			double scale = 11 / (length * length);
			color = intersect.plane->color;
			color.x *= scale;
			color.y *= scale;
			color.z *= scale;
		}
	}
	return color;
}

extern int write_bmp(const char* filename, int width, int height, char* rgb);

int main(int argc, char* argv[]) {
	Plane* box = createBox();
	char* rgb = (char*)malloc(3*640*480*sizeof(char));	
	int x,y;
	for(x=0; x<640; x++) {
		for(y=0; y<480; y++) {
			Ray r = getRay(x, y);
			Color c = traceRay(&r, box);
			
			int ipos = 3*(640*y+x);
			double red = c.z;
			double green  = c.y;
			double blue = c.x;
			rgb[ipos + 2] = (unsigned char)(red > 255 ? 255 : red); 
			rgb[ipos + 1] = (unsigned char)(green > 255 ? 255 : green);
			rgb[ipos] = (unsigned char)(blue > 255 ? 255 : blue);
		}
	}
	write_bmp("test.bmp", 640, 480, rgb);
	free(box);free(rgb);
	return 0;
}