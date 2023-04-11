#pragma once

#include "Bar.h"

namespace Codingfield {
  namespace UI {
    class HeaderBar : public Bar {
    public:
      enum class BatteryStatuses {UnknownBattery, LowBattery, WeakBattery, MediumBattery, FullBattery};
      enum class WirelessStatuses {UnknownSignal, NoSignal, WeakSignal, MediumSignal, FullSignal};
      enum class PositionStatus {NoPosition, Left, Right};
      HeaderBar() : Bar() {}
      HeaderBar(Widget* parent, Point position, int32_t height) : Bar(parent, position, height) {}
      void Draw() override;
      void Run() override;      
      void SetStatusRight(const BatteryStatuses status);
      void SetStatusLeft(const BatteryStatuses status);
      void SetStatusRight(const WirelessStatuses status);
      void SetStatusLeft(const WirelessStatuses status);
      void SetTitleRight(const std::string& t);
      void SetTitleLeft(const std::string& t);
      void SetTitle(const std::string& t);
      void SetColor(Color c);
      void SetTextColor(Color c);

    private:
      Color color = WHITE;
      Color textColor = BLACK;
      BatteryStatuses batteryStatus = BatteryStatuses::LowBattery;
      BatteryStatuses oldBatteryStatus = BatteryStatuses::UnknownBattery;
      PositionStatus batteryPosition = PositionStatus::NoPosition;
      WirelessStatuses wirelessStatus = WirelessStatuses::NoSignal;
      WirelessStatuses oldWirelessStatus = WirelessStatuses::UnknownSignal;
      PositionStatus wirelessPosition = PositionStatus::NoPosition;
      std::string titleRight;
      std::string oldTitleRight;
      std::string titleLeft;
      std::string oldTitleLeft;
      std::string title;
      std::string oldTitle;
    };
  }
}
