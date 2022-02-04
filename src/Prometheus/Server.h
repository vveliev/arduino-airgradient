#pragma once

#include <ESP8266WebServer.h>

#include <utility>
#include "Metrics/MetricGatherer.h"
#include "AQI/AQICalculator.h"

namespace AirGradient {
    class Server {


    public:

        Server(int serverPort, const char *deviceId, std::shared_ptr<AirGradient::MetricGatherer> metrics,
               std::shared_ptr<AirGradient::AQICalculator> aqiCalculator) : _serverPort(serverPort),
                                                                            _deviceId(deviceId),
                                                                            _metrics(std::move(metrics)),
                                                                            _aqiCalculator(std::move(aqiCalculator)) {
            _server = std::make_unique<ESP8266WebServer>(_serverPort);
        }

        /**
         * Handle requests made to the underlying webserver to get the metrics
         */
        void handleRequests();

        /**
         * Prepare the webserver
         */
        void begin();

        virtual ~Server();

    private:
        int _serverPort;
        const char *_deviceId;

        std::unique_ptr<ESP8266WebServer> _server;
        std::shared_ptr<AirGradient::MetricGatherer> _metrics;
        std::shared_ptr<AirGradient::AQICalculator> _aqiCalculator;


        void _handleRoot();

        void _handleNotFound();

        String _generateMetrics();

        String _getIdString(const char *labelType = nullptr, const char *labelValue = nullptr) const;

    };

}