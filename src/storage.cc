#include "storage.h"

#include <nlohmann/json.hpp>
#include <fstream>

#include "display.h"

grid_data::grid_data(const std::string& file) : file(file) {
	auto stream = std::ifstream(file);
	if(stream.fail()) return;

	auto json = nlohmann::json::parse(stream);

	std::vector<point_wrapper> pointValues;
	json["points"].get_to(pointValues);

	for(auto p : pointValues) points.push_back(p.value);

	for(auto c : json["curves"]) {
		std::vector<point*> p;
		for(int i : c["points"]) p.push_back(points[i]);

		if(c.contains("name")) {
			auto temp = new bezier_display(p);
			temp->name = c["name"];
			temp->color = c["color"];
			curves.push_back(temp);
		} else curves.push_back(new bezier(p));
	}

	point_display::counter = json["point_counter"];
	bezier_display::counter = json["curve_counter"];

	stream.close();
}

void grid_data::save() {
	if(file.empty()) return;

	nlohmann::json json;

	std::vector<curve_wrapper> curveValues;
	curveValues.reserve(points.size());
	for(auto c : curves) curveValues.push_back({c, points});

	std::vector<point_wrapper> pointValues;
	pointValues.reserve(points.size());
	for(auto p : points) pointValues.push_back({p});

	json["points"] = pointValues;
	json["curves"] = curveValues;

	json["point_counter"] = point_display::counter;
	json["curve_counter"] = bezier_display::counter;

	auto stream = std::ofstream(file);
	stream << json.dump(2);
	stream.close();
}

void grid_data::deleteCurve(bezier* curve) {
	curves.erase(std::remove(curves.begin(), curves.end(), curve), curves.end());
	delete curve;
}

void grid_data::deletePoint(point* point) {
	for(auto c : curves) {
		c->points.erase(std::remove(c->points.begin(), c->points.end(), point), c->points.end());
		if(c->points.size() <= 1) deleteCurve(c);
	}

	points.erase(std::remove(points.begin(), points.end(), point), points.end());
	delete point;
}


void to_json(nlohmann::json& j, const color_data& c) {
	j = nlohmann::json {
			{"red", c.red},
			{"green", c.green},
			{"blue", c.blue}
	};
}

void from_json(const nlohmann::json& j, color_data& c) {
	j["red"].get_to(c.red);
	j["green"].get_to(c.green);
	j["blue"].get_to(c.blue);
}

void to_json(nlohmann::json& j, const point_wrapper& p) {
	j = nlohmann::json {
			{"x", p.value->x},
			{"y", p.value->y}
	};

	if(auto d = dynamic_cast<point_display*>(p.value)) {
		j["name"] = d->name;
		j["color"] = d->color;
	}
}

void from_json(const nlohmann::json& j, point_wrapper& p) {
	if(j.contains("name")) {
		auto temp = new point_display(j["x"], j["y"]);

		temp->name = j["name"];
		temp->color = j["color"];

		p = {temp};
	} else p = {new point(j["x"], j["y"])};
}

void to_json(nlohmann::json& j, const curve_wrapper& c) {
	std::vector<int> points;
	for(auto p : c.value->points) {
		auto index = (int) std::distance(c.scope.begin(), std::find(c.scope.begin(), c.scope.end(), p));
		if(index >= c.scope.size()) c.scope.push_back(p);
		points.push_back(index);
	}

	j["points"] = points;

	if(auto d = dynamic_cast<bezier_display*>(c.value)) {
		j["name"] = d->name;
		j["color"] = d->color;
	}
}