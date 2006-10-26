#include <string.h>
#include <etk/Etk.h>
#include <E_Irc.h>
#include "config.h"
#include "el.h"
#include "el_gui.h"
#include "el_config_gui.h"
#include "el_about_gui.h"

static Etk_Bool _el_gui_shutdown (void *data);
static void     _quit_cb         (Etk_Object *obj, void *data);
static void     _config_cb       (Etk_Object *obj, void *data);
static void     _help_cb         (Etk_Object *obj, void *data);
static char    *_gui_get_markup  (char *data);

ELAPI Gui_Win *gui = NULL;

ELAPI void 
el_gui_init(void) 
{
   Etk_Widget *mbar, *vbox;
   Etk_Widget *hpane, *table;
   Etk_Widget *hdl, *entry;
   char buf[4096];

   gui = EL_NEW(Gui_Win, 1);
   if (!gui) return;
   
   snprintf(buf, sizeof(buf), "../../images/op.png");

   gui->win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(gui->win), PACKAGE_NAME" - "PACKAGE_VERSION);
   etk_window_wmclass_set(ETK_WINDOW(gui->win), PACKAGE, PACKAGE_NAME);
   etk_container_border_width_set(ETK_CONTAINER(gui->win), 3);
   etk_window_resize(ETK_WINDOW(gui->win), 450, 300);
   etk_signal_connect("delete_event", ETK_OBJECT(gui->win), ETK_CALLBACK(_el_gui_shutdown), NULL);

   vbox = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(gui->win), vbox);

   mbar = etk_menu_bar_new();
   etk_box_append(ETK_BOX(vbox), mbar, ETK_BOX_START, ETK_BOX_FILL, 0);
     {
	Etk_Widget *mi;
	Etk_Widget *sm;
	Etk_Widget *image;
	
	mi = etk_menu_item_new_with_label(_("File"));
	etk_menu_shell_append(ETK_MENU_SHELL(mbar), ETK_MENU_ITEM(mi));
	
	sm = etk_menu_new();
	etk_menu_item_submenu_set(ETK_MENU_ITEM(mi), ETK_MENU(sm));
	mi = etk_menu_item_image_new_from_stock(ETK_STOCK_DIALOG_CLOSE);
	etk_signal_connect("activated", ETK_OBJECT(mi), _quit_cb, NULL);
	etk_menu_shell_append(ETK_MENU_SHELL(sm), ETK_MENU_ITEM(mi));
	
	mi = etk_menu_item_new_with_label(_("Edit"));
	etk_menu_shell_append(ETK_MENU_SHELL(mbar), ETK_MENU_ITEM(mi));
	
	sm = etk_menu_new();
	etk_menu_item_submenu_set(ETK_MENU_ITEM(mi), ETK_MENU(sm));
	mi = etk_menu_item_image_new_with_label(_("Options"));
	etk_signal_connect("activated", ETK_OBJECT(mi), _config_cb, NULL);
	image = etk_image_new_from_stock(ETK_STOCK_PREFERENCES_SYSTEM, ETK_STOCK_SMALL);
	etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(mi), ETK_IMAGE(image));
	etk_menu_shell_append(ETK_MENU_SHELL(sm), ETK_MENU_ITEM(mi));
	
	mi = etk_menu_item_new_with_label(_("Help"));
	etk_menu_shell_append(ETK_MENU_SHELL(mbar), ETK_MENU_ITEM(mi));
	sm = etk_menu_new();
	etk_menu_item_submenu_set(ETK_MENU_ITEM(mi), ETK_MENU(sm));
	mi = etk_menu_item_image_new_with_label(_("About"));
	etk_signal_connect("activated", ETK_OBJECT(mi), _help_cb, NULL);
	image = etk_image_new_from_stock(ETK_STOCK_HELP_BROWSER, ETK_STOCK_SMALL);
	etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(mi), ETK_IMAGE(image));
	etk_menu_shell_append(ETK_MENU_SHELL(sm), ETK_MENU_ITEM(mi));
     }
   
   hpane = etk_hpaned_new();
   etk_box_append(ETK_BOX(vbox), hpane, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   table = etk_table_new(2, 2, ETK_FALSE);
   etk_box_append(ETK_BOX(vbox), table, ETK_BOX_START, ETK_BOX_FILL, 0);
   
   gui->tv_chat = etk_text_view_new();
   etk_widget_size_request_set(gui->tv_chat, 350, 100);
   etk_textblock_object_cursor_visible_set(ETK_TEXT_VIEW(gui->tv_chat)->textblock_object, ETK_FALSE);
   
   gui->note = etk_notebook_new();
   etk_notebook_tabs_visible_set(ETK_NOTEBOOK(gui->note), ETK_TRUE);
   etk_notebook_page_append(ETK_NOTEBOOK(gui->note), _("Channel"), gui->tv_chat);
   etk_table_attach(ETK_TABLE(table), gui->note, 0, 1, 0, 0, 0, 0, ETK_TABLE_EXPAND_FILL);

   Etk_Widget *icon;
   
   icon = etk_image_new_from_file(buf);
   
//   hdl = etk_label_new(_("Devilhorns"));
   hdl = etk_button_new();
//   etk_button_label_set(ETK_BUTTON(hdl), _("Devilhorns"));
//   etk_button_image_set(ETK_BUTTON(hdl), ETK_IMAGE(icon));
   etk_table_attach(ETK_TABLE(table), hdl, 0, 0, 1, 1, 3, 0, ETK_TABLE_HFILL);

   entry = etk_entry_new();
   etk_table_attach(ETK_TABLE(table), entry, 1, 1, 1, 1, 0, 3, ETK_TABLE_HFILL | ETK_TABLE_HEXPAND);
   
   etk_paned_child1_set(ETK_PANED(hpane), table, ETK_TRUE);

   gui->users = etk_tree_new();
   etk_widget_size_request_set(gui->users, 100, 100);
   etk_tree_mode_set(ETK_TREE(gui->users), ETK_TREE_MODE_LIST);
   etk_tree_headers_visible_set(ETK_TREE(gui->users), ETK_FALSE);
   etk_tree_multiple_select_set(ETK_TREE(gui->users), ETK_FALSE);
   etk_paned_child2_set(ETK_PANED(hpane), gui->users, ETK_TRUE);
     {
	Etk_Tree_Col *col;
	
	col = etk_tree_col_new(ETK_TREE(gui->users), NULL, 
	      etk_tree_model_icon_text_new(ETK_TREE(gui->users), 
					   ETK_TREE_FROM_FILE), 90);
	etk_tree_build(ETK_TREE(gui->users));

//	snprintf(buf, sizeof(buf), "%s/images/op.png", PACKAGE_DATA_DIR);
	etk_tree_freeze(ETK_TREE(gui->users));
//	etk_tree_append(ETK_TREE(gui->users), col, buf, _("Devilhorns"), NULL);
	etk_tree_thaw(ETK_TREE(gui->users));
     }
   
   etk_widget_show_all(gui->win);
}

ELAPI void
el_gui_data(void *data) 
{
   E_Irc_Client *client;
   Etk_Textblock *msg;
   Etk_Textblock_Iter *iter;
   
   if (!gui) return;
   
   client = data;
   if (!client) return;
   if (!client->data) return;

   etk_notebook_page_tab_label_set(ETK_NOTEBOOK(gui->note), 
				   etk_notebook_current_page_get(ETK_NOTEBOOK(gui->note)),
				   _("#e"));
   
   msg = etk_text_view_textblock_get(ETK_TEXT_VIEW(gui->tv_chat));
   if (!msg) return;
   iter = etk_textblock_iter_new(msg);
   etk_textblock_iter_forward_end(iter);
   etk_textblock_insert_markup(msg, iter, _gui_get_markup(client->data), -1);
}

static Etk_Bool
_el_gui_shutdown(void *data) 
{
   etk_main_quit();
   EL_FREE(gui);
   return 1;
}

static void 
_quit_cb(Etk_Object *obj, void *data) 
{
   etk_main_quit();
   EL_FREE(gui);
}

static void 
_config_cb(Etk_Object *obj, void *data) 
{
   Etk_Widget *win;
   
   win = el_configure();
   if (!win) return;
   
   etk_window_modal_for_window(ETK_WINDOW(win), ETK_WINDOW(gui->win));
   etk_window_center_on_window(ETK_WINDOW(win), ETK_WINDOW(gui->win));
   etk_widget_show_all(win);
}

static void 
_help_cb(Etk_Object *obj, void *data) 
{
   Etk_Widget *win;
   
   win = el_about();
   if (!win) return;
   
   etk_window_modal_for_window(ETK_WINDOW(win), ETK_WINDOW(gui->win));
   etk_window_center_on_window(ETK_WINDOW(win), ETK_WINDOW(gui->win));
   etk_widget_show_all(win);
}

static char *
_gui_get_markup(char *data) 
{
   char buf[4096];
   
   if (!data) return NULL;
   if (strstr(data, "NOTICE")) 
     snprintf(buf, sizeof(buf), "<b><font color=#a82f2f>%s</font></b>", data);
   return strdup(buf);
}
