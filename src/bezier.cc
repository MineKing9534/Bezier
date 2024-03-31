#include "bezier.h"

#include <cmath>
#include <vector>
#include <iostream>

point point::operator+(const point& point) const {
	return {x + point.x, y + point.y};
}

point point::operator-(const point& point) const {
	return {x - point.x, y - point.y};
}

point point::operator*(double value) const {
	return {x * value, y * value};
}

point point::operator/(double value) const {
	return {x / value, y / value};
}

point point::operator+=(const point& point) {
	x += point.x;
	y += point.y;

	return *this;
}

point point::operator-=(const point& point) {
	x -= point.x;
	y -= point.y;

	return *this;
}

point point::operator*=(double value) {
	x *= value;
	y *= value;

	return *this;
}

point point::operator/=(double value) {
	x /= value;
	y /= value;

	return *this;
}

std::ostream& operator<<(std::ostream& os, const point& p) {
	return os << "(" << p.x << " | " << p.y << ")";
}

double nCr(double n, double k) {
	if (2 * k > n) k = n - k;
	double result = 1;
	for (int i = 1; i <= k; i++) result *= (n + 1 - i) / i;
	return result;
}

point bezier::interpolate(double t) {
	point result = {0, 0};
	for (int i = 0; i < points.size(); i++) {
		result += *points[i] * nCr((int) points.size() - 1, i) * pow(1.0 - t, (double) points.size() - i - 1) * pow((double) t, (double) i);
	}
	return result;
}

std::vector<point*> bezier::elevateDegree() const {
	std::vector<point*> result(points.size() + 1);

	result[0] = new point(points[0]->x, points[0]->y);

	for (int i = 1; i < points.size(); i++) {
		auto p = (*points[i - 1] * ((double) i / (int) points.size())) + (*points[i] * (1 - (double) i / (int) points.size()));
		result[i] = new point(p.x, p.y);
	}

	result[points.size()] = new point(points[points.size() - 1]->x, points[points.size() - 1]->y);

	return result;
}

std::vector<point*> bezier::elevateDegree(unsigned short n) const {
	std::vector<point*> result = points;
	for(int i = 0; i < n; i++) result = bezier(result).elevateDegree();
	return result;
}