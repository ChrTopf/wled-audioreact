//
// Created by chrtopf on 04.06.23.
//

#ifndef SRC_LOG_H
#define SRC_LOG_H
#include <iostream>
#include <utility>

#define ANSI_RESET "\u001B[0m"
#define ANSI_RED "\u001b[31m"
#define ANSI_YELLOW "\u001B[33m"
#define ANSI_GREEN "\u001b[32m"
#define ANSI_PURPLE "\u001B[35m"

using namespace std;

enum LOG_LEVEL{
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

class Log {
private:
    //a list of labels ordered by their log level (0 to 3)
    inline static const string LABEL[] = {"DBUG", "INFO", "WARN", "FATA"};
    //a list of colors ordered by their log level matching the labels (0 to 3)
    inline static const string COLOR[] = {ANSI_PURPLE, ANSI_GREEN, ANSI_YELLOW, ANSI_RED};
    //the method for printing every log message
    static void out(LOG_LEVEL level, const string& msg);
    static bool isColor;
    static LOG_LEVEL logLevel;
public:
    /**
     * Enable or disable colored log output. Only the log level is going to be colored.
     * @param enabled true for enabling the color.
     */
    static void enableColor(bool enabled);

    /**
     * Set the minimum level of all log messages. If you set it to INFO no debug messages are going to be logged.
     * Default: INFO
     * @param level The minimum log level to be logged.
     */
    static void setLogLevel(LOG_LEVEL level);

    /**
     * Set the minimum level of all log messages. If you set it to 1 no debug messages are going to be logged.
     * Default: 1
     * @param level The minimum log level to be logged. Value between 0 and 3
     */
    static void setLogLevel(int level);

    /**
     * Log a debug message.
     * @param msg The message.
     */
    inline static void d(const string& msg){ out(DEBUG, msg);}
    /**
     * Log a debug message.
     * @param msg The message.
     */
    inline static void D(const string& msg){out(DEBUG, msg);}

    /**
     * Log an info message.
     * @param msg The message.
     */
    inline static void i(const string& msg){ out(INFO, msg);}
    /**
     * Log an info message.
     * @param msg The message.
     */
    inline static void I(const string& msg){out(INFO, msg);}

    /**
     * Log a warning message.
     * @param msg The message.
     */
    inline static void w(const string& msg){out(WARNING, msg);}
    /**
     * Log a warning message.
     * @param msg The message.
     */
    inline static void W(const string& msg){out(WARNING, msg);}

    /**
     * Log an error message.
     * @param msg The message.
     */
    inline static void e(const string& msg){out(ERROR, msg);}
    /**
     * Log an error message.
     * @param msg The message.
     */
    inline static void E(const string& msg){out(ERROR, msg);}
};


#endif //SRC_LOG_H
