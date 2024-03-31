#ifndef BEZIER_BEZIER_H
#define BEZIER_BEZIER_H

#include <vector>
#include <string>
#include <functional>
#include <iostream>


class point;
class bezier;

class point {
public:
	point(double x, double y) : x(x), y(y) {
	}

	point() : point(0, 0) {}

	double x;
	double y;

	point operator+(const point&) const;

	point operator-(const point&) const;

	point operator*(double) const;

	point operator/(double) const;

	point operator+=(const point&);

	point operator-=(const point&);

	point operator*=(double);

	point operator/=(double);

	virtual ~point() = default;
};

std::ostream& operator<<(std::ostream& os, const point& p);

class bezier {
public:
	explicit bezier(std::vector<point*>& points) : points(points) {}

	std::vector<point*> points;

	point interpolate(double);

	[[nodiscard]] std::vector<point*> elevateDegree() const;
	[[nodiscard]] std::vector<point*> elevateDegree(unsigned short) const;

	virtual ~bezier() = default;
};

#endif //BEZIER_BEZIER_H
