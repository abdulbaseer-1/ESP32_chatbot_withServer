#pragma once

typedef void (*wakeword_callback_t)(void);
//typedef void (*command_callback_t)(const char *command);

void wakeword_init(wakeword_callback_t wake_cb);//, command_callback_t command_cb
void wakeword_task(void *arg);
