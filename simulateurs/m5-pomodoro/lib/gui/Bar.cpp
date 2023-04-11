#include "Bar.h"

using namespace Codingfield::UI;

Color Bar::GetBackgroundColor() {
    return this->color;
}

void Bar::SetBackgroundColor(Color c) {
    this->color = c;
}

void Bar::Draw() {
  if(IsHidden()) return;
  if(isInvalidated)
    M5.Lcd.fillRect(position.x, position.y, size.width, size.height, color);

  isInvalidated = false;
}

void Bar::Run() {
  Draw();
}
