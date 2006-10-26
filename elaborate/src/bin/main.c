#include <stdio.h>
#include <stdlib.h>
#include <etk/Etk.h>
#include <E_Irc.h>
#include "config.h"
#include "el.h"
#include "el_gui.h"
#include "el_config.h"
#include "el_config_gui.h"

static void _shutdown (void);

int
main(int argc, char ** argv) 
{
//   E_Irc_Client *client;
   
   if (!el_config_init()) 
     {
	printf("Cannot Init %s Config\n", PACKAGE_NAME);
	return -1;
     }
   if (!e_irc_init()) 
     {
	printf("Cannot Init Irc library\n");
	return -1;
     }
   if (!etk_init(&argc, &argv)) 
     {
	printf("Cannot Init Etk\n");
	return -1;
     }

   el_gui_init();
//   client = e_irc_client_new("irc.freenode.net");
//   client->recv_func = _el_gui_data;
   
   etk_main();
   _shutdown();
   return 0;
}

static void
_shutdown(void) 
{
   etk_shutdown();

   if (!e_irc_shutdown()) 
     printf("Cannot shutdown Irc library\n");

   if (!el_config_shutdown())
     printf("Cannot shutdown %s Config\n", PACKAGE_NAME);

   exit(0);
}
