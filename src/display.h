#ifndef BEZIER_DISPLAY_H
#define BEZIER_DISPLAY_H

#include "bezier.h"

#include <ftxui/component/component.hpp>

class color_data;
class display;

class point_display;
class bezier_display;

ftxui::Component editModal(std::string*, int*, int*, display*&);

class color_data {
public:
	uint8_t red;
	uint8_t green;
	uint8_t blue;

	[[nodiscard]] ftxui::Color format() const {
		return ftxui::Color::RGB(red, green, blue);
	}

	[[nodiscard]] color_data dark() const {
		return {static_cast<uint8_t>(std::max(red - 50, 0)), static_cast<uint8_t>(std::max(green - 50, 0)), static_cast<uint8_t>(std::max(blue - 50, 0))};
	}
};

class display {
public:
	std::string name;
	color_data color;

	virtual ftxui::Component component() {
		return ftxui::Renderer([] { return ftxui::emptyElement(); });
	}
};

class point_display : public point, public display {
public:
	static int counter;

	point_display(double x, double y) : point(x, y) {
		name = std::string{(char) ('A' + (counter++ % 26))};
		color = {3, 102, 252};
	}
};

class bezier_display : public bezier, public display {
public:
	static int counter;

	explicit bezier_display(std::vector<point*> points) : bezier(points) {
		name = std::string{(char) ('a' + (counter++ % 26))};
		color = {105, 222, 33};
	}

	ftxui::Component component() override;
};

#endif //BEZIER_DISPLAY_H
