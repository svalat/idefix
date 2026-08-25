#ifndef IDEFIX_TW_HPP
#define IDEFIX_TW_HPP

#include <twin-checker/CheckerApi.h>

#define TWTOSTR(var) (#var)
#define TWCHECK(fid,var) \
  do { \
    twin_register_site((fid+__LINE__), __FILE__, strlen(__FILE__)); \
    Kokkos::fence(); \
    auto tmp = Kokkos::create_mirror_view(var); \
    Kokkos::deep_copy(tmp, var); \
    Kokkos::fence(); \
    twin_check_double_fixable_array(tmp.data(), tmp.span(), TWTOSTR(var), strlen(TWTOSTR(var)), (fid+__LINE__), __LINE__); \
  } while(0)


#endif
