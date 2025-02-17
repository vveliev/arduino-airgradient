#include "PrometheusServer.h"

AirGradient_Internal::PrometheusServer::~PrometheusServer() {
    _server->stop();
    _server->close();
}

void AirGradient_Internal::PrometheusServer::handleRequests() {
    _server->handleClient();
}

void AirGradient_Internal::PrometheusServer::begin() {
    _server->on("/", [this] { _handleRoot(); });
    _server->on("/metrics", [this] { _handleRoot(); });
    _server->onNotFound([this] { _handleNotFound(); });

    _server->begin();
    Serial.println("HTTP server started at ip " + WiFi.localIP().toString() + ":" + String(_serverPort));
}

String AirGradient_Internal::PrometheusServer::_generateMetrics() {
    String message = "";
    String idString = _getIdString();

    auto metrics = _metrics->getData();
    auto sensorType = _metrics->getMeasurements();

    if (!(sensorType & Measurement::Particle)) {
        message += "# HELP particle_count Count of Particulate Matter in µg/m3\n";
        message += "# TYPE particle_count gauge\n";
        message += "particle_count";
        message += _getIdString("type", "PM2.5");
        message += String(metrics.PARTICLE_DATA.PM_2_5);
        message += "\n";

        message += "# HELP particle_count Count of Particulate Matter in µg/m3\n";
        message += "# TYPE particle_count gauge\n";
        message += "particle_count";
        message += _getIdString("type", "PM1.0");
        message += String(metrics.PARTICLE_DATA.PM_1_0);
        message += "\n";

        message += "# HELP particle_count Count of Particulate Matter in µg/m3\n";
        message += "# TYPE particle_count gauge\n";
        message += "particle_count";
        message += _getIdString("type", "PM10.0");
        message += String(metrics.PARTICLE_DATA.PM_10_0);
        message += "\n";

        if (_aqiCalculator->isAQIAvailable()) {
            message += "# HELP air_quality_index Air Quality Index (AQI)\n";
            message += "# TYPE air_quality_index gauge\n";
            message += "air_quality_index";
            message += _getIdString("type", "PM2.5");
            message += String(_aqiCalculator->getAQI());
            message += "\n";
        }
    }

    if (!(sensorType & Measurement::CO2)) {
        message += "# HELP rco2 CO2 value, in ppm\n";
        message += "# TYPE rco2 gauge\n";
        message += "rco2";
        message += idString;
        message += String(metrics.GAS_DATA.CO2);
        message += "\n";

    }

    if (!(sensorType & Measurement::Temperature)) {
        message += "# HELP atmp Temperature, in degrees Celsius\n";
        message += "# TYPE atmp gauge\n";
        message += "atmp";
        message += idString;
        message += String(metrics.TMP);
        message += "\n";
    }

    if (!(sensorType & Measurement::Humidity)) {
        message += "# HELP rhum Relative humidity, in percent\n";
        message += "# TYPE rhum gauge\n";
        message += "rhum";
        message += idString;
        message += String(metrics.HUM);
        message += "\n";
    }
    
    if (!(sensorType & Measurement::Pressure)) {
        message += "# HELP apre Athmospherique pressure, in hPa\n";
        message += "# TYPE apre gauge\n";
        message += "apre";
        message += idString;
        message += String(metrics.PRE);
        message += "\n";
    }

    if (!(sensorType & Measurement::BootTime)) {
        message += "# HELP sensors_boot_time AirGradient_Internal boot time, in unixtime.\n";
        message += "# TYPE sensors_boot_time gauge\n";
        message += "sensors_boot_time" + idString + String(metrics.BOOT_TIME) + "\n";
    }

    if (!(sensorType & Measurement::TVOC)) {
        message += "# HELP total_volatile_organic_compounds total of harmful gas detected.\n";
        message += "# TYPE total_volatile_organic_compounds gauge\n";
        message += "total_volatile_organic_compounds" + idString + String(metrics.GAS_DATA.TVOC) + "\n";
    }

    if (!(sensorType & Measurement::ETHANOL)) {
        message += "# HELP ethanol ethanol level.\n";
        message += "# TYPE ethanol gauge\n";
        message += "ethanol" + idString + String(metrics.GAS_DATA.ETHANOL) + "\n";
    }

    if (!(sensorType & Measurement::H2)) {
        message += "# HELP hydrogen hydrogen level.\n";
        message += "# TYPE hydrogen gauge\n";
        message += "hydrogen" + idString + String(metrics.GAS_DATA.H2) + "\n";
    }


    return message;
}

String AirGradient_Internal::PrometheusServer::_getIdString(const char *labelType, const char *labelValue) const {
    if (labelType == nullptr || labelValue == nullptr) {

        return "{id=\"" + String(_deviceId) + "\",mac=\"" + WiFi.macAddress().c_str() + "\"}";
    }
    return "{id=\"" + String(_deviceId) + "\",mac=\"" + WiFi.macAddress().c_str() + "\"," + labelType + "=\"" +
           labelValue + "\"}";
}

void AirGradient_Internal::PrometheusServer::_handleRoot() {
    _server->send(200, "text/plain", _generateMetrics());
}

void AirGradient_Internal::PrometheusServer::_handleNotFound() {
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += _server->uri();
    message += "\nMethod: ";
    message += (_server->method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += _server->args();
    message += "\n";
    for (int i = 0; i < _server->args(); i++) {
        message += " " + _server->argName(i) + ": " + _server->arg(i) + "\n";
    }
    _server->send(404, "text/html", message);
}
