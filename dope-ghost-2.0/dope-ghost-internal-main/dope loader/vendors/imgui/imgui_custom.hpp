#pragma once

#include "imgui.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

#include <vector>
#include <string>

namespace ImGui {

	bool HorizontalColorPicker(ImColor* col, ImVec2 size, bool border = false);

	bool CustomBeginCombo(const char* label, const char* preview_value, ImGuiComboFlags flags);

	bool CustomCombo1(const char* label, std::vector<std::string>& items, int items_size, int* selectedItem, int height_in_items = -1);

	void CustomColorPicker(const char* name, float color[3], ImVec2 size) noexcept;

	void SpinnerLemniscate(const char* label, float radius, float thickness, const ImColor& color = 0xffffffff, float speed = 2.8f, float angle = IM_PI / 2.0f);

	void SpinnerCircleDrop(const char* label, float radius, float thickness, float thickness_drop, const ImColor& color = 0xffffffff, const ImColor& bg = 0xffffff80, float speed = 2.8f, float angle = IM_PI);

	void SpinnerAngEclipse(const char* label, float radius, float thickness, const ImColor& color = 0xffffffff, float speed = 2.8f, float angle = IM_PI);

	bool Hotkey(const char* label, int* k, const ImVec2& size_arg);
}