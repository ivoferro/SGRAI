#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <iostream>

#define DEGREES_TO_RADIANS(d) (M_PI * (d) / 180.0)

#define SIZE_2D_COORDINATE 2
#define X_COORDINATE 0
#define Y_COORDINATE 1

///
/// Calculates the middle point between p1 and p2.
///
float* middle_point(float *p1, float *p2);

///
/// Calculates the distance between p1 and p2.
///
float distance(float *p1, float *p2);

///
/// Returns a table with the distance between equal spaced points
/// in a circumference of a given radius.
///
float** spaced_points(float radius, float *middle_point, size_t number_of_points);

int main() {

	float point_0[SIZE_2D_COORDINATE], point_1[SIZE_2D_COORDINATE];
	printf("Insert the first coordinate in the format DD.D;DD.D (e.g. 22.3;14.2):\n");
	fscanf_s(stdin, "%f;%f", &point_0[X_COORDINATE], &point_0[Y_COORDINATE]);
	printf("Insert the second coordinate in the format DD.D;DD.D (e.g. 22.3;14.2):\n");
	fscanf_s(stdin, "%f;%f", &point_1[X_COORDINATE], &point_1[Y_COORDINATE]);

	float *center = middle_point(point_0, point_1);
	float diameter = distance(point_0, point_1);

	printf("\nThe center point is P(%.2f;%.2f) and the diameter is %.2f\n",
		center[X_COORDINATE], center[Y_COORDINATE], diameter);

	float radius = diameter / 2.0;
	size_t number_of_points = 8;
	float** points = spaced_points(radius, center, number_of_points);

	printf("\n| X | Y |\n");
	for (size_t i = 0; i < number_of_points; i++)
	{
		printf("| %.2f | %.2f |\n", points[i][X_COORDINATE], points[i][Y_COORDINATE]);
	}
	printf("\n");

	for (size_t i = 0; i < number_of_points; i++)
	{
		delete[] points[i];
	}
	delete[] points;
	delete[] center;

	system("pause");
	return 0;
}

///
/// Calculates the middle point between p1 and p2.
///
float* middle_point(float *p1, float *p2) {

	float *middle_pt = new float[SIZE_2D_COORDINATE];

	middle_pt[X_COORDINATE] = (p1[X_COORDINATE] + p2[X_COORDINATE]) / 2.0;
	middle_pt[Y_COORDINATE] = (p1[Y_COORDINATE] + p2[Y_COORDINATE]) / 2.0;

	return middle_pt;
}

///
/// Calculates the distance between p1 and p2.
///
float distance(float *p1, float *p2) {

	float x_pow = pow((p2[X_COORDINATE] - p1[X_COORDINATE]), 2);
	float y_pow = pow((p2[Y_COORDINATE] - p1[Y_COORDINATE]), 2);

	return sqrt((x_pow + y_pow));
}

///
/// Returns a table with the distance between equal spaced points
/// in a circumference of a given radius.
///
float** spaced_points(float radius, float *middle_point, size_t number_of_points) {

	float **table = new float*[number_of_points];
	for (size_t i = 0; i < number_of_points; i++)
	{
		table[i] = new float[SIZE_2D_COORDINATE];
	}

	float rotation = 0;
	float gap = DEGREES_TO_RADIANS(360 / number_of_points);

	for (size_t i = 0; i < number_of_points; i++)
	{
		table[i][X_COORDINATE] = radius * cos(rotation) + middle_point[X_COORDINATE];
		table[i][Y_COORDINATE] = radius * sin(rotation) + middle_point[Y_COORDINATE];
		rotation += gap;
	}

	return table;
}