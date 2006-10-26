#include <stdio.h>
#include <stdlib.h>
#include <etk/Etk.h>
#include "config.h"
#include "el.h"
#include "el_network.h"
#include "el_network_gui.h"

static void _add_cb       (Etk_Object *obj, void *data);
static void _add_resp_cb  (Etk_Object *obj, int response_id, void *data);
static void _dlg_del_cb   (Etk_Object *obj, void *data);
static void _apply_cb     (Etk_Object *obj, void *data);
static void _close_cb     (Etk_Object *obj, void *data);
static void _servs_load   (Etk_Widget *obj, const char *name);
static void _serv_sel_cb  (Etk_Object *obj, Etk_Tree_Row *row, Etk_Event_Mouse_Up *event, void *data);

ELAPI Gui_Net *gui_net = NULL;
ELAPI Dlg_Add *dlg_add = NULL;

ELAPI void *
el_network(const char *name) 
{
   Etk_Widget *vbox;
   Etk_Widget *button, *hbox;
   Etk_Widget *label;
   Etk_Widget *table, *frame;
   Etk_Widget *t2, *t3;
   
   gui_net = EL_NEW(Gui_Net, 1);
   if (!gui_net) return NULL;
   if (name) 
     gui_net->cfg_net = el_network_find(name);

   gui_net->win = etk_dialog_new();
   etk_window_title_set(ETK_WINDOW(gui_net->win), PACKAGE_NAME" - Network Configuration");
   etk_window_wmclass_set(ETK_WINDOW(gui_net->win), PACKAGE"_Net_Config", PACKAGE_NAME"_Net_Config");
   etk_container_border_width_set(ETK_CONTAINER(gui_net->win), 3);
   etk_signal_connect("delete_event", ETK_OBJECT(gui_net->win), ETK_CALLBACK(_close_cb), NULL);

   vbox = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(gui_net->win), vbox);

   table = etk_table_new(2, 1, ETK_FALSE);
   etk_box_append(ETK_BOX(vbox), table, ETK_BOX_START, ETK_BOX_FILL, 0);
   label = etk_label_new(_("Name:"));
   etk_table_attach(ETK_TABLE(table), label, 0, 0, 0, 0, 3, 0, ETK_TABLE_HFILL);
   gui_net->entry = etk_entry_new();
   if (name)
     etk_entry_text_set(ETK_ENTRY(gui_net->entry), name);
   etk_table_attach(ETK_TABLE(table), gui_net->entry, 1, 1, 0, 0, 3, 0, ETK_TABLE_HFILL | ETK_TABLE_HEXPAND);
   
   frame = etk_frame_new(_("Available Servers"));
   etk_box_append(ETK_BOX(vbox), frame, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 5);
   
   t2 = etk_table_new(2, 4, ETK_FALSE);
   etk_container_add(ETK_CONTAINER(frame), t2);
   
   t3 = etk_table_new(1, 2, ETK_FALSE);
   
   gui_net->servers = etk_tree_new();
   etk_widget_size_request_set(gui_net->servers, 175, 150);
   etk_tree_mode_set(ETK_TREE(gui_net->servers), ETK_TREE_MODE_LIST);
   etk_tree_headers_visible_set(ETK_TREE(gui_net->servers), ETK_FALSE);
   etk_tree_multiple_select_set(ETK_TREE(gui_net->servers), ETK_FALSE);
   etk_signal_connect("row_clicked", ETK_OBJECT(gui_net->servers), ETK_CALLBACK(_serv_sel_cb), NULL);
     {
	Etk_Tree_Col *col;
	
	col = etk_tree_col_new(ETK_TREE(gui_net->servers), NULL, 
			       etk_tree_model_text_new(ETK_TREE(gui_net->servers)), 90);
	etk_tree_build(ETK_TREE(gui_net->servers));

	if (name)
	  _servs_load(gui_net->servers, name);
     }
   etk_table_attach(ETK_TABLE(t2), gui_net->servers, 0, 0, 0, 4, 3, 0, ETK_TABLE_EXPAND_FILL);

   button = etk_button_new_from_stock(ETK_STOCK_LIST_ADD);
   etk_signal_connect("clicked", ETK_OBJECT(button), ETK_CALLBACK(_add_cb), NULL);
   etk_table_attach(ETK_TABLE(t3), button, 0, 0, 0, 0, 0, 0, ETK_TABLE_HFILL);

   button = etk_button_new_from_stock(ETK_STOCK_LIST_REMOVE);
   etk_table_attach(ETK_TABLE(t3), button, 0, 0, 1, 1, 0, 0, ETK_TABLE_HFILL);
   
   etk_table_attach(ETK_TABLE(t2), t3, 1, 1, 0, 0, 0, 0, ETK_TABLE_NONE);

   table = etk_table_new(2, 1, ETK_FALSE);
   etk_box_append(ETK_BOX(vbox), table, ETK_BOX_START, ETK_BOX_FILL, 0);
   
   label = etk_label_new(_("Port:"));
   etk_table_attach(ETK_TABLE(table), label, 0, 0, 0, 0, 3, 0, ETK_TABLE_HFILL);
   gui_net->port = etk_entry_new();
   etk_table_attach(ETK_TABLE(table), gui_net->port, 1, 1, 0, 0, 3, 0, ETK_TABLE_HFILL | ETK_TABLE_HEXPAND);
   
   etk_box_append(ETK_BOX(vbox), etk_hseparator_new(), ETK_BOX_END, ETK_BOX_FILL, 5);
   
   hbox = etk_hbox_new(ETK_FALSE, 3);
   etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_END, ETK_BOX_NONE, 0);

   button = etk_button_new_from_stock(ETK_STOCK_DIALOG_APPLY);
   etk_signal_connect("clicked", ETK_OBJECT(button), ETK_CALLBACK(_apply_cb), NULL);
   etk_box_append(ETK_BOX(hbox), button, ETK_BOX_END, ETK_BOX_NONE, 0);
   
   button = etk_button_new_from_stock(ETK_STOCK_DIALOG_CLOSE);
   etk_signal_connect_swapped("clicked", ETK_OBJECT(button), ETK_CALLBACK(_close_cb), gui_net->win);
   etk_box_append(ETK_BOX(hbox), button, ETK_BOX_END, ETK_BOX_NONE, 0);

   return gui_net;
}

static void 
_add_cb(Etk_Object *obj, void *data) 
{
   Etk_Widget *label;
   Etk_Widget *button;

   dlg_add = EL_NEW(Dlg_Add, 1);
   if (!dlg_add) return;
   
   dlg_add->win = etk_dialog_new();
   etk_widget_size_request_set(dlg_add->win, 250, 100);
   etk_signal_connect("delete_event", ETK_OBJECT(dlg_add->win), ETK_CALLBACK(_dlg_del_cb), NULL);
   etk_container_border_width_set(ETK_CONTAINER(dlg_add->win), 3);
   etk_window_title_set(ETK_WINDOW(dlg_add->win), PACKAGE_NAME" - Add Server");
   etk_dialog_has_separator_set(ETK_DIALOG(dlg_add->win), ETK_TRUE);

   label = etk_label_new("Address:");
   etk_dialog_pack_in_main_area(ETK_DIALOG(dlg_add->win), label, ETK_FALSE, ETK_TRUE, 3, ETK_FALSE);
   dlg_add->entry = etk_entry_new();
   etk_dialog_pack_in_main_area(ETK_DIALOG(dlg_add->win), dlg_add->entry, ETK_FALSE, ETK_TRUE, 3, ETK_FALSE);
   
   button = etk_dialog_button_add_from_stock(ETK_DIALOG(dlg_add->win), ETK_STOCK_DIALOG_OK, ETK_RESPONSE_OK);
   button = etk_dialog_button_add_from_stock(ETK_DIALOG(dlg_add->win), ETK_STOCK_DIALOG_CANCEL, ETK_RESPONSE_CANCEL);

   etk_signal_connect("response", ETK_OBJECT(dlg_add->win), ETK_CALLBACK(_add_resp_cb), dlg_add);

   etk_window_modal_for_window(ETK_WINDOW(dlg_add->win), ETK_WINDOW(gui_net->win));
   etk_window_center_on_window(ETK_WINDOW(dlg_add->win), ETK_WINDOW(gui_net->win));
   etk_widget_show_all(dlg_add->win);
}

static void 
_add_resp_cb(Etk_Object *obj, int response_id, void *data) 
{
   Etk_Tree_Row *row;
   Etk_Tree_Col *col;
   const char *n;

   col = etk_tree_nth_col_get(ETK_TREE(gui_net->servers), 0);
   n = etk_entry_text_get(ETK_ENTRY(dlg_add->entry));
   switch (response_id) 
     {
      case ETK_RESPONSE_OK:
	if (!n) break;
	etk_tree_freeze(ETK_TREE(gui_net->servers));
	row = etk_tree_append(ETK_TREE(gui_net->servers), col, n, NULL);
	etk_tree_row_select(row);
	etk_tree_row_scroll_to(row, ETK_TRUE);
	etk_tree_thaw(ETK_TREE(gui_net->servers));
	break;
      default:
	break;
     }
   etk_object_destroy(ETK_OBJECT(obj));
   EL_FREE(dlg_add);
}

static void 
_dlg_del_cb(Etk_Object *obj, void *data) 
{
   etk_object_destroy(ETK_OBJECT(obj));
   EL_FREE(dlg_add);
}

static void 
_apply_cb(Etk_Object *obj, void *data) 
{
   Evas_List *l;
   Etk_Tree_Row *r;
   Etk_Tree_Col *c;
   Config_Network_Server *cs;
   const char *name;
   int i, num, f;
   char *n;
   
   if (!gui_net->cfg_net) return;
   
   name = etk_entry_text_get(ETK_ENTRY(gui_net->entry));
   if (strcmp(name, gui_net->cfg_net->name))
     {
	if (gui_net->cfg_net->name)
	  evas_stringshare_del(gui_net->cfg_net->name);
	gui_net->cfg_net->name = evas_stringshare_add(name);
     }

   num = etk_tree_num_rows_get(ETK_TREE(gui_net->servers));
   c = etk_tree_nth_col_get(ETK_TREE(gui_net->servers), 0);
   r = etk_tree_first_row_get(ETK_TREE(gui_net->servers));
   for (i = 0; i < num; i++) 
     {
	etk_tree_row_fields_get(r, c, &n, NULL);
	r = etk_tree_next_row_get(r, ETK_FALSE, ETK_FALSE);
	if (!n) continue;

	f = 0;
	for (l = gui_net->cfg_net->servers; l; l = l->next) 
	  {
	     cs = l->data;
	     if (!cs) continue;
	     if (strcmp(cs->address, n)) continue;
	     f = 1;
	     break;
	  }
	if (!f) 
	  {
	     cs = EL_NEW(Config_Network_Server, 1);
	     cs->address = evas_stringshare_add(n);
	     cs->port = 6667;
	     gui_net->cfg_net->servers = 
	       evas_list_append(gui_net->cfg_net->servers, cs);
	  }
     }
   
   if (!el_network_save())
     printf("Network save failed\n");
}

static void 
_close_cb(Etk_Object *obj, void *data) 
{
   if (dlg_add)
     etk_object_destroy(ETK_OBJECT(dlg_add->win));
   EL_FREE(dlg_add);
   
   etk_object_destroy(ETK_OBJECT(obj));
   EL_FREE(gui_net);
}

static void 
_servs_load(Etk_Widget *obj, const char *name) 
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
	Evas_List *ll;
	
	cn = l->data;
	if (!cn) continue;
	if (strcmp(cn->name, name)) continue;
	for (ll = cn->servers; ll; ll = ll->next) 
	  {
	     Config_Network_Server *cs;

	     cs = ll->data;
	     if (!cs) continue;
	     snprintf(buf, sizeof(buf), "%s", cs->address);
	     etk_tree_append(ETK_TREE(obj), col, buf, NULL);
	  }
	break;
     }
   etk_tree_thaw(ETK_TREE(obj));
}

static void 
_serv_sel_cb(Etk_Object *obj, Etk_Tree_Row *row, Etk_Event_Mouse_Up *event, void *data) 
{
   Etk_Tree_Col *col;
   Evas_List *l;
   char *n;
   
   if (!gui_net->cfg_net) return;
   col = etk_tree_nth_col_get(ETK_TREE(obj), 0);
   if (!col) return;
   etk_tree_row_fields_get(row, col, &n, NULL);
   for (l = gui_net->cfg_net->servers; l; l = l->next) 
     {
	Config_Network_Server *cs;
	char buf[256];
	
	cs = l->data;
	if (!cs) continue;
	if (strcmp(cs->address, n)) continue;
	snprintf(buf, sizeof(buf), "%i", cs->port);
	etk_entry_text_set(ETK_ENTRY(gui_net->port), buf);
	break;
     }
}
