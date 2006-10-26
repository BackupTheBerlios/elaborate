#ifndef _EL_NETWORK_H
#define _EL_NETWORK_H

typedef struct _Config_Networks Config_Networks;
typedef struct _Config_Network Config_Network;
typedef struct _Config_Network_Server Config_Network_Server;

struct _Config_Networks 
{
   Evas_List *networks;
};

struct _Config_Network 
{
   const char *name;
   Evas_List *servers;
};

struct _Config_Network_Server 
{
   const char *address;
   int port;
};

ELAPI int   el_network_init     (void);
ELAPI int   el_network_shutdown (void);
ELAPI int   el_network_save     (void);
ELAPI void *el_network_load     (void);
ELAPI void *el_network_find     (const char *name);

extern ELAPI Config_Networks *el_config_nets;

#endif
