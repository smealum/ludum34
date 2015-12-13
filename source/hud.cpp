#include "hud.h"

Hud::Hud():
	selectionText("selected layer : white ", glm::vec2(-0.95, 0.90))
{

}

const char* layer_names[] = {
	"white",
	"blue",
	"red",
};

void Hud::updateSelectedLayer(int layer)
{
	// if(layer < 0 || layer >= 3) return;

	// static char buffer[32];
	// sprintf(buffer, "selected layer : %s", layer_names[layer]);
	// selectionText.update(buffer);
}

void Hud::draw()
{
	// selectionText.draw();
}
