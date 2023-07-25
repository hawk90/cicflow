
#include "cicflow/base/log_severity.h"

LogSeverity *log_severities() {
  LogSeverity *log_severities = (LogSeverity *)malloc(sizeof(LogSeverity) * NUM_SEVERITY);
  if (log_severities == NULL) goto ERROR;

  for (int i = CIC_LOG_TRACE; i <= CIC_LOG_EMERGENCY; ++i) {
    log_severities[i] = (LogSeverity)i;
  }
  return log_severities;
ERROR:
  return NULL;
}

const char *log_severity_name(LogSeverity s) {
  switch (s) {
    case (CIC_LOG_TRACE):
      return "TRACE";
    case (CIC_LOG_DEBUG):
      return "DEBUG";
    case (CIC_LOG_INFO):
      return "INFO";
    case (CIC_LOG_NOTICE):
      return "NOTICE";
    case (CIC_LOG_WARNING):
      return "WARNING";
    case (CIC_LOG_ERROR):
      return "ERROR";
    case (CIC_LOG_CRITICAL):
      return "ERROR";
    case (CIC_LOG_ALERT):
      return "ALERT";
    case (CIC_LOG_EMERGENCY):
      return "EMERGENCY";
    case (CIC_LOG_FATAL):
      return "FATAL";
  }

  return "UNKNOWN";
}
