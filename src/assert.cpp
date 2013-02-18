#include <photon_assert.h>
#include <photon_core.h>

namespace photon{

void ReportFailure(const char *condition, const char *file, int line, const char *function, bool critical){
    PrintToLog("ASSERTION FAILED: \"%s\" in \"%s:%i: %s", condition, file, line, function);
    if(critical){
        abort();
    }
}

}
