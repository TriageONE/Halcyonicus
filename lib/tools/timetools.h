//
// Created by Triage on 8/26/2023.
//

#ifndef HALCYONICUS_TIMETOOLS_H
#define HALCYONICUS_TIMETOOLS_H

#include <chrono>

class TIMETOOLS{

public:
    static long long getCurrentEpochTime() {
        auto currentTime = std::chrono::system_clock::now().time_since_epoch();
        return std::chrono::duration_cast<std::chrono::seconds>(currentTime).count();
    }

    static std::string getCurrentDateTimeLabel() {
#if (WIN32)
        // Get the current timepoint
        auto currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

        // Convert the timepoint to a struct tm for formatting
        std::tm timeInfo;
        localtime_s(&timeInfo, &currentTime); // Use localtime_s on Windows, localtime_r on Unix-like systems

        // Format the date and time
        std::ostringstream formattedDateTime;
        formattedDateTime << std::put_time(&timeInfo, "%Y/%m/%d %T");

        return formattedDateTime.str();
#else
        // Get the current time
        auto now = std::chrono::system_clock::now();

        // Convert it to a time_t object
        std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

        // Create a character array to hold the formatted date-time string
        char buffer[80];

        // Format the time as a string
        std::strftime(buffer, sizeof(buffer), "%Y/%m/%d %T", std::localtime(&currentTime));

        // Convert the character array to a string
        std::string dateTimeString(buffer);

        return dateTimeString;
#endif

    }
};
#endif //HALCYONICUS_TIMETOOLS_H
