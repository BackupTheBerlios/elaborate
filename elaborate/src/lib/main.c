#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <Ecore.h>
#include <Ecore_Con.h>
#include "E_Irc.h"

static int _server_add  (void *data, int type, void *event);
static int _server_del  (void *data, int type, void *event);
static int _server_data (void *data, int type, void *event);

static Ecore_List *clients;

EIRC int
e_irc_init(void) 
{
   ecore_con_init();
   clients = ecore_list_new();
   ecore_list_set_free_cb(clients, free);
   return 1;
}

EIRC int
e_irc_shutdown(void) 
{
   E_Irc_Client *client;

   ecore_list_goto_first(clients);
   while ((client = ecore_list_next(clients)) != NULL) 
     {
	if (client->add_hdl)
	  ecore_event_handler_del(client->add_hdl);
	if (client->del_hdl)
	  ecore_event_handler_del(client->del_hdl);
	if (client->data_hdl)
	  ecore_event_handler_del(client->data_hdl);
	ecore_list_remove(clients);
     }
   free(client);
   client = NULL;
   ecore_list_destroy(clients);
   ecore_con_shutdown();
   return 1;
}

EIRC E_Irc_Client *
e_irc_client_new(const char *address) 
{
   E_Irc_Client *client;
   
   client = calloc(1, sizeof(E_Irc_Client));
   if (!client) return NULL;
   client->state = 0;
   
   client->server = ecore_con_server_connect(ECORE_CON_REMOTE_SYSTEM, address, 6667, client);
   if (client->server == NULL)
     printf("Cannot connect to server\n");
   client->add_hdl = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_ADD, _server_add, client);
   client->del_hdl = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DEL, _server_del, client);
   client->data_hdl = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DATA, _server_data, client);
   
   ecore_list_append(clients, client);
   
   return client;
}

static int 
_server_add(void *data, int type, void *event) 
{
   E_Irc_Client *client;
   
   client = data;
   if (!client) return 1;
   client->state = 1;
   return 0;
}

static int 
_server_del(void *data, int type, void *event) 
{
   E_Irc_Client *client;
   Ecore_Con_Event_Server_Del *ev;
   
   ev = event;
   client = data;
   if (!client) return 1;
   
   ecore_con_server_del(ev->server);
   client->state = 0;
   client->server = NULL;
   return 0;
}

static int 
_server_data(void *data, int type, void *event) 
{
   Ecore_Con_Event_Server_Data *ev;
   E_Irc_Client *client;
   char in[512], out[512];
   char h[64];
   int len;
   
   ev = event;
   client = data;
   if (!client) return 1;
   if (client->state == 0) return 1;

   len = sizeof(in) - 1;
   len = (((len) > (ev->size)) ? ev->size : len);
   memcpy(in, ev->data, len);
   in[len] = 0;
   client->data = strdup(in);
   if (client->recv_func)
     client->recv_func(client);
//   printf("Data: %i, %s\n", ev->size, (char *)ev->data);
   switch (client->state) 
     {
      case 1:
	len = snprintf(out, sizeof(out), "NICK :devilhorns_\n");
	ecore_con_server_send(ev->server, out, len);
	client->state++;
	break;
      case 2:
	gethostname(h, 64);
	len = snprintf(out, sizeof(out), "USER devilhorns_ %s irc.freenode.net :test\n", h);
	ecore_con_server_send(ev->server, out, len);
	client->state++;
	break;
      case 3:
	len = snprintf(out, sizeof(out), "JOIN #e\n");
	ecore_con_server_send(ev->server, out, len);
	client->state++;
      default:
	break;
     }
   return 0;
}

