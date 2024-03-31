#include "scroller.h"

namespace ftxui {

class ScrollerBase : public ComponentBase {
public:
	ScrollerBase(const Component& child, int& position) : selected_(position) {
		Add(child);
	}

private:
	Element Render() final {
		auto focused = Focused() ? focus : ftxui::select;

		Element background = ComponentBase::Render();
		background->ComputeRequirement();
		size_ = background->requirement().min_y;

		return dbox({
				std::move(background),
				vbox({
						text(L"") | size(HEIGHT, EQUAL, selected_),
						text(L"") | focused,
				}),
		}) | vscroll_indicator | yframe | yflex | reflect(box_);
	}

	bool OnEvent(Event event) final {
		if (event.is_mouse() && box_.Contain(event.mouse().x, event.mouse().y)) TakeFocus();

		const int selected_old = selected_;
		if (event == Event::ArrowUp || event == Event::Character('k') || (event.is_mouse() && event.mouse().button == Mouse::WheelUp)) selected_--;
		if ((event == Event::ArrowDown || event == Event::Character('j') || (event.is_mouse() && event.mouse().button == Mouse::WheelDown))) selected_++;

		if (event == Event::PageDown) selected_ += box_.y_max - box_.y_min;
		if (event == Event::PageUp) selected_ -= box_.y_max - box_.y_min;
		if (event == Event::Home) selected_ = 0;
		if (event == Event::End) selected_ = size_;

		selected_ = std::clamp(selected_, 0, size_ - 1);

		return selected_old != selected_;
	}

	[[nodiscard]] bool Focusable() const final {
		return true;
	}

	int& selected_;
	int size_ = 0;
	Box box_;
};

Component Scroller(const Component& child, int& position) {
	return Make<ScrollerBase>(child, position);
}

}  // namespace ftxui

// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.