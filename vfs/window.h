#ifndef WINDOW_H
#define WINDOW_H

typedef void EventCallback(int type, float ts, int p1, int p2);

void win_init(EventCallback* callback);
void win_fini();
void win_update();
double win_time();
void win_size(int *w, int *h);
int win_should_close();

#endif
