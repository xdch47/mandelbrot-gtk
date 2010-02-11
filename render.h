
#ifndef __RENDER_H__
#define __RENDER_H__

#include "interface.h"

void init_render_thread(struct winctl *w);
gboolean is_render_thread_alive(struct winctl *w);
gboolean is_render_thread_pause(struct winctl *w);
gboolean start_render_thread(struct winctl *w);
void pause_render_thread(struct winctl *w);
void resume_render_thread(struct winctl *w);
void kill_render_thread(struct winctl *w);
void destroy_render_thread(struct winctl *w);
#endif /* __RENDER_H__ */

