#include "PMSXSensor.h"

bool AirGradient_Internal::PMSXSensor::begin() {
    //Check if sensor is supposed to provide the particle reading, if not, return directly
    if (!!(getCurrentMeasurement() & Measurement::Particle)) {
        Serial.printf("%s can only provide Particle reading and it's disabled.", getName());
        return false;
    }
    _softwareSerial = std::make_unique<SoftwareSerial>(_rxPin, _txPin);
    _softwareSerial->begin(_baudRate);
    _sensor = std::make_unique<PMS>(*_softwareSerial);
    _wakeUpPm2();

    _wakerTicker.attach_scheduled(_wakupXSecs, [this] { _wakeUpPm2(); });
    return true;
}


void AirGradient_Internal::PMSXSensor::_wakeUpPm2() {
    _sensor->wakeUp();
    _readSleepTicker.once_scheduled(PMS_DELAY_BEFORE_READING_SECS, [this] { _getPm2DataSleep(); });
}

void AirGradient_Internal::PMSXSensor::_getPm2DataSleep() {

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

void AirGradient_Internal::PMSXSensor::getData(AirGradient_Internal::SensorData &data) const {
    //Check if sensor is supposed to provide the Particle reading, if not, return directly
    if (!!(getCurrentMeasurement() & Measurement::Particle)) {
        return;
    }
    data.PARTICLE_DATA = _data;
}

