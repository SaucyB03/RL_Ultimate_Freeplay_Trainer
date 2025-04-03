#include "pch.h"
#include "Freeplay_Trainer.h"


BAKKESMOD_PLUGIN(Freeplay_Trainer, "Ultimate Freeplay Trainer", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void Freeplay_Trainer::onLoad()
{
	_globalCvarManager = cvarManager;
	LOG("Freeplay Customizer loaded!");

}

void Freeplay_Trainer::onUnload() {
	LOG("yeeee");
}

void Freeplay_Trainer::Load_Presets() {
	std::ifstream stream(gameWrapper->GetBakkesModPath() / "data" / "presetscfg.txt");
}

