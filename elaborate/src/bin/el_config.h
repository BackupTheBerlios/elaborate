#ifndef _EL_CONFIG_H
#define _EL_CONFIG_H

#define EL_CONFIG_DD_NEW(str, typ) \
   el_config_descriptor_new(str, sizeof(typ))
#define EL_CONFIG_DD_FREE(eed) if (eed) { eet_data_descriptor_free((eed)); (eed) = NULL; }
#define EL_CONFIG_VAL(edd, type, member, dtype) EET_DATA_DESCRIPTOR_ADD_BASIC(edd, type, #member, member, dtype)
#define EL_CONFIG_SUB(edd, type, member, eddtype) EET_DATA_DESCRIPTOR_ADD_SUB(edd, type, #member, member, eddtype)
#define EL_CONFIG_LIST(edd, type, member, eddtype) EET_DATA_DESCRIPTOR_ADD_LIST(edd, type, #member, member, eddtype)

typedef struct _Config Config;
typedef struct _Config_Server Config_Server;
typedef struct _Config_Channel Config_Channel;

typedef Eet_Data_Descriptor EL_Config_DD;

struct _Config 
{
   const char *version;
   const char *realname;
   const char *nickname;
   const char *password;
   Config_Server *server;
};

struct _Config_Server 
{
   const char *address;
   int port;
   Evas_List *channels;
};

struct _Config_Channel 
{
   const char *name;
};

ELAPI int   el_config_init     (void);
ELAPI int   el_config_shutdown (void);
ELAPI int   el_config_save     (void);
ELAPI void *el_config_load     (void);

ELAPI EL_Config_DD *el_config_descriptor_new(const char *name, int size);

extern ELAPI Config *el_config;

#endif
