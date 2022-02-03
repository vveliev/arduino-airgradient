#include "PMSXSensor.h"

void AirGradient::PMSXSensor::begin() {

    _softwareSerial = std::make_unique<SoftwareSerial>(_rxPin, _txPin);
    _softwareSerial->begin(_baudRate);
    _sensor = std::make_unique<PMS>(*_softwareSerial);
    _wakeUpPm2();

    _wakerTicker.attach_scheduled(_wakupXSecs, [this] { _wakeUpPm2(); });

}


AirGradient::PMSXSensor::PMSXSensor() : PMSXSensor(D5, D6, PMS_DEFAULT_BAUDRATE, PMS_DEFAULT_WAKE_SECS) {}

AirGradient::PMSXSensor::PMSXSensor(uint8_t rxPin, uint8_t txPin, uint16_t baudRate, uint16_t wakupXSecs) :
        _rxPin(rxPin),
        _txPin(txPin),
        _baudRate(baudRate),
        _wakupXSecs(wakupXSecs) {}

void AirGradient::PMSXSensor::_wakeUpPm2() {
    _sensor->wakeUp();
    _readSleepTicker.once_scheduled(PMS_DELAY_BEFORE_READING_SECS, [this] { _getPm2DataSleep(); });
}

void AirGradient::PMSXSensor::_getPm2DataSleep() {
    auto previousReading = _data.PM_2_5;
    PMS::DATA data{};
    if (!_sensor->readUntil(data, 2000)) {
        Serial.println("Couldn't get a reading of PMS sensor");
        return;
    }

    //Sometimes the sensor give 0 when there isn't any value
    //let's check that it's possible because the previous value was already under 10
    if (data.PM_AE_UG_2_5 > 0 || previousReading < 10) {
        _data.PM_2_5 = data.PM_AE_UG_2_5;
        _data.PM_10_0 = data.PM_AE_UG_10_0;
        _data.PM_1_0 = data.PM_AE_UG_1_0;
        _sensor->sleep();
    }
}

void AirGradient::PMSXSensor::updateData(AirGradient::SensorData &data) const {
    data.ParticleData = _data;
}