# Finding librt
# The module defines
# LibRT_LIBRARIES - the librt library

include(LibFindMacros)
include(CheckCSourceCompiles)

# get hints about paths
libfind_pkg_check_modules(LibRT_PKGCONF librt)

# library
find_library(LibRT_LIBRARY
    NAMES rt
    PATHS ${LibRT_PKGCONF_LIBRARY_DIRS}
    )

if(LibRT_LIBRARY)
    set(CMAKE_REQUIRED_LIBRARIES "${LibRT_LIBRARY}")
endif()

check_c_source_compiles("
#include <signal.h>
#include <time.h>

int main(){
struct sigevent event;
timer_t timer;
event.sigev_notify = SIGEV_SIGNAL;
event.sigev_signo = SIGINT;
event.sigev_value.sival_int = 0;
timer_create(CLOCK_REALTIME, &event, &timer);
return 0;
}" HAVE_WORKING_LibRT)

set(LibRT_PROCESS_LIBS LibRT_LIBRARY)
libfind_process(LibRT)

