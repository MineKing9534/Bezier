#include "components.h"

#include "scroller.h"
#include "display.h"

#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/canvas.hpp>

#include <vector>
#include <cmath>

#include <sstream>
#include <iomanip>

using namespace ftxui;

const Color VIRTUAL_COLOR = Color::GrayLight;

int pointPosition = 0;
int curvePosition = 0;

std::string format(double value, int places) {
	std::ostringstream str;
	str << std::setprecision(places) << value;
	return str.str();
}

Component elementList(const std::vector<point*>& points, const std::vector<bezier*>& curves, const int& horizontalSplit, int& verticalSplit, const std::function<void(display*)>& edit) {
	auto pointList = Scroller(Renderer([&] {
		std::vector<Element> names;

		for (const auto p: points) {
			auto d = dynamic_cast<point_display*>(p);

			names.push_back(hbox({
					text("⬤") | color(d ? d->color.format() : VIRTUAL_COLOR),
					text("  | " + (d ? d->name : "Virtual Point")) | color(Color::GrayLight),
					text(" = (" + format(p->x, 2) + ", " + format(p->y, 2) + ")") |
					color(Color::GrayDark),
			}));
		}

		return hbox({
				text(" ") | bold,
				vbox(names),
				filler()
		});
	}), pointPosition);

	auto curveList = Scroller(Renderer([&] {
		std::vector<Element> names;

		for (const auto& c: curves) {
			std::string value;

			for (auto p: c->points) {
				if (auto d = dynamic_cast<point_display*>(p)) value += ", " + d->name;
				else value += ", ?";
			}

			if (value.length() > 2) value = value.substr(2);

			auto d = dynamic_cast<bezier_display*>(c);
			names.push_back(hbox({
					text("⬤") | color((d ? d->color.format() : VIRTUAL_COLOR)),
					text("  | " + (d ? d->name : "Virtual Curve")) | color(Color::GrayLight),
					text(" = {" + value + "}") | color(Color::GrayDark),
			}));
		}

		return hbox({
				text(" ") | bold,
				vbox(names),
				filler()
		});
	}), curvePosition);

	return ResizableSplit({
			Renderer(pointList, [pointList] {
				return vbox({
						text(" Points") | bold,
						pointList->Render()
				});
			}),
			Renderer(curveList, [curveList] {
				return vbox({
						text(" Curves") | bold,
						curveList->Render()
				});
			}),
			Direction::Up,
			&verticalSplit,
			[] { return separatorDouble() | color(Color::White); }
	}) | clear_under | CatchEvent([&horizontalSplit, &verticalSplit, edit, &points, &curves](Event event) {
		if (!event.is_mouse()) return false;
		if (event.mouse().x > horizontalSplit) return false;
		if (!(event.mouse().button == Mouse::Left && event.mouse().motion == ftxui::Mouse::Released)) return false;

		int index = event.mouse().y - 2;

		if (index < 0) return false;

		if (event.mouse().y > verticalSplit) {
			index -= verticalSplit + 1;
			index += curvePosition;

			if (index < curves.size()) {
				auto d = dynamic_cast<bezier_display*>(curves[index]);
				if (d) edit(d);
			}
		} else {
			index += pointPosition;

			std::cout << pointPosition << ", " << index << "\n";
			if (index < points.size()) {
				auto d = dynamic_cast<point_display*>(points[index]);
				if (d) edit(d);
			}
		}

		return false;
	});
}

Component configBar(int& type, bool& showAxis, bool& showGrid, bool& showPoints, bool& showPolygons, bool& showCurves, bool& showNames, bool& showVirtual) {
	auto axis = Checkbox("Axis", &showAxis);
	auto grid = Checkbox("Grid", &showGrid);
	auto points = Checkbox("Points", &showPoints);
	auto polygons = Checkbox("Polygons", &showPolygons);
	auto curves = Checkbox("Curves", &showCurves);
	auto names = Checkbox("Names", &showNames);
	auto virt = Checkbox("Virtual", &showVirtual);

	auto options = new std::vector<std::string>({
			"Point",
			"Curve"
	});
	auto typeSelector = Radiobox(options, &type);

	auto pointReset = Button("", [] { point_display::counter = 0; }, {
			.transform = [](auto state) {
				return state.hovered
				       ? text("[ Reset ]") | bold
				       : text("Point: " + std::string{(char) ('A' + (point_display::counter % 26))});
			}
	});

	auto curveReset = Button("", [] { bezier_display::counter = 0; }, {
			.transform = [](auto state) {
				return state.hovered
						? text("[ Reset ]") | bold
						: text("Curve: " + std::string{(char) ('a' + (bezier_display::counter % 26))});
			}
	});

	return Renderer(Container::Vertical({
			axis,
			grid,
			points,
			polygons,
			curves,
			names,
			virt,
			typeSelector,
			pointReset,
			curveReset
	}), [axis, grid, points, polygons, curves, names, virt, typeSelector, pointReset, curveReset] {
		return vbox({
				hbox({
						text(" "),
						vbox({
								axis->Render(),
								grid->Render(),
								points->Render(),
								polygons->Render(),
								curves->Render(),
								names->Render(),
								virt->Render(),
								filler()
						})
				}),
				separator(),
				hbox({
						text(" "),
						vbox({
								typeSelector->Render(),
								filler()
						})
				}),
				separator(),
				hbox({
						text(" "),
						hbox({
								pointReset->Render(),
								separatorEmpty(),
								separator(),
								separatorEmpty(),
								curveReset->Render()
						})
				}),
				separator()
		}) | clear_under | color(Color::White);
	});
}

const int steps = 50;

Component render(const std::vector<point*>& points, const std::vector<bezier*>& curves, point& position, point& delta_mouse, const int& splitLeft, const int& splitRight, double& zoom, bool& showAxis, bool& showGrid, bool& showPoints,
                 bool& showPolygons, bool& showCurves, bool& showNames, bool& showVirtual) {
	return Renderer([&curves, &points, &position, &zoom, &splitRight, &showAxis, &showGrid, &showPoints, &showPolygons, &showCurves, &showNames, &showVirtual] {
		auto dim = Terminal::Size();

		Canvas graph(1000, 1000);
		Canvas lines(1000, 1000);

		if (showGrid) {
			for (int i = (int) (position.x - 1) * 2 % (int) lround(zoom * 20); i < lines.width(); i += (int) lround(zoom * 20)) lines.DrawPointLine(i, 0, i, lines.height(), Color::RGB(50, 50, 50));
			for (int i = (int) (dim.dimy - position.y) * 4 % (int) lround(zoom * 20); i < lines.height(); i += (int) lround(zoom * 20)) lines.DrawPointLine(0, i, lines.width(), i, Color::RGB(50, 50, 50));
		}

		for (auto c: curves) {
			auto d = dynamic_cast<bezier_display*>(c);
			if (!showVirtual && !d) continue;

			if (showPolygons) {
				for (int i = 1; i < c->points.size(); i++) {
					auto p1 = *c->points[i - 1];
					auto p2 = *c->points[i];

					graph.DrawPointLine(
							(int) lround((zoom * p1.x * 2 + position.x - 1) * 2),
							(int) lround((dim.dimy - (position.y + zoom * p1.y + 0.5)) * 4),
							(int) lround((zoom * p2.x * 2 + position.x - 1) * 2),
							(int) lround((dim.dimy - (position.y + zoom * p2.y + 0.5)) * 4),
							[d](Pixel& p) {
								if (d) p.foreground_color = d->color.dark().format();
								else p.foreground_color = Color::GrayDark;
							}
					);
				}
			}

			std::vector<point> temp;
			for (double t = 0; t <= 1; t += 1.0 / (steps * zoom)) temp.push_back(c->interpolate(t));
			temp.push_back(*c->points[c->points.size() - 1]);

			if (showCurves) {
				for (int i = 1; i < temp.size(); i++) {
					auto p1 = temp[i - 1];
					auto p2 = temp[i];

					graph.DrawPointLine(
							(int) lround((zoom * p1.x * 2 + position.x - 1.5) * 2),
							(int) lround((dim.dimy - (position.y + zoom * p1.y + 0.5)) * 4),
							(int) lround((zoom * p2.x * 2 + position.x - 1.5) * 2),
							(int) lround((dim.dimy - (position.y + zoom * p2.y + 0.5)) * 4),
							d ? d->color.format() : VIRTUAL_COLOR
					);
				}
			}

			if (showNames && d) graph.DrawText((int) (zoom * temp[(int) (zoom * steps / 5)].x * 2 + position.x) * 2 + 2, (int) (dim.dimy - zoom * temp[(int) (zoom * steps / 5)].y - position.y) * 4 + 4, d->name, d->color.format());
		}

		if (showPoints) {
			for (auto p: points) {
				auto d = dynamic_cast<point_display*>(p);
				if (!showVirtual && !d) continue;

				auto x = (int) lround((position.x + zoom * 2 * p->x - 1.5) * 2);
				auto y = (int) lround((dim.dimy - (position.y + zoom * p->y + 0.5)) * 4);

				graph.DrawText(x, y, "⊗", d ? d->color.format() : VIRTUAL_COLOR);
				if (showNames && d) graph.DrawText(x + 2, y - 4, d->name, d->color.format());
			}
		}

		return dbox({
				canvas(lines),
				showAxis ? dbox({
						hbox({emptyElement() | ftxui::size(WIDTH, EQUAL, (int) position.x - 1), separator() | color(Color::GrayDark)}),
						vbox({emptyElement() | ftxui::size(HEIGHT, EQUAL, (int) (dim.dimy - position.y)), separator() | color(Color::GrayDark)})
				}) : emptyElement(),
				canvas(std::move(graph)),
				hbox({
						filler(),
						vbox({
								text("Zoom: " + format(zoom, 2)) | color(Color::White),
								filler()
						}),
						emptyElement() | size(WIDTH, EQUAL, splitRight + 2)
				})
		});
	}) | CatchEvent([&position, &delta_mouse, &splitLeft, &splitRight, &zoom](Event event) {
		if (!event.is_mouse()) return false;
		auto mouse = event.mouse();

		if (mouse.x <= splitLeft || mouse.x >= Terminal::Size().dimx - splitRight) return false;

		if (mouse.button == Mouse::Right && mouse.motion == Mouse::Moved) {
			position += delta_mouse;
			position.x = std::max(position.x, -1.0);
		} else if(mouse.button == Mouse::WheelUp || mouse.button == Mouse::WheelDown) {
			auto temp = zoom;

			if (mouse.button == Mouse::WheelUp) temp += 0.05;
			else if (mouse.button == Mouse::WheelDown) temp -= 0.05;

			temp = std::clamp(temp, 0.5, 2.0);
			position += {(zoom - temp) * (mouse.x - position.x) / (zoom), (zoom - temp) * ((Terminal::Size().dimy - (mouse.y + position.y)) / zoom + 1)};
			zoom = temp;
		}

		return false;
	});
}
