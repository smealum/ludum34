#ifndef HUD_H
#define HUD_H

#include "text.h"

class Hud
{
	public:
		Hud();

		void updateSelectedLayer(int layer);
		void draw();

	private:
		Text selectionText;

};

#endif
