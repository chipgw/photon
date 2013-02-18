#ifndef _PHOTON_ASSERT_H_
#define _PHOTON_ASSERT_H_

#ifdef assert
#undef assert
#endif

#define assert(expr, critical)  ((expr) ? (void)(0) : photon::ReportFailure(#expr, __FILE__, __LINE__, __func__, critical))
#define assert_critical(expr)   (assert(expr, true))
#define assert_soft(expr)       (assert(expr, false))

namespace photon{

void ReportFailure(const char* condition, const char* file, int line, const char* function, bool critical);

}
#endif
