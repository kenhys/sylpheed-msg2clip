#include <gtk/gtk.h>
#include <windows.h>
#include <shlobj.h>
#include <tchar.h>

void callback( GtkWidget *widget,
               gpointer   data )
{
  g_print ("callback - %s was pressed\n", (char *) data);

  GtkWidget* chooser = gtk_file_chooser_dialog_new("Select FILES",
						   NULL,
						   GTK_FILE_CHOOSER_ACTION_OPEN,
						   GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
						   GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
						   NULL);
  
  gchar* filename = NULL;
  if (gtk_dialog_run (GTK_DIALOG (chooser)) == GTK_RESPONSE_ACCEPT)
  {
    filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (chooser));
  }
  gtk_widget_destroy (chooser);
      
  LPDROPFILES lpDropFile;

  int mblen = MultiByteToWideChar(CP_UTF8, 0, filename, -1, NULL,0);
  WCHAR* pszUnicode = malloc(sizeof(WCHAR)*( mblen+1));
  memset(pszUnicode, sizeof(WCHAR), (mblen+1));

  MultiByteToWideChar(CP_UTF8, 0, filename, strlen(filename)+1, pszUnicode, mblen);
  

  HDROP hDrop = (HDROP)GlobalAlloc(GHND,sizeof(DROPFILES)+2*mblen+2);
  if (hDrop==NULL) {
    return;
  }
  
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
    GlobalFree(hDrop);
    return;
  }
  EmptyClipboard();
  SetClipboardData(CF_HDROP, hDrop);
  CloseClipboard();
}

void delete_event( GtkWidget *widget,
                   GdkEvent  *event,
                   gpointer   data )
{
    gtk_main_quit ();
}

int main( int   argc,
          char *argv[] )
{
  GtkWidget *window;
  GtkWidget *button;
  GtkWidget *box;

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  gtk_window_set_title (GTK_WINDOW (window), "Hello Buttons!");

  gtk_signal_connect (GTK_OBJECT (window), "delete_event",
		      GTK_SIGNAL_FUNC (delete_event), NULL);

  gtk_container_set_border_width (GTK_CONTAINER (window), 10);

  box = gtk_hbox_new(FALSE, 0);

  gtk_container_add (GTK_CONTAINER (window), box);

  button = gtk_button_new_with_label ("Button 1");

  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (callback), (gpointer) "button 1");

  gtk_box_pack_start(GTK_BOX(box), button, TRUE, TRUE, 0);

  gtk_widget_show(button);
  gtk_widget_show(box);
  gtk_widget_show (window);

  gtk_main ();

  return(0);
}
