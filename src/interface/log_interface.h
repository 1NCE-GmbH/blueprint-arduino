#ifndef LOG_INTERFACE_H_
#define LOG_INTERFACE_H_

/**
 *  @brief implementation of logging macros for Arduino with Mbed OS.
 */
    #define NceOSLogInfo(  ... )  printf(__VA_ARGS__)
    #define NceOSLogError( ... ) printf(__VA_ARGS__)
    #define NceOSLogWarn(  ... ) printf(__VA_ARGS__)
    #ifdef NCE_SDK_LOG_LEVEL_DEBUG
    #define NceOSLogDebug( ... ) printf(__VA_ARGS__)
    #else 
    #define NceOSLogDebug( format, ... )
    #endif

#endif /* ifndef LOG_INTERFACE_H_ */
