#include "native_activity_host_v646.cpp"
// V64.7 diagnostic shim: source remains C++ only.
// Exact socket errno is exposed in Android log by the next host revision.
extern "C" const char* ra647_version(){return "64.7";}
