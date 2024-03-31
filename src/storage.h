#ifndef BEZIER_STORAGE_H
#define BEZIER_STORAGE_H

#include <nlohmann/json.hpp>
#include <vector>

#include "display.h"

class grid_data;

struct point_wrapper;
struct curve_wrapper;

class grid_data {
public:
	grid_data() = default;
	explicit grid_data(const std::string&);

	std::string file;

	std::vector<point*> points;
	std::vector<bezier*> curves;

	void deleteCurve(bezier*);
	void deletePoint(point*);

	void save();
};

struct point_wrapper {
	point* value;
};

struct curve_wrapper {
	bezier* value;
	std::vector<point*>& scope;
};

void to_json(nlohmann::json& j, const color_data& c);
void from_json(const nlohmann::json& j, color_data& c);

void to_json(nlohmann::json& j, const point_wrapper& p);
void from_json(const nlohmann::json& j, point_wrapper& p);

void to_json(nlohmann::json& j, const curve_wrapper& c);

#endif //BEZIER_STORAGE_H
