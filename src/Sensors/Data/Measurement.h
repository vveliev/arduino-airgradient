#pragma once

#include "Utils/Enum.h"

namespace AirGradient_Internal {

    /**
     * Represent the type of data the sensor can provide. Can be multiple. Used as flag.
     */
    enum class Measurement : uint16_t {
        /**
         * Used to check flags, please don't assign it to your sensor.
         */
        None = 0,
        /**
         * Provide particle measurement (PM 1.0, PM 2.5, etc ...)
         */
        Particle = 1 << 0,
        /**
         * Provide CO2 measurement
         */
        CO2 = 1 << 1,
        /**
         * Provide Temperature measurement
         */
        Temperature = 1 << 2,
        /**
         * Provide humidity measurement
         */
        Humidity = 1 << 3,
        /**
         * Provide the boot time
         */
        BootTime = 1 << 4,
        /**
         * Total Volatile Organic Compounds sensor
         */
        TVOC = 1 << 5,

        /**
         * H2 Gas
         */
        H2 = 1 << 6,

        /***
         * Ethanol gas
         */
        ETHANOL = 1 << 7,

        /***
         * Pressure
         */
        Pressure = 1 << 8
    };


    ENUM_FLAG_OPERATORS(Measurement);
}
