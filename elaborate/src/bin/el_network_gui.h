#ifndef _EL_NETWORK_GUI_H
#define _EL_NETWORK_GUI_H

typedef struct _Gui_Net Gui_Net;
typedef struct _Dlg_Add Dlg_Add;

struct _Gui_Net 
{
   Etk_Widget *win, *entry;
   Etk_Widget *servers;
   Config_Network *cfg_net;
};

struct _Dlg_Add 
{
   Etk_Widget *win, *entry;
   Etk_Widget *port;
};

ELAPI void *el_network(const char *name);

extern ELAPI Gui_Net *gui_net;
extern ELAPI Dlg_Add *dlg_add;

#endif
