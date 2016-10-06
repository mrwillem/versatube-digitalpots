#include "hw_config.h"
#include "userio.h"
#include "main.h"
#include "configmenu.h"


uint8_t configmenustatus;

void configmenu_init(void)
{
	configmenustatus=CONFIGMENU_LANDING;
}

void configmenu(uint8_t button)
{
	switch(configmenustatus)
	{
	case CONFIGMENU_LANDING:
		configmenu_landing(button);
		break;
	case CONFIGMENU_MIDICHANNEL_CHANGE:
		configmenu_midichange(button);
		break;
	}
}
/* This is the Basic Configmenu Site */
void configmenu_landing(uint8_t button)
{
	switch(button)
	{
	case 0:
		if(GL_displaydata.displaybuffer.channel > 3)
		{
			GL_displaydata.displaybuffer.channel = 1;
		}
		else
		{
			GL_displaydata.displaybuffer.channel++;
		}
		channelled_display_channel(GL_displaydata.displaybuffer.channel);
		segment_display_float_text("Hello Willem", 12);
		break;
	case 1:
		if((GL_displaydata.displaybuffer.channel > 7) || (GL_displaydata.displaybuffer.channel < 5))
		{
			GL_displaydata.displaybuffer.channel = 5;
		}
		else
		{
			GL_displaydata.displaybuffer.channel++;
		}
		channelled_display_channel(GL_displaydata.displaybuffer.channel);
		segment_display_float_text("What The Fuck", 13);
		break;
	case 2:
		if((GL_displaydata.displaybuffer.channel > 11) || (GL_displaydata.displaybuffer.channel < 9))
		{
			GL_displaydata.displaybuffer.channel = 9;
		}
		else
		{
			GL_displaydata.displaybuffer.channel++;
		}
		channelled_display_channel(GL_displaydata.displaybuffer.channel);
		segment_display_float_text("I LOVE DANNI", 12);
		break;
	case 3:
		if(GL_displaydata.displaybuffer.midichannel <127)
		{
			GL_displaydata.displaybuffer.midichannel++;
		}
		else
		{
			GL_displaydata.displaybuffer.midichannel=0;
		}
		segment_display_number(GL_displaydata.displaybuffer.midichannel+1);
		display_blink(BLINKSEGMENT);
		configmenustatus=CONFIGMENU_MIDICHANNEL_CHANGE;
		break;
	case 4:
		if(GL_displaydata.displaybuffer.midichannel >0)
		{
			GL_displaydata.displaybuffer.midichannel--;
		}
		else
		{
			GL_displaydata.displaybuffer.midichannel=127;
		}
		segment_display_number(GL_displaydata.displaybuffer.midichannel+1);
		display_blink(BLINKSEGMENT);
		configmenustatus=CONFIGMENU_MIDICHANNEL_CHANGE;
		break;
	}
}

void configmenu_midichange(uint8_t button)
{
	switch (button)
	{
	case 8:
		display_blink(0);
		// update system midi channel
		configmenustatus=CONFIGMENU_LANDING;
		break;
	case 3:
		if(GL_displaydata.displaybuffer.midichannel < 127 )
		{
			GL_displaydata.displaybuffer.midichannel++;
		}
		else
		{
			GL_displaydata.displaybuffer.midichannel=0;
		}
		segment_display_number(GL_displaydata.displaybuffer.midichannel+1);


		break;
	case 4:
		if(GL_displaydata.displaybuffer.midichannel >0)
		{
			GL_displaydata.displaybuffer.midichannel--;
		}
		else
		{
			GL_displaydata.displaybuffer.midichannel=127;
		}
		segment_display_number(GL_displaydata.displaybuffer.midichannel+1);


		break;
	default:
		// debug
		GL_displaydata.displaybuffer.midichannel=10;
		segment_display_number(GL_displaydata.displaybuffer.midichannel);
		configmenustatus=CONFIGMENU_LANDING;
		display_blink(0);
		break;
	}

}


