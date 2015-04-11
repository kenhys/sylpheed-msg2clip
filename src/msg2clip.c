/*
 * Msg2Clip -- 
 * Copyright (C) 2012 HAYASHI Kentaro
 */

#include "config.h"

#include <glib.h>
#include <gtk/gtk.h>
#include <glib/gi18n-lib.h>

#if defined(G_OS_WIN32)
#include <windows.h>
#include <shlobj.h>
#include <tchar.h>
#endif

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
#include "sylplugin_factory.h"
#include "copying.h"
#include "msg2clip.h"

#include "../res/page_save.xpm"

#define PLUGIN_NAME N_("Msg2Clip Plug-in")
#define PLUGIN_DESC N_("Copy message to clipboard")

static SylPluginInfo info = {
  N_(PLUGIN_NAME),
  VERSION,
  "HAYASHI Kentaro",
  N_(PLUGIN_DESC)
};

Msg2ClipOption g_opt;

static void init_done_cb(GObject *obj, gpointer data);
static void app_exit_cb(GObject *obj, gpointer data);

static void exec_msg2clip_menu_cb(void);
static void copy_btn_clicked(GtkButton *button, gpointer data);

gulong app_exit_handler_id = 0;

extern GtkWidget *create_config_main_page(GtkWidget *notebook, GKeyFile *pkey);
extern GtkWidget *create_config_about_page(GtkWidget *notebook, GKeyFile *pkey);


void plugin_load(void)
{
  gpointer mainwin;

  SYLPF_START_FUNC;

  syl_init_gettext(MSG2CLIP, "lib/locale");
  
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
  
  SYLPF_END_FUNC;
}

void plugin_unload(void)
{
  SYLPF_START_FUNC;

  g_signal_handler_disconnect(syl_app_get(), app_exit_handler_id);

  SYLPF_END_FUNC;
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

static void prefs_ok_cb(GtkWidget *widget, gpointer data)
{
  
  
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
  GtkWidget *notebook;
  
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
  notebook = gtk_notebook_new();
  /* main tab */
  create_config_main_page(notebook, g_opt.rcfile);
  /* about, copyright tab */
  sylpf_append_config_about_page(notebook,
                                 g_opt.rcfile,
                                 _("About"),
                                 _(PLUGIN_NAME),
                                 _(PLUGIN_DESC),
                                 _(copyright));

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



static void messageview_show_cb(GObject *obj, gpointer msgview,
				MsgInfo *msginfo, gboolean all_headers)
{
  MessageView *messageview;
  HeaderView *headerview;
  GtkWidget *hbox;
  gchar *msg_path;
  GtkWidget *copy_btn;
  GdkPixbuf* pbuf;
  GtkWidget* image;
  GtkTooltips *tip;
  GList* wl;
  gint i;
  gboolean bfound = FALSE;
  gpointer gdata;

  
#if DEBUG
  g_print("[DEBUG] test: %p: messageview_show (%p), all_headers: %d: %s\n",
	  obj, msgview, all_headers,
	  msginfo && msginfo->subject ? msginfo->subject : "");
#endif
  
  if (!msgview) {
    g_print("[DEBUG] msgview is NULL\n");
    return;
  }

  messageview = (MessageView*)msgview;
  if (!messageview) {
    g_print("[DEBUG] messageview is NULL\n");
    return;
  }

  headerview = messageview->headerview;
  if (!headerview) {
    g_print("[DEBUG] headerview is NULL\n");
    return;
  }
  
  hbox = headerview->hbox;
  if (!hbox) {
    g_print("[DEBUG] hbox is NULL\n");
    return;
  }

  wl = gtk_container_get_children(GTK_CONTAINER(hbox));

  i = g_list_length(wl)-1;

  /* search recently added GtkImage */
  while (i >= 0) {
    gdata = g_list_nth_data(wl, i);
    if (GTK_IS_BUTTON(gdata) && gdata != headerview->image) {
      /* remove from hbox */
      g_print("[DEBUG] GTK_IS_BUTTON %p\n", gdata);
#if DEBUG
      g_print("[DEBUG] remove button: %p\n", gicon);
#endif
      gtk_container_remove(GTK_CONTAINER(hbox), GTK_WIDGET(gdata));
    }
    i--;
  }


  msg_path = procmsg_get_message_file_path(msginfo);
                    

  debug_print("[DEBUG] msg_path:%s\n", msg_path);


  if (bfound != TRUE){
    copy_btn = gtk_button_new_from_stock(GTK_STOCK_FILE);
    gtk_box_pack_end(GTK_BOX(hbox), copy_btn, FALSE, FALSE, 0);

    pbuf = gdk_pixbuf_new_from_xpm_data((const char**)page_save);
    image = gtk_image_new_from_pixbuf(pbuf);
    
    gtk_button_set_image(GTK_BUTTON(copy_btn), image);
    gtk_button_set_label(GTK_BUTTON(copy_btn), "");

    tip = gtk_tooltips_new();
    gtk_tooltips_set_tip(tip, copy_btn, _("Copy this mail to clipboard."), NULL);

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

#if defined(G_OS_WIN32)
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
#endif
  } 
}



GtkWidget *create_config_main_page(GtkWidget *notebook, GKeyFile *pkey)
{
  GtkWidget *vbox;
  GtkWidget *startup_align;
  GtkWidget *startup_frm;
  GtkWidget *startup_frm_align;
  GtkWidget *general_lbl;
  
  debug_print("create_config_main_page\n");
  if (notebook == NULL){
    return NULL;
  }
  /* startup */
  if (pkey!=NULL){
  }
  vbox = gtk_vbox_new(FALSE, 0);

  /**/
  startup_align = gtk_alignment_new(0, 0, 1, 1);
  gtk_alignment_set_padding(GTK_ALIGNMENT(startup_align), ALIGN_TOP, ALIGN_BOTTOM, ALIGN_LEFT, ALIGN_RIGHT);

  startup_frm = gtk_frame_new(_("Startup Option"));
  startup_frm_align = gtk_alignment_new(0, 0, 1, 1);
  gtk_alignment_set_padding(GTK_ALIGNMENT(startup_frm_align), ALIGN_TOP, ALIGN_BOTTOM, ALIGN_LEFT, ALIGN_RIGHT);


  g_opt.startup = gtk_check_button_new_with_label(_("Enable plugin on startup."));
  gtk_container_add(GTK_CONTAINER(startup_frm_align), g_opt.startup);
  gtk_container_add(GTK_CONTAINER(startup_frm), startup_frm_align);
  gtk_container_add(GTK_CONTAINER(startup_align), startup_frm);

  gtk_widget_show(g_opt.startup);

  /**/
  gtk_box_pack_start(GTK_BOX(vbox), startup_align, FALSE, FALSE, 0);

  general_lbl = gtk_label_new(_("General"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, general_lbl);
  gtk_widget_show_all(notebook);
  return NULL;

}

