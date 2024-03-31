#ifndef BEZIER_COMPONENTS_H
#define BEZIER_COMPONENTS_H

#include <ftxui/component/component.hpp>

#include <functional>

#include "display.h"

ftxui::Component elementList(const std::vector<point*>&, const std::vector<bezier*>&, const int&, int&, const std::function<void(display*)>&);
ftxui::Component configBar(int& type, bool&, bool&, bool&, bool&, bool&, bool&, bool&);

ftxui::Component render(const std::vector<point*>&, const std::vector<bezier*>&, point&, point&, const int&, const int&, double&, bool&, bool&, bool&, bool&, bool&, bool&, bool&);

#endif //BEZIER_COMPONENTS_H
