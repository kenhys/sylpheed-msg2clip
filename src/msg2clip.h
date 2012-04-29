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

#ifndef __MSG2CLIP_H__
#define __MSG2CLIP_H__

#define MSG2CLIP "msg2clip"
#define MSG2CLIPRC "msg2cliprc"
#define _(String)   dgettext(MSG2CLIP, String)
#define N_(String)  gettext_noop(String)
#define gettext_noop(String) (String)

#define PLUGIN_NAME N_("Msg2Clip Plug-in")
#define PLUGIN_DESC N_("Copy message to clipboard")

#define GET_RC_BOOLEAN(keyarg) g_key_file_get_boolean(g_opt.rcfile, MSG2CLIP, keyarg, NULL)

typedef struct _Msg2ClipOption Msg2ClipOption;

struct _Msg2ClipOption {
  /* General section */

  /* full path to ghostbiffrc*/
  gchar *rcpath;
  /* rcfile */
  GKeyFile *rcfile;
  
  GtkWidget *startup;

};
    
#define ALIGN_TOP 3
#define ALIGN_BOTTOM 3
#define ALIGN_LEFT 6
#define ALIGN_RIGHT 6
#define BOX_SPACE 6

static void messageview_show_cb(GObject *obj, gpointer msgview,
                                MsgInfo *msginfo, gboolean all_headers);

#endif /* __MSG2CLIP_H__ */
