#include <stdio.h>
#include <stdlib.h>
#include <Eet.h>
#include <Ecore.h>
#include <Evas.h>
#include "config.h"
#include "el.h"
#include "el_config.h"

ELAPI Config *el_config = NULL;

static EL_Config_DD *_el_config_edd = NULL;
static EL_Config_DD *_el_config_server_edd = NULL;
static EL_Config_DD *_el_config_channel_edd = NULL;

ELAPI int
el_config_init(void) 
{
   _el_config_channel_edd = EL_CONFIG_DD_NEW("EL_Config_Channel", Config_Channel);
   #undef T
   #undef D
   #define T Config_Channel
   #define D _el_config_channel_edd
   EL_CONFIG_VAL(D, T, name, EET_T_STRING);
   
   _el_config_server_edd = EL_CONFIG_DD_NEW("EL_Config_Server", Config_Server);
   #undef T
   #undef D
   #define T Config_Server
   #define D _el_config_server_edd
   EL_CONFIG_VAL(D, T, address, EET_T_STRING);
   EL_CONFIG_VAL(D, T, port, EET_T_INT);
   EL_CONFIG_LIST(D, T, channels, _el_config_channel_edd);
   
   _el_config_edd = EL_CONFIG_DD_NEW("EL_Config", Config);
   #undef T
   #undef D
   #define T Config
   #define D _el_config_edd
   EL_CONFIG_VAL(D, T, version, EET_T_STRING);
   EL_CONFIG_VAL(D, T, realname, EET_T_STRING);
   EL_CONFIG_VAL(D, T, nickname, EET_T_STRING);
   EL_CONFIG_VAL(D, T, password, EET_T_STRING);
   EL_CONFIG_SUB(D, T, server, _el_config_channel_edd);
   
   el_config = el_config_load();
   if (!el_config) 
     {
	el_config = EL_NEW(Config, 1);
	el_config->version = evas_stringshare_add(PACKAGE_VERSION);
	el_config->realname = evas_stringshare_add("changeme");
	el_config->nickname = evas_stringshare_add("changeme");
	el_config->password = evas_stringshare_add("changeme");
	el_config_save();
     }
   
   return 1;
}

ELAPI int
el_config_shutdown(void) 
{
   EL_FREE(el_config);
   EL_CONFIG_DD_FREE(_el_config_edd);
   EL_CONFIG_DD_FREE(_el_config_server_edd);
   EL_CONFIG_DD_FREE(_el_config_channel_edd);
   return 1;
}

ELAPI EL_Config_DD *
el_config_descriptor_new(const char *name, int size) 
{
   Eet_Data_Descriptor_Class eddc;
   
   eddc.version = EET_DATA_DESCRIPTOR_CLASS_VERSION;
   eddc.func.mem_alloc = NULL;
   eddc.func.mem_free = NULL;
   eddc.func.str_alloc = (char *(*)(const char *)) evas_stringshare_add;
   eddc.func.str_free = (void (*)(const char *)) evas_stringshare_del;
   eddc.func.list_next = (void *(*)(void *)) evas_list_next;
   eddc.func.list_append = (void *(*)(void *l, void *d)) evas_list_append;
   eddc.func.list_data = (void *(*)(void *)) evas_list_data;
   eddc.func.list_free = (void *(*)(void *)) evas_list_free;
   eddc.func.hash_foreach = 
      (void  (*) (void *, int (*) (void *, const char *, void *, void *), void *)) 
      evas_hash_foreach;
   eddc.func.hash_add = (void *(*) (void *, const char *, void *)) evas_hash_add;
   eddc.func.hash_free = (void  (*) (void *)) evas_hash_free;
   eddc.name = name;
   eddc.size = size;
   return (EL_Config_DD *)eet_data_descriptor2_new(&eddc);   
}

ELAPI int 
el_config_save(void) 
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
   snprintf(buf, sizeof(buf), "%s/.e/%s/%s.cfg", homedir, PACKAGE, PACKAGE);
   ef = eet_open(buf, EET_FILE_MODE_WRITE);
   if (ef) 
     {
	ok = eet_data_write(ef, _el_config_edd, "config", el_config, 1);
	eet_close(ef);
     }
   return ok;
}

ELAPI void *
el_config_load(void) 
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
   snprintf(buf, sizeof(buf), "%s/.e/%s/%s.cfg", homedir, PACKAGE, PACKAGE);
   ef = eet_open(buf, EET_FILE_MODE_READ);
   if (ef) 
     {
	data = eet_data_read(ef, _el_config_edd, "config");
	eet_close(ef);
     }
   return data;
}
