#ifndef _E_IRC_H
#define _E_IRC_H

#include <Ecore.h>
#include <Ecore_Con.h>
#include "errors.h"

#ifdef EIRC
# undef EIRC
#endif

#ifdef WIN32
# ifdef BUILDING_DLL
#  define EIRC __declspec(dllexport)
# else
#  define EIRC __declspec(dllimport)
# endif
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EIRC __attribute__ ((visibility("default")))
#  else
#   define EIRC
#  endif
# else
#  define EIRC
# endif
#endif

//#ifdef __cplusplus
//extern "C" {
//#endif

typedef struct _E_Irc_Client E_Irc_Client;
struct _E_Irc_Client 
{
   Ecore_Con_Server *server;
   Ecore_Event_Handler *add_hdl;
   Ecore_Event_Handler *del_hdl;
   Ecore_Event_Handler *data_hdl;

   char *data;
   void (*recv_func) (void *data);
   void (*send_func) (void *data);
   
   int state;
};
   
EIRC int           e_irc_init       (void);
EIRC int           e_irc_shutdown   (void);
EIRC E_Irc_Client *e_irc_client_new (const char *address);

//#ifdef __cplusplus
//}
//#endif

#endif
