#ifndef CICFLOW_BASE_LOG_SEVERITY_H_
#define CICFLOW_BASE_LOG_SEVERITY_H_

#include <stdio.h>
#include <stdlib.h>

#include "cicflow/base/attributes.h"
#include "cicflow/base/config.h"

#define NUM_SEVERITY 9

typedef enum {
  CIC_LOG_TRACE = 0,  // - finer-grained information events than the DEBUG
  CIC_LOG_DEBUG,      // - fine-grained information events that are most useful
                      // to debug an application
  CIC_LOG_INFO,       // - information messages that highlight the progress
                      // of the application at coarse-grained level
  CIC_LOG_NOTICE,     //
  CIC_LOG_WARNING,    // - potentially harmful situations
  CIC_LOG_ERROR,      // - error events that migh still allow the application
                      // to continue running
  CIC_LOG_CRITICAL,   //
  CIC_LOG_ALERT,      //
  CIC_LOG_EMERGENCY,  //
  CIC_LOG_FATAL,      //
} LogSeverity;

LogSeverity *log_severities();

const char *log_severity_name(LogSeverity s);

#endif  // CICFLOW_BASE_LOG_SEVERITY_H_
