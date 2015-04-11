/*
 * Msg2Clip -- 
 * Copyright (C) 2012-2015 HAYASHI Kentaro <kenhys@gmail.com>
 *
 */

#ifndef __MSG2CLIP_H__
#define __MSG2CLIP_H__

#define MSG2CLIP "msg2clip"
#define MSG2CLIPRC "msg2cliprc"

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
