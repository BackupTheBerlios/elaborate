#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Evas.h>
#include <etk/Etk.h>
#include "config.h"
#include "el.h"
#include "el_config.h"
#include "el_config_gui.h"

static void _config_gui_shutdown (Etk_Object *obj, void *data);
static void _apply_cb            (Etk_Object *obj, void *data);
static void _close_cb            (Etk_Object *obj, void *data);
static void _tree_load           (Etk_Widget *obj);
static void _add_cb              (Etk_Object *obj, void *data);
static void _edit_cb             (Etk_Object *obj, void *data);
static void _remove_cb           (Etk_Object *obj, void *data);

ELAPI Config_Gui *gui_cfg = NULL;

ELAPI void *
el_configure(void) 
{
   Etk_Widget *vbox, *hbox, *table;
   Etk_Widget *label, *button;
   Etk_Widget *frame, *nets;
   Etk_Widget *t2, *t3, *icon;
   
   gui_cfg = EL_NEW(Config_Gui, 1);
   if (!gui_cfg) return NULL;
   
   gui_cfg->win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(gui_cfg->win), PACKAGE_NAME" - Configuration");
   etk_window_wmclass_set(ETK_WINDOW(gui_cfg->win), PACKAGE"_Config", PACKAGE_NAME"_Configuration");
   etk_container_border_width_set(ETK_CONTAINER(gui_cfg->win), 3);
   etk_signal_connect("delete_event", ETK_OBJECT(gui_cfg->win), ETK_CALLBACK(_config_gui_shutdown), NULL);

   vbox = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(gui_cfg->win), vbox);
   
   table = etk_table_new(2, 3, ETK_FALSE);
   etk_box_append(ETK_BOX(vbox), table, ETK_BOX_START, ETK_BOX_FILL, 0);
   
   label = etk_label_new(_("Realname:"));
   etk_table_attach(ETK_TABLE(table), label, 0, 0, 0, 0, 3, 0, ETK_TABLE_HFILL);
   label = etk_label_new(_("Nickname:"));
   etk_table_attach(ETK_TABLE(table), label, 0, 0, 1, 1, 3, 0, ETK_TABLE_HFILL);
   label = etk_label_new(_("Password:"));
   etk_table_attach(ETK_TABLE(table), label, 0, 0, 2, 2, 3, 0, ETK_TABLE_HFILL);

   gui_cfg->real = etk_entry_new();
   etk_entry_text_set(ETK_ENTRY(gui_cfg->real), el_config->realname);
   etk_table_attach(ETK_TABLE(table), gui_cfg->real, 1, 1, 0, 0, 3, 0, ETK_TABLE_HFILL | ETK_TABLE_HEXPAND);

   gui_cfg->nick = etk_entry_new();
   etk_entry_text_set(ETK_ENTRY(gui_cfg->nick), el_config->nickname);
   etk_table_attach(ETK_TABLE(table), gui_cfg->nick, 1, 1, 1, 1, 3, 0, ETK_TABLE_HFILL | ETK_TABLE_HEXPAND);

   gui_cfg->pass = etk_entry_new();
   etk_entry_password_mode_set(ETK_ENTRY(gui_cfg->pass), ETK_TRUE);
   etk_entry_text_set(ETK_ENTRY(gui_cfg->pass), el_config->password);
   etk_table_attach(ETK_TABLE(table), gui_cfg->pass, 1, 1, 2, 2, 3, 0, ETK_TABLE_HFILL | ETK_TABLE_HEXPAND);

   frame = etk_frame_new(_("Available Networks"));
   etk_box_append(ETK_BOX(vbox), frame, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 5);

   t2 = etk_table_new(2, 4, ETK_FALSE);
   etk_container_add(ETK_CONTAINER(frame), t2);

   t3 = etk_table_new(1, 3, ETK_FALSE);
   
   nets = etk_tree_new();
   etk_widget_size_request_set(nets, 200, 150);
   etk_tree_mode_set(ETK_TREE(nets), ETK_TREE_MODE_LIST);
   etk_tree_headers_visible_set(ETK_TREE(nets), ETK_FALSE);
   etk_tree_multiple_select_set(ETK_TREE(nets), ETK_FALSE);
     {
	Etk_Tree_Col *col;
	
	col = etk_tree_col_new(ETK_TREE(nets), NULL, 
			       etk_tree_model_text_new(ETK_TREE(nets)), 90);
	etk_tree_build(ETK_TREE(nets));
	
	_tree_load(nets);
     }
   etk_table_attach(ETK_TABLE(t2), nets, 0, 0, 0, 4, 3, 0, ETK_TABLE_EXPAND_FILL);

   button = etk_button_new_from_stock(ETK_STOCK_LIST_ADD);
   etk_signal_connect_swapped("clicked", ETK_OBJECT(button), 
				ETK_CALLBACK(_add_cb), NULL);
   etk_table_attach(ETK_TABLE(t3), button, 0, 0, 0, 0, 0, 0, ETK_TABLE_HFILL);

   icon = etk_image_new_from_stock(ETK_STOCK_EDIT_CUT, ETK_STOCK_SMALL);
   
   button = etk_button_new();
   etk_button_label_set(ETK_BUTTON(button), _("Edit"));
   etk_button_image_set(ETK_BUTTON(button), ETK_IMAGE(icon));
   etk_signal_connect_swapped("clicked", ETK_OBJECT(button), 
				ETK_CALLBACK(_edit_cb), nets);
   etk_table_attach(ETK_TABLE(t3), button, 0, 0, 1, 1, 0, 0, ETK_TABLE_HFILL);
   
   button = etk_button_new_from_stock(ETK_STOCK_LIST_REMOVE);
   etk_signal_connect_swapped("clicked", ETK_OBJECT(button), 
				ETK_CALLBACK(_remove_cb), nets);
   etk_table_attach(ETK_TABLE(t3), button, 0, 0, 2, 2, 0, 0, ETK_TABLE_HFILL);
   
   etk_table_attach(ETK_TABLE(t2), t3, 1, 1, 0, 0, 0, 0, ETK_TABLE_NONE);

   etk_box_append(ETK_BOX(vbox), etk_hseparator_new(), ETK_BOX_END, ETK_BOX_FILL, 5);

   hbox = etk_hbox_new(ETK_FALSE, 3);
   etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_END, ETK_BOX_NONE, 0);

   button = etk_button_new_from_stock(ETK_STOCK_DIALOG_APPLY);
   etk_signal_connect_swapped("clicked", ETK_OBJECT(button), 
				ETK_CALLBACK(_apply_cb), NULL);
   etk_box_append(ETK_BOX(hbox), button, ETK_BOX_END, ETK_BOX_NONE, 0);

   button = etk_button_new_from_stock(ETK_STOCK_DIALOG_CLOSE);
   etk_signal_connect_swapped("clicked", ETK_OBJECT(button), 
				ETK_CALLBACK(_close_cb), gui_cfg->win);
   etk_box_append(ETK_BOX(hbox), button, ETK_BOX_END, ETK_BOX_NONE, 0);
   
   return gui_cfg->win;
}

static void
_config_gui_shutdown(Etk_Object *obj, void *data) 
{
//   el_network_shutdown();
   etk_object_destroy(ETK_OBJECT(obj));
   EL_FREE(gui_cfg);
}

static void 
_apply_cb(Etk_Object *obj, void *data) 
{
   const char *real;
   const char *nick;
   const char *pass;
   
   if (!gui_cfg) return;
   
   real = etk_entry_text_get(ETK_ENTRY(gui_cfg->real));
   nick = etk_entry_text_get(ETK_ENTRY(gui_cfg->nick));
   pass = etk_entry_text_get(ETK_ENTRY(gui_cfg->pass));

   if (strcmp(real, el_config->realname)) 
     {
	if (el_config->realname)
	  evas_stringshare_del(el_config->realname);
	el_config->realname = evas_stringshare_add(real);
     }
   
   if (strcmp(nick, el_config->nickname))
     {
	if (el_config->nickname)
	  evas_stringshare_del(el_config->nickname);
	el_config->nickname = evas_stringshare_add(nick);
     }
   
   if (strcmp(pass, el_config->password)) 
     {
	if (el_config->password)
	  evas_stringshare_del(el_config->password);
	el_config->password = evas_stringshare_add(pass);
     }
   
   if (!el_config_save())
     printf("Save Failed\n");
}

static void 
_close_cb(Etk_Object *obj, void *data) 
{
   el_network_shutdown();

   if (dlg_add)
     etk_object_destroy(ETK_OBJECT(dlg_add->win));
   EL_FREE(dlg_add);
   
   if (gui_net)
     etk_object_destroy(ETK_OBJECT(gui_net->win));
   EL_FREE(gui_net);
   
   etk_object_destroy(ETK_OBJECT(obj));
   EL_FREE(gui_cfg);
}

static void 
_tree_load(Etk_Widget *obj) 
{
   Evas_List *l;
   Etk_Tree_Col *col;
   char buf[256];
   
   if (!el_config_nets) 
     el_network_init();

   col = etk_tree_nth_col_get(ETK_TREE(obj), 0);
   if (!col) return;

   etk_tree_freeze(ETK_TREE(obj));
   etk_tree_clear(ETK_TREE(obj));

   for (l = el_config_nets->networks; l; l = l->next) 
     {
	Config_Network *cn;
	
	cn = l->data;
	if (!cn) continue;
	snprintf(buf, sizeof(buf), "%s", cn->name);
	etk_tree_append(ETK_TREE(obj), col, buf, NULL);
     }
   
   etk_tree_thaw(ETK_TREE(obj));
}

static void 
_add_cb(Etk_Object *obj, void *data) 
{
   
}

static void 
_edit_cb(Etk_Object *obj, void *data) 
{
   Gui_Net *gui_net;
   Etk_Tree_Row *r;
   Etk_Tree_Col *col;
   char *c;

   r = etk_tree_selected_row_get(ETK_TREE(obj));
   if (!r) return;
   col = etk_tree_nth_col_get(ETK_TREE(obj), 0);
   if (!col) return;
   
   etk_tree_row_fields_get(r, col, &c, NULL);

   gui_net = el_network(c);
   if (!gui_net) return;
   etk_window_modal_for_window(ETK_WINDOW(gui_net->win), ETK_WINDOW(gui_cfg->win));
   etk_window_center_on_window(ETK_WINDOW(gui_net->win), ETK_WINDOW(gui_cfg->win));
   etk_widget_show_all(gui_net->win);
}

static void 
_remove_cb(Etk_Object *obj, void *data) 
{
   Etk_Tree_Row *r;
   Etk_Tree_Col *col;
   char *c;

   r = etk_tree_selected_row_get(ETK_TREE(obj));
   if (!r) return;
   col = etk_tree_nth_col_get(ETK_TREE(obj), 0);
   if (!col) return;
   
   etk_tree_row_fields_get(r, col, &c, NULL);
}
