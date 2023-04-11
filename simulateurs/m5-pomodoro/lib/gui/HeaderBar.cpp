#include "HeaderBar.h"

using namespace Codingfield::UI;

extern const uint8_t image_data_wifi0[];
extern const uint8_t image_data_wifi1[];
extern const uint8_t image_data_wifi2[];
extern const uint8_t image_data_wifi3[];

extern const uint8_t image_data_battery0[];
extern const uint8_t image_data_battery1[];
extern const uint8_t image_data_battery2[];
extern const uint8_t image_data_battery3[];

void HeaderBar::SetStatusRight(const HeaderBar::BatteryStatuses status) {
  if(batteryStatus != status) {
    oldBatteryStatus = batteryStatus;
    batteryStatus = status;
    batteryPosition = PositionStatus::Right;
  }
}

void HeaderBar::SetStatusLeft(const HeaderBar::BatteryStatuses status) {
  if(batteryStatus != status) {
    oldBatteryStatus = batteryStatus;
    batteryStatus = status;
    batteryPosition = PositionStatus::Left;
  }
}

void HeaderBar::SetStatusRight(const HeaderBar::WirelessStatuses status) {
  if(wirelessStatus != status) {
    oldWirelessStatus = wirelessStatus;
    wirelessStatus = status;
    wirelessPosition = PositionStatus::Right;
  }
}

void HeaderBar::SetStatusLeft(const HeaderBar::WirelessStatuses status) {
  if(wirelessStatus != status) {
    oldWirelessStatus = wirelessStatus;
    wirelessStatus = status;
    wirelessPosition = PositionStatus::Left;
  }
}

void HeaderBar::SetTitleRight(const std::string& t) {
  if(titleRight != t) {
    oldTitleRight = titleRight;
    titleRight = t;
  }
}

void HeaderBar::SetTitleLeft(const std::string& t) {
  if(titleLeft != t) {
    oldTitleLeft = titleLeft;
    titleLeft = t;
  }
}

void HeaderBar::SetTitle(const std::string& t) {
  if(title != t) {
    oldTitle = title;
    title = t;
  }
}

void HeaderBar::SetColor(Color c) {
  this->color = c;
  Bar::SetBackgroundColor(c);
}

void HeaderBar::SetTextColor(Color c) {
  this->textColor = c;
}

void HeaderBar::Draw() {
  if(IsHidden()) return;
  bool wasInvalidated = isInvalidated;
  Bar::Draw();

  if((wasInvalidated || (batteryStatus != oldBatteryStatus)) && (batteryPosition == PositionStatus::Right)) {
    const uint8_t* batterybmp = image_data_battery0;
    switch(batteryStatus) {
      case BatteryStatuses::WeakBattery: batterybmp = image_data_battery1; break;
      case BatteryStatuses::MediumBattery: batterybmp = image_data_battery2; break;
      case BatteryStatuses::FullBattery: batterybmp = image_data_battery3; break;
      default:
      case BatteryStatuses::LowBattery:
        batterybmp = image_data_battery0;
        break;
    }

    M5.Lcd.setBitmapColor(BLACK, color);
    M5.Lcd.pushImage(295, 0, 25, 25, const_cast<uint8_t*>(batterybmp), false);

    oldBatteryStatus = batteryStatus;
  }  

  if((wasInvalidated || (batteryStatus != oldBatteryStatus)) && (batteryPosition == PositionStatus::Left)) {
    const uint8_t* batterybmp = image_data_battery0;
    switch(batteryStatus) {
      case BatteryStatuses::WeakBattery: batterybmp = image_data_battery1; break;
      case BatteryStatuses::MediumBattery: batterybmp = image_data_battery2; break;
      case BatteryStatuses::FullBattery: batterybmp = image_data_battery3; break;
      default:
      case BatteryStatuses::LowBattery:
        batterybmp = image_data_battery0;
        break;
    }

    M5.Lcd.setBitmapColor(BLACK, color);
    M5.Lcd.pushImage(1, 0, 25, 25, const_cast<uint8_t*>(batterybmp), false);

    oldBatteryStatus = batteryStatus;
  }  

  if((wasInvalidated || (wirelessStatus != oldWirelessStatus)) && (wirelessPosition == PositionStatus::Right)) {
    const uint8_t* wifibmp = image_data_wifi0;
    switch(wirelessStatus) {
      case WirelessStatuses::WeakSignal: wifibmp = image_data_wifi1; break;
      case WirelessStatuses::MediumSignal: wifibmp = image_data_wifi2; break;
      case WirelessStatuses::FullSignal: wifibmp = image_data_wifi3; break;
      default:
      case WirelessStatuses::NoSignal:
        wifibmp = image_data_wifi0;
        break;
    }

    M5.Lcd.setBitmapColor(BLACK, color);
    M5.Lcd.pushImage(295, 0, 25, 25, const_cast<uint8_t*>(wifibmp), false);

    oldWirelessStatus = wirelessStatus;
  }

  if((wasInvalidated || (wirelessStatus != oldWirelessStatus)) && (wirelessPosition == PositionStatus::Left)) {
    const uint8_t* wifibmp = image_data_wifi0;
    switch(wirelessStatus) {
      case WirelessStatuses::WeakSignal: wifibmp = image_data_wifi1; break;
      case WirelessStatuses::MediumSignal: wifibmp = image_data_wifi2; break;
      case WirelessStatuses::FullSignal: wifibmp = image_data_wifi3; break;
      default:
      case WirelessStatuses::NoSignal:
        wifibmp = image_data_wifi0;
        break;
    }

    M5.Lcd.setBitmapColor(BLACK, color);
    M5.Lcd.pushImage(1, 0, 25, 25, const_cast<uint8_t*>(wifibmp), false);

    oldWirelessStatus = wirelessStatus;
  }

  if(wasInvalidated || (oldTitle != title)) {
    M5.Lcd.setTextDatum(TC_DATUM);

    M5.Lcd.setTextColor(color);
    M5.Lcd.drawString(oldTitle.c_str(), 160, 5);

    M5.Lcd.setTextColor(textColor);
    M5.Lcd.drawString(title.c_str(), 160, 5);

    oldTitle = title;
  }

  if((wasInvalidated || (oldTitleRight != titleRight)) && (batteryPosition != PositionStatus::Right || wirelessPosition != PositionStatus::Right)) {
    M5.Lcd.setTextDatum(TL_DATUM);

    M5.Lcd.setTextColor(color);
    M5.Lcd.drawString(oldTitleRight.c_str(), 240, 5);

    M5.Lcd.setTextColor(textColor);
    M5.Lcd.drawString(titleRight.c_str(), 240, 5);

    oldTitleRight = titleRight;
  }  

  if((wasInvalidated || (oldTitleLeft != titleLeft)) && (batteryPosition != PositionStatus::Left || wirelessPosition != PositionStatus::Left)) {
    M5.Lcd.setTextDatum(TL_DATUM);

    M5.Lcd.setTextColor(color);
    M5.Lcd.drawString(oldTitleLeft.c_str(), 1, 5);

    M5.Lcd.setTextColor(textColor);
    M5.Lcd.drawString(titleLeft.c_str(), 1, 5);

    oldTitleLeft = titleLeft;
  }
  isInvalidated = false;
}

void HeaderBar::Run() {
    Draw();
}
