#ifndef COLOR_SENSOR_V3_H
#define COLOR_SENSOR_V3_H

#include <Arduino.h>
#include <Wire.h>
#include <lib/Color.h>

enum Register : int {
    kMainCtrl = 0x00,
    kProximitySensorLED = 0x01,
    kProximitySensorPulses = 0x02,
    kProximitySensorRate = 0x03,
    kLightSensorMeasurementRate = 0x04,
    kLightSensorGain = 0x05,
    kPartID = 0x06,
    kMainStatus = 0x07,
    kProximityData = 0x08,
    kDataInfrared = 0x0A,
    kDataGreen = 0x0D,
    kDataBlue = 0x10,
    kDataRed = 0x13
};

enum MainControl : int {
    kRGBMode = 0x04,               // If bit is set to 1, color channels are activated
    kLightSensorEnable = 0x02,     // Enable light sensor
    kProximitySensorEnable = 0x01, // Proximity sensor active
    OFF = 0x00                     // Nothing on
};

enum GainFactor : int {
    kGain1x = 0x00,
    kGain3x = 0x01,
    kGain6x = 0x02,
    kGain9x = 0x03,
    kGain18x = 0x04
};

enum LEDCurrent : int {
    kPulse2mA = 0x00,
    kPulse5mA = 0x01,
    kPulse10mA = 0x02,
    kPulse25mA = 0x03,
    kPulse50mA = 0x04,
    kPulse75mA = 0x05,
    kPulse100mA = 0x06, // default value
    kPulse125mA = 0x07
};

enum LEDPulseFrequency : int {
    kFreq60kHz = 0x18,  // default value
    kFreq70kHz = 0x40,
    kFreq80kHz = 0x28,
    kFreq90kHz = 0x30,
    kFreq100kHz = 0x38
};

enum ProximitySensorResolution : int {
    kProxRes8bit = 0x00,
    kProxRes9bit = 0x08,
    kProxRes10bit = 0x10,
    kProxRes11bit = 0x18
};

enum ProximitySensorMeasurementRate : int {
    kProxRate6ms = 0x01,
    kProxRate12ms = 0x02,
    kProxRate25ms = 0x03,
    kProxRate50ms = 0x04,
    kProxRate100ms = 0x05, // default value
    kProxRate200ms = 0x06,
    kProxRate400ms = 0x07
};

enum ColorSensorResolution : int {
    kColorSensorRes20bit = 0x00,
    kColorSensorRes19bit = 0x10,
    kColorSensorRes18bit = 0x20,
    kColorSensorRes17bit = 0x30,
    kColorSensorRes16bit = 0x40,
    kColorSensorRes13bit = 0x50
};

enum ColorSensorMeasurementRate : int {
    kColorRate25ms = 0x00,
    kColorRate50ms = 0x01,
    kColorRate100ms = 0x02,
    kColorRate200ms = 0x03,
    kColorRate500ms = 0x04,
    kColorRate1000ms = 0x05,
    kColorRate2000ms = 0x07
};

class ColorSensorV3 {
    private:
        TwoWire* wire;
        int address;

        int read11BitInteger(int reg) {
            int readBuff[2];
            wire->beginTransmission(address);
            wire->write(reg);
            wire->endTransmission();
            wire->requestFrom(address, 2);
            for (int i = 0; i < 2; i++) readBuff[i] = wire->read();
            return (readBuff[0] & 0xFF) | ((readBuff[1] & 0xFF) << 8) & 0x7FF;
        }

        int read20BitInteger(int reg) {
            int readBuff[3];
            wire->beginTransmission(address);
            wire->write(reg);
            wire->endTransmission();
            wire->requestFrom(address, 3);
            for (int i = 0; i < 3; i++) readBuff[i] = wire->read();
            return (readBuff[0] & 0xFF) | ((readBuff[1] & 0xFF) << 8) | ((readBuff[2] & 0xFF) << 16) & 0x03FFFF;
        }

        void write8(int reg, int val) {
            wire->beginTransmission(address);
            wire->write(reg);
            wire->write(val);
            wire->endTransmission();
        }

    public:
        ColorSensorV3(TwoWire* wire = &Wire, int address = 0x52) {
            this->wire = wire;
            this->address = address;
        }

        void begin() {
            this->write8(Register::kMainCtrl, MainControl::kRGBMode | MainControl::kLightSensorEnable | MainControl::kProximitySensorEnable);
            this->write8(Register::kProximitySensorRate, ProximitySensorResolution::kProxRes11bit | ProximitySensorMeasurementRate::kProxRate100ms);
            this->write8(Register::kProximitySensorPulses, 32);
        }

        uint getRed() {
            return this->read20BitInteger(Register::kDataRed);
        }

        uint getGreen() {
            return this->read20BitInteger(Register::kDataGreen);
        }

        uint getBlue() {
            return this->read20BitInteger(Register::kDataBlue);
        }

        Color getColor() {
            float r = (float)this->getRed();
            float g = (float)this->getGreen();
            float b = (float)this->getBlue();
            float mag = r + g + b;
            return Color(r / mag, g / mag, b / mag);
        }

        uint getProximity() {
            return this->read11BitInteger(Register::kProximityData);
        }
};

#endif