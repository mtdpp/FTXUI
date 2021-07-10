#include <functional>  // for function
#include <memory>      // for shared_ptr

#include "ftxui/component/captured_mouse.hpp"  // for CapturedMouse
#include "ftxui/component/event.hpp"  // for Event, Event::Return
#include "ftxui/component/mouse.hpp"  // for Mouse, Mouse::Left, Mouse::Pressed
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive

#include <functional>  // for function
#include <string>      // for allocator, wstring

#include "ftxui/component/component.hpp"       // for Component
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/dom/elements.hpp"   // for Element, Decorator, inverted, nothing
#include "ftxui/screen/box.hpp"     // for Box
#include "ftxui/screen/string.hpp"  // for ConstStringRef

namespace ftxui {

namespace {
/// @brief A Checkbox. It can be checked or unchecked.Display an element on a
/// ftxui::Screen.
/// @ingroup dom
class CheckboxBase : public ComponentBase {
 public:
  CheckboxBase(ConstStringRef label,
               bool* state,
               Ref<CheckboxOption> option)
      : label_(label), state_(state), option_(std::move(option)) {
#if defined(FTXUI_MICROSOFT_TERMINAL_FALLBACK)
    // Microsoft terminal do not use fonts able to render properly the default
    // radiobox glyph.
    if (option->checked == L"▣ ")
      option->checked = L"[X]";
    if (option->unchecked == L"☐ ")
      option->unchecked = L"[ ]";
#endif
  }

  ~CheckboxBase() override = default;

 private:
  // Component implementation.
  Element Render() override {
    bool is_focused = Focused();
    auto style = is_focused ? option_->style_focused : option_->style_unfocused;
    auto focus_management = is_focused ? focus : *state_ ? select : nothing;
    return hbox(text(*state_ ? option_->style_checked
                             : option_->style_unchecked),
                text(*label_) | style | focus_management) |
           reflect(box_);
  }

  bool OnEvent(Event event) override {
    if (event.is_mouse())
      return OnMouseEvent(event);

    if (event == Event::Character(' ') || event == Event::Return) {
      *state_ = !*state_;
      option_->on_change();
      return true;
    }
    return false;
  }

  bool OnMouseEvent(Event event) {
    if (!CaptureMouse(event))
      return false;
    if (!box_.Contain(event.mouse().x, event.mouse().y))
      return false;

    TakeFocus();

    if (event.mouse().button == Mouse::Left &&
        event.mouse().motion == Mouse::Pressed) {
      *state_ = !*state_;
      option_->on_change();
      return true;
    }

    return false;
  }

  ConstStringRef label_;
  bool* const state_;
  Box box_;
  Ref<CheckboxOption> option_;
};
}  // namespace

/// @brief Draw checkable element.
/// @param label The label of the checkbox.
/// @param checked Whether the checkbox is checked or not.
/// @ingroup component
/// @see CheckboxBase
///
/// ### Example
///
/// ```cpp
/// auto screen = ScreenInteractive::FitComponent();
/// std::wstring label = L"Make a sandwidth";
/// bool checked = false;
/// Component checkbox = Checkbox(&label, &checked);
/// screen.Loop(checkbox)
/// ```
///
/// ### Output
///
/// ```bash
/// ☐ Make a sandwitch
/// ```
Component Checkbox(ConstStringRef label,
                   bool* checked,
                   Ref<CheckboxOption> option) {
  return Make<CheckboxBase>(label, checked, std::move(option));
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
