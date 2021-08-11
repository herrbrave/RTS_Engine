#pragma once
#ifndef __WIDGET_FACTORY__
#define __WIDGET_FACTORY__

#include "Ui.h"

class UIConfig;
typedef shared_ptr<UIConfig> UIConfigPtr;
typedef weak_ptr<UIConfig> WeakUIConfigPtr;

class WidgetFactory;
typedef shared_ptr<WidgetFactory> WidgetFactoryPtr;
typedef weak_ptr<WidgetFactory> WeakWidgetFactoryPtr;

void applyButtonWithText(SystemManagerPtr systemManager, unsigned long entityId, float w, float h, const string& text, const string& font, const string& script, ButtonConfigPtr buttonConfig);
void applyButtonWithIcon(SystemManagerPtr systemManager, unsigned long entityId, float w, float h, TexturePtr icon, const string& script, ButtonConfigPtr buttonConfig);
void applyLabel(SystemManagerPtr systemManager, unsigned long entityId, const string& text, const string& font, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
void applyProgress(SystemManagerPtr systemManager, unsigned long entityId, float w, float h, unsigned int maxProgress, unsigned int currentProgress, const string& location);
void applyTextbox(SystemManagerPtr systemManager, unsigned long entityId, float w, float h, const string& text, const string& font, TextboxConfigPtr textboxConfig);

ButtonConfigPtr createButtonConfig(const rapidjson::Value& value, SystemManagerPtr systemManager);
void parseState(const std::string& tag, const rapidjson::Value& button, std::unordered_map<std::string, TexturePtr>& stateMap);
PanelConfigPtr createPanelConfig(const rapidjson::Value& value, SystemManagerPtr systemManager);
TextboxConfigPtr createTextboxConfig(const rapidjson::Value& value, SystemManagerPtr systemManager);
UIConfigPtr parseUIConfig(const std::string& path, SystemManagerPtr systemManager);

class WidgetFactory : public EntityFactory {
public:
	WidgetFactory(const string& uiConfigPath, SystemManagerPtr systemManager);

	EntityPtr createLabel(std::string text, std::string font, float x, float y);
	EntityPtr createButtonWithText(const string& text, const string& font, const string& script, float x, float y, float width, float height);
	EntityPtr createButtonWithIcon(TexturePtr icon, const string& script, float x, float y, float width, float height);
	EntityPtr createPanel(float x, float y, float width, float height);
	EntityPtr createProgressBar(float x, float y, float width, float height, unsigned int progressMax, unsigned int currentProgress);
	EntityPtr createTextBox(float x, float y, float width, float height, const string& text, const string& font);

private:
	UIConfigPtr uiConfig;
};

#endif