#include <stdio.h>
#include <stdlib.h>
#include <etk/Etk.h>
#include "config.h"
#include "el.h"
#include "el_about_gui.h"

static void _close_cb(Etk_Object *obj, void *data);

ELAPI void *
el_about(void) 
{
   Etk_Widget *win, *vbox, *hbox;
   Etk_Widget *button, *label;
   Etk_Widget *about;
   Etk_Textblock *msg;
   Etk_Textblock_Iter *iter;
   char buf[4096];
   
   win = etk_dialog_new();
   etk_window_title_set(ETK_WINDOW(win), "About "PACKAGE_NAME);
   etk_window_wmclass_set(ETK_WINDOW(win), PACKAGE"_About", PACKAGE_NAME"_About");
   etk_container_border_width_set(ETK_CONTAINER(win), 3);
   etk_signal_connect("delete_event", ETK_OBJECT(win), ETK_CALLBACK(_close_cb), NULL);

   vbox = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(win), vbox);
   
   label = etk_label_new("<h1>"PACKAGE_NAME"</h1>");
   etk_label_alignment_set(ETK_LABEL(label), 0.5, 0.5);
   etk_box_append(ETK_BOX(vbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);

   snprintf(buf, sizeof(buf), 
	    "<b>%s is an Etk-based IRC client in development</b>\n\n"
	    "Christopher 'devilhorns' Michael\n\n%s\n", 
	    PACKAGE_NAME, PACKAGE_BUGREPORT);
   about = etk_text_view_new();
   etk_widget_size_request_set(about, 250, 150);
   etk_box_append(ETK_BOX(vbox), about, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   msg = etk_text_view_textblock_get(ETK_TEXT_VIEW(about));
   etk_textblock_object_cursor_visible_set(ETK_TEXT_VIEW(about)->textblock_object, ETK_FALSE);
   
   iter = etk_textblock_iter_new(msg);
   etk_textblock_iter_forward_end(iter);
   etk_textblock_insert_markup(msg, iter, buf, -1);

   etk_box_append(ETK_BOX(vbox), etk_hseparator_new(), ETK_BOX_END, ETK_BOX_FILL, 5);
   
   hbox = etk_hbox_new(ETK_FALSE, 3);
   etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_END, ETK_BOX_NONE, 0);

   button = etk_button_new_from_stock(ETK_STOCK_DIALOG_CLOSE);
   etk_signal_connect_swapped("clicked", ETK_OBJECT(button), ETK_CALLBACK(_close_cb), win);
   etk_box_append(ETK_BOX(hbox), button, ETK_BOX_END, ETK_BOX_NONE, 0);
   
   return win;
}

static void 
_close_cb(Etk_Object *obj, void *data) 
{
   etk_object_destroy(ETK_OBJECT(obj));
}
