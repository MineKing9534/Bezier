#include "display.h"

using namespace ftxui;

Component editModal(std::string* name, int* cursor, int* colorValue, display*& object) {
	InputOption input;
	input.on_change = [name, &object] {
		if(!name->empty()) object->name = *name;
	};
	input.cursor_position = cursor;
	input.multiline = false;

	auto nameInput = Input(name, input);

	auto colorInput = Slider("", colorValue, 16, 255, 1);

	return Renderer(Container::Vertical({
		nameInput,
		colorInput
	}), [nameInput, colorInput, colorValue] {
		return vbox({
				hbox({
						text("Name:  "),
						nameInput->Render()
				}),
				separatorEmpty(),
				hbox({
						text("Color: "),
						colorInput->Render() | color(static_cast<Color::Palette256>(*colorValue))
				})
		}) | borderEmpty | size(WIDTH, GREATER_THAN, 40) | size(HEIGHT, GREATER_THAN, 10) | borderRounded | borderEmpty;
	});
}

int point_display::counter = 0;
int bezier_display::counter = 0;

Component bezier_display::component() {
	return ftxui::Button("Elevate Degree", [] {

	});
}