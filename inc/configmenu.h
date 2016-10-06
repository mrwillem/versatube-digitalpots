#ifndef CONFIGMENU_H
#define CONFIGMENU_H

extern uint8_t GL_configmenustatus;

void configmenu_landing(uint8_t);
void configmenu_midichange(uint8_t);
void configmenu(uint8_t);
void configmenu_init(void);


#define CONFIGMENU_LANDING 0
#define CONFIGMENU_MIDICHANNEL_CHANGE 1
#define CONFIGMENU_SETUP_LANDING 2
#endif CONFIGMENU_H
