#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>

#include "components.h"
#include "display.h"
#include "storage.h"

using namespace ftxui;

void elevateDegree(bezier*, unsigned short);

auto screen = ScreenInteractive::Fullscreen();

point last_mouse;
point delta_mouse;

grid_data grid;

point position;
double zoom = 1;

int type = 0;
point* movingPoint = nullptr;

bezier* currentCurve;

int main(int argc, char** argv) {
	grid = grid_data(argc == 2 ? argv[1] : "save");

	auto size = Terminal::Size();
	position = {size.dimx / 2.0, size.dimy / 2.0 + 2};

	auto showAxis = true;
	auto showGrid = true;
	auto showPoints = true;
	auto showPolygons = false;
	auto showCurves = true;
	auto showNames = true;
	auto showVirtual = true;

	auto splitLeft = size.dimx / 6;
	auto splitRight = 23;
	auto splitVertical = size.dimy / 2 - 2;

	auto list = elementList(grid.points, grid.curves, splitLeft, splitVertical, [](display*) {});
	auto config = configBar(type, showAxis, showGrid, showPoints, showPolygons, showCurves, showNames, showVirtual);

	auto graph = render(grid.points, grid.curves, position, delta_mouse, splitLeft, splitRight, zoom, showAxis, showGrid, showPoints, showPolygons, showCurves, showNames, showVirtual);

	list = ResizableSplit({
			list,
			Renderer([] { return filler(); }),
			Direction::Left,
			&splitLeft,
			[] { return separatorDouble() | color(Color::White); }
	});

	config = ResizableSplit({
			config,
	        Renderer([] { return filler(); }),
			Direction::Right,
			&splitRight,
			[] { return separatorDouble() | color(Color::White); }
	});

	auto container = Renderer(Container::Horizontal({list, graph, config}), [&] {
		return dbox({
				graph->Render(),
				list->Render(),
				config->Render()
		});
	});

	container |= CatchEvent([&](Event event) {
		if(!event.is_mouse()) return false;

		auto size = Terminal::Size();
		auto mouse = event.mouse();

		{
			point temp = {(double) mouse.x, (double) 1 + size.dimy - mouse.y};
			delta_mouse = temp - last_mouse;
			last_mouse = temp;
		}

		if(!(mouse.x > splitLeft + 2 && mouse.x < size.dimx - splitRight - 2 && showPoints)) return false;

		auto x = (int) lround((mouse.x - position.x) / (2.0 * zoom));
		auto y = (int) lround((size.dimy - (mouse.y + position.y)) / zoom);

		int i = -1;
		for(int j = (int) grid.points.size() - 1; j >= 0; j--) {
			auto p = grid.points[j];
			if(p->x >= x - 1 / zoom && p->x <= x + 1 / zoom && p->y >= y - 1 / zoom && p->y <= y + 1 / zoom) {
				i = j;
				break;
			}
		}

		if(type == 0) {
			if (mouse.motion == Mouse::Released) {
				if (movingPoint == nullptr) {
					if (mouse.button == Mouse::Left && i == -1) grid.points.push_back(new point_display(x, y));
					else if (mouse.button == Mouse::Right && i != -1) grid.deletePoint(grid.points[i]);
				} else movingPoint = nullptr;
			} else {
				if (movingPoint != nullptr) {
					movingPoint->x = x;
					movingPoint->y = y;
				} else if (mouse.button == Mouse::Left && i != -1) movingPoint = grid.points[i];
			}
		} else if(type == 1) {
			if(mouse.motion == Mouse::Released && mouse.button == Mouse::Left) {
				if(i != -1) {
					if(currentCurve == nullptr) {
						std::vector temp = {grid.points[i]};
						currentCurve = new bezier(temp);

						grid.curves.push_back(currentCurve);
					} else currentCurve->points[currentCurve->points.size() - 1] = grid.points[i];

					currentCurve->points.push_back(new point(x, y));
				} else if(currentCurve != nullptr && !currentCurve->points.empty()) {
					currentCurve->points.pop_back();

					auto curve = new bezier_display(currentCurve->points);
					grid.curves.push_back(curve);

					grid.deleteCurve(currentCurve);

					currentCurve = nullptr;
				}
			} else if(mouse.motion == Mouse::Moved && currentCurve != nullptr) {
				auto temp = currentCurve->points[currentCurve->points.size() - 1];
				temp->x = x;
				temp->y = y;
			}
		}

		return false;
	});

	container |= CatchEvent([](const Event& event) {
		if (event == Event::Character('q') || event == Event::Escape) {
			if(currentCurve != nullptr) grid.deleteCurve(currentCurve);

			grid.save();
			screen.Exit();
		}
		return false;
	});

	//container |= Modal();

	screen.Loop(Renderer(container, [&] {
		return container->Render() | border | color(Color::White);
	}));

	return 0;
}

void elevateDegree(bezier* curve, unsigned short n) {
	auto temp = curve->elevateDegree(n);
	std::vector<point*> result;

	for(auto p : temp) {
		auto display = new point_display((int) (p->x * 10) / 10.0, (int) (p->y * 10) / 10.0);

		grid.points.push_back(display);
		result.push_back(display);
	}

	grid.curves.push_back(new bezier_display(result));
}
