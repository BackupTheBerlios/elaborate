#include <stdio.h>
#include <stdlib.h>
#include <Eet.h>
#include <Ecore.h>
#include <Evas.h>
#include "config.h"
#include "el.h"
#include "el_config.h"
#include "el_network.h"

static void                   _el_networks_new       (void);
static Config_Network_Server *_el_network_server_new (const char *addr, int port);

ELAPI Config_Networks *el_config_nets = NULL;

static EL_Config_DD *_el_config_nets_edd = NULL;
static EL_Config_DD *_el_config_net_edd = NULL;
static EL_Config_DD *_el_config_net_serv_edd = NULL;

ELAPI int
el_network_init(void) 
{
   _el_config_net_serv_edd = EL_CONFIG_DD_NEW("EL_Config_Network_Server", Config_Network_Server);
   #undef T
   #undef D
   #define T Config_Network_Server
   #define D _el_config_net_serv_edd
   EL_CONFIG_VAL(D, T, address, EET_T_STRING);
   EL_CONFIG_VAL(D, T, port, EET_T_INT);
   
   _el_config_net_edd = EL_CONFIG_DD_NEW("EL_Config_Network", Config_Network);
   #undef T
   #undef D
   #define T Config_Network
   #define D _el_config_net_edd
   EL_CONFIG_VAL(D, T, name, EET_T_STRING);
   EL_CONFIG_LIST(D, T, servers, _el_config_net_serv_edd);

   _el_config_nets_edd = EL_CONFIG_DD_NEW("EL_Config_Networks", Config_Networks);
   #undef T
   #undef D
   #define T Config_Networks
   #define D _el_config_nets_edd
   EL_CONFIG_LIST(D, T, networks, _el_config_net_edd);
   
   el_config_nets = el_network_load();
   if (!el_config_nets) 
     _el_networks_new();

   if (!el_config_nets)
     return 0;   
   return 1;
}

ELAPI int
el_network_shutdown(void) 
{
   EL_FREE(el_config_nets);
   EL_CONFIG_DD_FREE(_el_config_net_serv_edd);
   EL_CONFIG_DD_FREE(_el_config_net_edd);
   EL_CONFIG_DD_FREE(_el_config_nets_edd);   
   return 1;
}

ELAPI int
el_network_save(void) 
{
   Eet_File *ef;
   char buf[4096];
   char *homedir;
   int ok = 0;
   
   homedir = getenv("HOME");
   if (!homedir) 
     {
	printf("Cannot get homedir\n");
	return 0;
     }
   snprintf(buf, sizeof(buf), "%s/.e/%s", homedir, PACKAGE);
   ecore_file_mkdir(buf);
   snprintf(buf, sizeof(buf), "%s/.e/%s/network.cfg", homedir, PACKAGE);
   ef = eet_open(buf, EET_FILE_MODE_WRITE);
   if (ef) 
     {
	ok = eet_data_write(ef, _el_config_nets_edd, "config", el_config_nets, 1);
	eet_close(ef);
     }
   return ok;   
}

ELAPI void *
el_network_load(void) 
{
   Eet_File *ef;
   char buf[4096];
   char *homedir;
   void *data = NULL;
   
   homedir = getenv("HOME");
   if (!homedir) 
     {
	printf("Cannot get homedir\n");
	return 0;
     }
   snprintf(buf, sizeof(buf), "%s/.e/%s/network.cfg", homedir, PACKAGE);
   ef = eet_open(buf, EET_FILE_MODE_READ);
   if (ef) 
     {
	data = eet_data_read(ef, _el_config_nets_edd, "config");
	eet_close(ef);
     }
   return data;   
}

ELAPI void *
el_network_find(const char *name) 
{
   Evas_List *l;
   
   if (!name) return NULL;
   if (!el_config_nets) return NULL;
   for (l = el_config_nets->networks; l; l = l->next) 
     {
	Config_Network *cn;
	
	cn = l->data;
	if (!cn) continue;
	if (!strcmp(cn->name, name))
	  return cn;
     }
   return NULL;
}

static void 
_el_networks_new(void) 
{
   Config_Network *cn;
   Config_Network_Server *cs;
   
   el_config_nets = EL_NEW(Config_Networks, 1);

   cn = EL_NEW(Config_Network, 1);
   cn->name = evas_stringshare_add("2600net");
   cs = _el_network_server_new("irc.2600.net", 6667);
   cn->servers = evas_list_append(cn->servers, cs);
   el_config_nets->networks = evas_list_append(el_config_nets->networks, cn);

   cn = EL_NEW(Config_Network, 1);
   cn->name = evas_stringshare_add("AccessIRC");
   cs = _el_network_server_new("irc.accessirc.net", 6667);
   cn->servers = evas_list_append(cn->servers, cs);
   cs = _el_network_server_new("eu.accessirc.net", 6667);
   cn->servers = evas_list_append(cn->servers, cs);
   el_config_nets->networks = evas_list_append(el_config_nets->networks, cn);
   
   cn = EL_NEW(Config_Network, 1);
   cn->name = evas_stringshare_add("AfterNET");
   cs = _el_network_server_new("irc.afternet.org", 6667);
   cn->servers = evas_list_append(cn->servers, cs);
   cs = _el_network_server_new("us.afternet.org", 6667);
   cn->servers = evas_list_append(cn->servers, cs);
   cs = _el_network_server_new("eu.afternet.org", 6667);
   cn->servers = evas_list_append(cn->servers, cs);   
   el_config_nets->networks = evas_list_append(el_config_nets->networks, cn);

   cn = EL_NEW(Config_Network, 1);
   cn->name = evas_stringshare_add("Aitvaras");
   cs = _el_network_server_new("irc.data.lt", 6668);
   cn->servers = evas_list_append(cn->servers, cs);
   cs = _el_network_server_new("irc-ssl.omnitel.net", 6668);
   cn->servers = evas_list_append(cn->servers, cs);
   cs = _el_network_server_new("irc-ssl.le.lt", 9999);
   cn->servers = evas_list_append(cn->servers, cs);
   cs = _el_network_server_new("irc.data.lt", 6667);
   cn->servers = evas_list_append(cn->servers, cs);   
   cs = _el_network_server_new("irc.omnitel.net", 6667);
   cn->servers = evas_list_append(cn->servers, cs);   
   cs = _el_network_server_new("irc.ktu.lt", 6667);
   cn->servers = evas_list_append(cn->servers, cs);   
   cs = _el_network_server_new("irc.le.lt", 6667);
   cn->servers = evas_list_append(cn->servers, cs);   
   cs = _el_network_server_new("irc.takas.lt", 6667);
   cn->servers = evas_list_append(cn->servers, cs);   
   cs = _el_network_server_new("irc.5ci.net", 6667);
   cn->servers = evas_list_append(cn->servers, cs);   
   cs = _el_network_server_new("irc.kis.lt", 6667);
   cn->servers = evas_list_append(cn->servers, cs);
   el_config_nets->networks = evas_list_append(el_config_nets->networks, cn);

   cn = EL_NEW(Config_Network, 1);
   cn->name = evas_stringshare_add("AmigaNet");
   cs = _el_network_server_new("irc.amiganet.org", 6667);
   cn->servers = evas_list_append(cn->servers, cs);
   cs = _el_network_server_new("us.amiganet.org", 6667);
   cn->servers = evas_list_append(cn->servers, cs);
   cs = _el_network_server_new("uk.amiganet.org", 6667);
   cn->servers = evas_list_append(cn->servers, cs);
   el_config_nets->networks = evas_list_append(el_config_nets->networks, cn);

   cn = EL_NEW(Config_Network, 1);
   cn->name = evas_stringshare_add("ARCNet");
   cs = _el_network_server_new("se1.arcnet.vapor.com", 6667);
   cn->servers = evas_list_append(cn->servers, cs);
   cs = _el_network_server_new("us1.arcnet.vapor.com", 6667);
   cn->servers = evas_list_append(cn->servers, cs);
   cs = _el_network_server_new("us2.arcnet.vapor.com", 6667);
   cn->servers = evas_list_append(cn->servers, cs);
   cs = _el_network_server_new("us3.arcnet.vapor.com", 6667);
   cn->servers = evas_list_append(cn->servers, cs);
   cs = _el_network_server_new("ca1.arcnet.vapor.com", 6667);
   cn->servers = evas_list_append(cn->servers, cs);
   cs = _el_network_server_new("de1.arcnet.vapor.com", 6667);
   cn->servers = evas_list_append(cn->servers, cs);
   cs = _el_network_server_new("de3.arcnet.vapor.com", 6667);
   cn->servers = evas_list_append(cn->servers, cs);
   cs = _el_network_server_new("ch1.arcnet.vapor.com", 6667);
   cn->servers = evas_list_append(cn->servers, cs);
   cs = _el_network_server_new("be1.arcnet.vapor.com", 6667);
   cn->servers = evas_list_append(cn->servers, cs);
   cs = _el_network_server_new("nl3.arcnet.vapor.com", 6667);
   cn->servers = evas_list_append(cn->servers, cs);
   cs = _el_network_server_new("uk1.arcnet.vapor.com", 6667);
   cn->servers = evas_list_append(cn->servers, cs);
   cs = _el_network_server_new("uk2.arcnet.vapor.com", 6667);
   cn->servers = evas_list_append(cn->servers, cs);
   cs = _el_network_server_new("fr1.arcnet.vapor.com", 6667);
   cn->servers = evas_list_append(cn->servers, cs);
   el_config_nets->networks = evas_list_append(el_config_nets->networks, cn);

   cn = EL_NEW(Config_Network, 1);
   cn->name = evas_stringshare_add("AstroLink");
   cs = _el_network_server_new("irc.astrolink.org", 6667);
   cn->servers = evas_list_append(cn->servers, cs);
   el_config_nets->networks = evas_list_append(el_config_nets->networks, cn);

   cn = EL_NEW(Config_Network, 1);
   cn->name = evas_stringshare_add("AustNet");
   cs = _el_network_server_new("au.austnet.org", 6667);
   cn->servers = evas_list_append(cn->servers, cs);
   cs = _el_network_server_new("us.austnet.org", 6667);
   cn->servers = evas_list_append(cn->servers, cs);
   cs = _el_network_server_new("ca.austnet.org", 6667);
   cn->servers = evas_list_append(cn->servers, cs);
   el_config_nets->networks = evas_list_append(el_config_nets->networks, cn);

   cn = EL_NEW(Config_Network, 1);
   cn->name = evas_stringshare_add("AzzurraNet");
   cs = _el_network_server_new("irc.azzurra.org", 6667);
   cn->servers = evas_list_append(cn->servers, cs);
   cs = _el_network_server_new("crypto.azzurra.org", 6667);
   cn->servers = evas_list_append(cn->servers, cs);
   el_config_nets->networks = evas_list_append(el_config_nets->networks, cn);
   
   el_network_save();
}

static Config_Network_Server *
_el_network_server_new(const char *addr, int port) 
{
   Config_Network_Server *cs;
   
   cs = EL_NEW(Config_Network_Server, 1);
   cs->address = evas_stringshare_add(addr);
   cs->port = port;
   return cs;
}
