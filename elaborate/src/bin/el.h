#ifndef EL_H
#define EL_H

#ifdef ELAPI
#undef ELAPI
#endif
#ifdef WIN32
# ifdef BUILDING_DLL
#  define ELAPI __declspec(dllexport)
# else
#  define ELAPI __declspec(dllimport)
# endif
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define ELAPI __attribute__ ((visibility("default")))
#  else
#   define ELAPI
#  endif
# else
#  define ELAPI
# endif
#endif

#define EL_NEW(s, n) (s *)calloc(n, sizeof(s))
#define EL_FREE(p) { if (p) {free(p); p = NULL;} }
   
#endif
