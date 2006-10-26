#ifndef _EL_CONFIG_GUI_H
#define _EL_CONFIG_GUI_H

typedef struct _Config_Gui Config_Gui;
struct _Config_Gui 
{
   Etk_Widget *win;
   Etk_Widget *real, *nick, *pass;
};

ELAPI void *el_configure(void);

#endif
