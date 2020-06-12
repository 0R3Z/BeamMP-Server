///
/// Created by jojos38 on 28/01/2020
///


#include <fstream>
#include "logger.h"
#include <string>
void addToLog(const std::string& Data);
int loggerlevel;

void setLoggerLevel(int level) {
    //0 ALL 1 DEBUG 2 INFO 3 WARN 4 ERROR 5 OFF
    loggerlevel = level;
}

std::stringstream getDate() {
    // current date/time based on current system
    time_t now = time(nullptr);
    tm* ltm = localtime(&now);
    
    int month = 1 + ltm->tm_mon;
    int day = ltm->tm_mday;
    int hours = ltm->tm_hour;
    int minutes = ltm->tm_min;
    int seconds = ltm->tm_sec;

    std::string month_string;
    if (month < 10) month_string = "0" + std::to_string(month);
    else month_string = std::to_string(month);

    std::string day_string;
    if (day < 10) day_string = "0" + std::to_string(day);
    else day_string = std::to_string(day);

    std::string hours_string;
    if (hours < 10) hours_string = "0" + std::to_string(hours);
    else hours_string = std::to_string(hours);

    std::string minutes_string;
    if (minutes < 10) minutes_string = "0" + std::to_string(minutes);
    else minutes_string = std::to_string(minutes);

    std::string seconds_string;
    if (seconds < 10) seconds_string = "0" + std::to_string(seconds);
    else seconds_string = std::to_string(seconds);

    std::stringstream date;
    date
        << "["
        << day_string << "/"
        << month_string << "/"
        << 1900 + ltm->tm_year << " "
        << hours_string << ":"
        << minutes_string << ":"
        << seconds_string
        << "] ";
    return date;
}


void info(const std::string& toPrint) {
    if (loggerlevel <= 2){
        std::cout << getDate().str() << "[INFO] " << toPrint << std::endl;
        addToLog(getDate().str() + "[INFO] " + toPrint + "\n");
    }
}

void error(const std::string& toPrint) {
    if (loggerlevel <= 4) {
        std::cout << getDate().str() << "[ERROR] " << toPrint << std::endl;
        addToLog(getDate().str() + "[ERROR] " + toPrint + "\n");
    }
}


void warn(const std::string& toPrint) {
    if (loggerlevel <= 3) {
        std::cout << getDate().str() << "[WARN] " << toPrint << std::endl;
        addToLog(getDate().str() + "[WARN] " + toPrint + "\n");
    }
}


void debug(const std::string& toPrint) {
    if (loggerlevel <= 1) {
        std::cout << getDate().str() << "[DEBUG] " << toPrint << std::endl;
        addToLog(getDate().str() + "[DEBUG] " + toPrint + "\n");
    }
}