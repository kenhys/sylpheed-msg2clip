/*
 * Msg2Clip -- 
 * Copyright (C) 2012 HAYASHI Kentaro
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <glib.h>
#include <gtk/gtk.h>

#include <windows.h>
#include <shlobj.h>
#include <tchar.h>

#include "sylmain.h"
#include "plugin.h"
#include "folder.h"
#include "procmsg.h"
#include "compose.h"
#include "alertpanel.h"
#include "foldersel.h"
#include "headerview.h"
#include "messageview.h"
#include "procheader.h"
#include "msg2clip.h"

#include "page_save.xpm"

static SylPluginInfo info = {
  N_(PLUGIN_NAME),
  "0.1.0",
  "HAYASHI Kentaro",
  N_(PLUGIN_DESC)
};

static Msg2ClipOption g_opt;

static void init_done_cb(GObject *obj, gpointer data);
static void app_exit_cb(GObject *obj, gpointer data);

static void exec_msg2clip_menu_cb(void);
static void copy_btn_clicked(GtkButton *button, gpointer data);

gulong app_exit_handler_id = 0;

static gchar* g_copyright = N_("Msg2Clip is distributed under GPL license.\n"
			       "\n"
			       "Copyright (C) 2012 HAYASHI Kentaro <kenhys@gmail.com>"
			       "\n"
			       "msg2clip contains following resource.\n"
			       "\n"
			       "page_save.xpm: converted from page_save.png.\n"
			       "Silk icon set 1.3 By Mark James\n"
			       "http://www.famfamfam.com/lab/icons/silk/\n"
			       "Creative Commons Attribution 2.5 License.\n");

void plugin_load(void)
{
  gpointer mainwin;

  syl_init_gettext(MSG2CLIP, "lib/locale");
  
  g_print("msg2clip plug-in loaded!\n");

  syl_plugin_add_menuitem("/Tools", NULL, NULL, NULL);
  syl_plugin_add_menuitem("/Tools", _("Msg2Clip [msg2clip]"), exec_msg2clip_menu_cb, NULL);

  mainwin = syl_plugin_main_window_get();
  g_print("mainwin: %p\n", mainwin);
  syl_plugin_main_window_popup(mainwin);

  g_signal_connect_after(syl_app_get(), "init-done", G_CALLBACK(init_done_cb),
                         NULL);
  app_exit_handler_id =
	g_signal_connect(syl_app_get(), "app-exit", G_CALLBACK(app_exit_cb),
                     NULL);

  syl_plugin_signal_connect("messageview-show",
                            G_CALLBACK(messageview_show_cb), NULL);
  
  g_print("msg2clip plug-in loading done\n");

}

void plugin_unload(void)
{
  g_print("msg2clip plug-in unloaded!\n");
  g_signal_handler_disconnect(syl_app_get(), app_exit_handler_id);
}

SylPluginInfo *plugin_info(void)
{
  return &info;
}

gint plugin_interface_version(void)
{
    return 0x0108;
    /* return SYL_PLUGIN_INTERFACE_VERSION; */
}

static void init_done_cb(GObject *obj, gpointer data)
{
#if 0
  syl_plugin_update_check_set_check_url("http://localhost/version_pro.txt?");
  syl_plugin_update_check_set_download_url("http://localhost/download.php?sno=123&ver=VER&os=win");
  syl_plugin_update_check_set_jump_url("http://localhost/index.html");
  syl_plugin_update_check_set_check_plugin_url("http://localhost/plugin_version.txt");
  syl_plugin_update_check_set_jump_plugin_url("http://localhost/plugin.html");
#endif
  g_print("test: %p: app init done\n", obj);
}

static void app_exit_cb(GObject *obj, gpointer data)
{
  g_print("test: %p: app will exit\n", obj);
}

static void activate_menu_cb(GtkMenuItem *menuitem, gpointer data)
{
  g_print("menu activated\n");
}


static void prefs_ok_cb(GtkWidget *widget, gpointer data)
{
  
  GList* folder_list = folder_get_list();
  
  gtk_widget_destroy(GTK_WIDGET(data));
}

static void prefs_cancel_cb(GtkWidget *widget, gpointer data)
{
  gtk_widget_destroy(GTK_WIDGET(data));
}

static void exec_msg2clip_menu_cb(void)
{
  /* show modal dialog */
  GtkWidget *window;
  GtkWidget *vbox;
  GtkWidget *confirm_area;
  GtkWidget *ok_btn;
  GtkWidget *cancel_btn;

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_container_set_border_width(GTK_CONTAINER(window), 8);
  gtk_window_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_modal(GTK_WINDOW(window), TRUE);
  gtk_window_set_policy(GTK_WINDOW(window), FALSE, TRUE, FALSE);
  gtk_window_set_default_size(GTK_WINDOW(window), 300, 100);
  gtk_widget_realize(window);

  vbox = gtk_vbox_new(FALSE, 6);
  gtk_widget_show(vbox);
  gtk_container_add(GTK_CONTAINER(window), vbox);


  /* notebook */ 
  GtkWidget *notebook = gtk_notebook_new();
  /* main tab */
  create_config_main_page(notebook, g_opt.rcfile);
  /* about, copyright tab */
  create_config_about_page(notebook, g_opt.rcfile);

  gtk_widget_show(notebook);
  gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 0);


  confirm_area = gtk_hbutton_box_new();
  gtk_button_box_set_layout(GTK_BUTTON_BOX(confirm_area), GTK_BUTTONBOX_END);
  gtk_box_set_spacing(GTK_BOX(confirm_area), 6);

  ok_btn = gtk_button_new_from_stock(GTK_STOCK_OK);
  GTK_WIDGET_SET_FLAGS(ok_btn, GTK_CAN_DEFAULT);
  gtk_box_pack_start(GTK_BOX(confirm_area), ok_btn, FALSE, FALSE, 0);
  gtk_widget_show(ok_btn);

  cancel_btn = gtk_button_new_from_stock(GTK_STOCK_CANCEL);
  GTK_WIDGET_SET_FLAGS(cancel_btn, GTK_CAN_DEFAULT);
  gtk_box_pack_start(GTK_BOX(confirm_area), cancel_btn, FALSE, FALSE, 0);
  gtk_widget_show(cancel_btn);

  gtk_widget_show(confirm_area);
	
  gtk_box_pack_end(GTK_BOX(vbox), confirm_area, FALSE, FALSE, 0);
  gtk_widget_grab_default(ok_btn);
  gtk_widget_show(vbox);

  gtk_window_set_title(GTK_WINDOW(window), _("Msg2Clip Settings"));

  g_signal_connect(G_OBJECT(ok_btn), "clicked",
                   G_CALLBACK(prefs_ok_cb), window);
  g_signal_connect(G_OBJECT(cancel_btn), "clicked",
                   G_CALLBACK(prefs_cancel_cb), window);
  gtk_widget_show_all(window);

}

static GtkWidget *create_config_main_page(GtkWidget *notebook, GKeyFile *pkey)
{
  debug_print("create_config_main_page\n");
  if (notebook == NULL){
    return NULL;
  }
  /* startup */
  if (pkey!=NULL){
  }
  GtkWidget *vbox = gtk_vbox_new(FALSE, 0);

  /**/
  GtkWidget *startup_align = gtk_alignment_new(0, 0, 1, 1);
  gtk_alignment_set_padding(GTK_ALIGNMENT(startup_align), ALIGN_TOP, ALIGN_BOTTOM, ALIGN_LEFT, ALIGN_RIGHT);

  GtkWidget *startup_frm = gtk_frame_new(_("Startup Option"));
  GtkWidget *startup_frm_align = gtk_alignment_new(0, 0, 1, 1);
  gtk_alignment_set_padding(GTK_ALIGNMENT(startup_frm_align), ALIGN_TOP, ALIGN_BOTTOM, ALIGN_LEFT, ALIGN_RIGHT);


  g_opt.startup = gtk_check_button_new_with_label(_("Enable plugin on startup."));
  gtk_container_add(GTK_CONTAINER(startup_frm_align), g_opt.startup);
  gtk_container_add(GTK_CONTAINER(startup_frm), startup_frm_align);
  gtk_container_add(GTK_CONTAINER(startup_align), startup_frm);

  gtk_widget_show(g_opt.startup);

  /**/
  gtk_box_pack_start(GTK_BOX(vbox), startup_align, FALSE, FALSE, 0);

  GtkWidget *general_lbl = gtk_label_new(_("General"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, general_lbl);
  gtk_widget_show_all(notebook);
  return NULL;

}

/* about, copyright tab */
static GtkWidget *create_config_about_page(GtkWidget *notebook, GKeyFile *pkey)
{
  debug_print("create_config_about_page\n");
  if (notebook == NULL){
    return NULL;
  }
  GtkWidget *hbox = gtk_hbox_new(TRUE, 6);
  GtkWidget *vbox = gtk_vbox_new(FALSE, 6);

  GtkWidget *lbl = gtk_label_new(_("Msg2Clip"));
  GtkWidget *desc = gtk_label_new(PLUGIN_DESC);

  /* copyright */
  GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);

  gtk_box_pack_start(GTK_BOX(vbox), lbl, FALSE, TRUE, 6);
  gtk_box_pack_start(GTK_BOX(vbox), desc, FALSE, TRUE, 6);
  gtk_box_pack_start(GTK_BOX(vbox), scrolled, TRUE, TRUE, 6);
  gtk_box_pack_start(GTK_BOX(hbox), vbox, TRUE, TRUE, 6);

  GtkTextBuffer *tbuffer = gtk_text_buffer_new(NULL);
  gtk_text_buffer_set_text(tbuffer, _(g_copyright), strlen(g_copyright));
  GtkWidget *tview = gtk_text_view_new_with_buffer(tbuffer);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(tview), FALSE);
  gtk_container_add(GTK_CONTAINER(scrolled), tview);
    
  gtk_box_pack_start(GTK_BOX(vbox), scrolled, TRUE, TRUE, 6);
    
  /**/
  GtkWidget *general_lbl = gtk_label_new(_("About"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), hbox, general_lbl);
  gtk_widget_show_all(notebook);
  return NULL;
}


static void messageview_show_cb(GObject *obj, gpointer msgview,
				MsgInfo *msginfo, gboolean all_headers)
{
#if DEBUG
  g_print("[DEBUG] test: %p: messageview_show (%p), all_headers: %d: %s\n",
	  obj, msgview, all_headers,
	  msginfo && msginfo->subject ? msginfo->subject : "");
#endif
  
  if (!msgview) {
    g_print("[DEBUG] msgview is NULL\n");
    return;
  }

  MessageView *messageview = (MessageView*)msgview;
  if (!messageview) {
    g_print("[DEBUG] messageview is NULL\n");
    return;
  }

  HeaderView *headerview = messageview->headerview;
  if (!headerview) {
    g_print("[DEBUG] headerview is NULL\n");
    return;
  }
  
  GtkWidget *hbox = headerview->hbox;
  if (!hbox) {
    g_print("[DEBUG] hbox is NULL\n");
    return;
  }

  GList* wl = gtk_container_get_children(GTK_CONTAINER(hbox));

  gint i=g_list_length(wl)-1;
  gboolean bfound = FALSE;

  /* search recently added GtkImage */
  while (i >= 0) {
    gpointer gdata = g_list_nth_data(wl, i);
    if (GTK_IS_BUTTON(gdata) && gdata != headerview->image) {
      /* remove from hbox */
      g_print("[DEBUG] GTK_IS_BUTTON %p\n", gdata);
#if DEBUG
      g_print("[DEBUG] remove button: %p\n", gicon);
#endif
      gtk_container_remove(GTK_CONTAINER(hbox), GTK_BUTTON(gdata));
    }
    i--;
  }

  gchar *msg_path = procmsg_get_message_file_path(msginfo);
                    

  debug_print("[DEBUG] msg_path:%s\n", msg_path);


  if (bfound != TRUE){
    GtkWidget *copy_btn = gtk_button_new_from_stock(GTK_STOCK_FILE);
    gtk_box_pack_end(GTK_BOX(hbox), copy_btn, FALSE, FALSE, 0);

    GdkPixbuf* pbuf = gdk_pixbuf_new_from_xpm_data((const char**)page_save);
    GtkWidget* image = gtk_image_new_from_pixbuf(pbuf);
    
    gtk_button_set_image(GTK_BUTTON(copy_btn), image);
    gtk_button_set_label(GTK_BUTTON(copy_btn), "");

    g_signal_connect(G_OBJECT(copy_btn), "clicked",
		     G_CALLBACK(copy_btn_clicked), msginfo);
    
    gtk_widget_show(image);
    gtk_widget_show_all(copy_btn);

    debug_print("[DEBUG] copy mail to clipboard icon: %p\n", copy_btn);

  }

}

static void copy_btn_clicked(GtkButton *button, gpointer data)
{
  
  if (data != NULL) {
    MsgInfo* msginfo = (MsgInfo*)data;
    gchar *msg_path = procmsg_get_message_file_path(msginfo);
    
    debug_print("[DEBUG] msg_path:%s\n", msg_path);

    LPDROPFILES lpDropFile;

    int mblen = MultiByteToWideChar(CP_UTF8, 0, msg_path, -1, NULL,0);
    WCHAR* pszUnicode = malloc( sizeof(WCHAR)*(mblen+1));
    memset(pszUnicode, sizeof(WCHAR), (mblen+1));

    MultiByteToWideChar(CP_UTF8, 0, msg_path, strlen(msg_path)+1, pszUnicode, mblen);
  

    HDROP hDrop = (HDROP)GlobalAlloc(GHND,sizeof(DROPFILES)+2*mblen+2);
    if (hDrop==NULL) {
      debug_print ("[DEBUG] failed to allocate Global memory.\n");
      return;
    }
  
    debug_print ("[DEBUG] lpDropFile[0] :%p\n", &lpDropFile[0]);
    debug_print ("[DEBUG] lpDropFile[1] :%p\n", &lpDropFile[1]);
    debug_print ("[DEBUG] sizeof DROPFILES :%d\n", sizeof(DROPFILES));
    debug_print ("[DEBUG] wcslen :%d\n", wcslen(pszUnicode));


    lpDropFile = (LPDROPFILES)GlobalLock(hDrop);
    lpDropFile->pFiles = sizeof(DROPFILES);
    lpDropFile->pt.x = 0;
    lpDropFile->pt.y = 0;
    lpDropFile->fNC = FALSE;
    lpDropFile->fWide = TRUE;

    char *buf;
    buf = (char *)(&lpDropFile[1]);
    wcscpy((WCHAR*)buf,pszUnicode);
    buf += 2*wcslen(pszUnicode) + 1;
    *buf++ = 0;
    *buf = 0;

    GlobalUnlock(hDrop);
 
    if (OpenClipboard(0) == 0) {
      debug_print ("[DEBUG] failed to open clipboard.\n");
      GlobalFree(hDrop);
      return;
    }
    EmptyClipboard();
    SetClipboardData(CF_HDROP, hDrop);
    CloseClipboard();

  } 
}


