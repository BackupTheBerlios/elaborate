#ifndef _EL_GUI_H
#define _EL_GUI_H

typedef struct _Gui_Win Gui_Win;
struct _Gui_Win 
{
   Etk_Widget *tv_chat, *users;
   Etk_Widget *win;
   Etk_Widget *note;
};

ELAPI void el_gui_init (void);
ELAPI void el_gui_data (void *data);

#endif
