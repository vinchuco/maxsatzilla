/// This file defines some preprocessing utils for error cases
/// warning cases and some specific error handling issues.

#ifndef PPUTILS_HH
#define PPUTILS_HH

#define MSZWarn(msg, ...)					\
  fprintf(stderr, "WARNING (%s:%d) ", __FILE__,  __LINE__);     \
  fprintf(stderr, msg, ##__VA_ARGS__);                          \
  fprintf(stderr, "\n");

#define MSZError(msg, ...)						      \
  fprintf(stderr, "ERROR (%s:%d) ", __FILE__, __LINE__);              \
  fprintf(stderr, msg, ##__VA_ARGS__);                                \
  fprintf(stderr, "\n");                                              \
  exit(EXIT_FAILURE);

#define MSZMemOut(msg) "ERROR"

#define MSZSegFault(msg) "ERROR"

// type shortcuts
typedef unsigned int uint;

#endif // PPUTILS_HH
