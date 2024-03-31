#ifndef SCROLLER_H
#define SCROLLER_H

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>

namespace ftxui {
	Component Scroller(const Component& child, int& position);
}

#endif /* end of include guard: SCROLLER_H */

// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.