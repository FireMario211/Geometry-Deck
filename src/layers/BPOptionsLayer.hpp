#pragma once 
#include "BigPictureLayer.hpp"
#include "../nodes/ScrollLayerExt.hpp"
#include <Geode/Geode.hpp>
using namespace geode::prelude;

enum class BPOptionType {
    Title,
    Toggle,
    Input,
    Slider,
    Button,
    Dropdown
};
enum class BPValueType {
    IntVar,
    DSDict,
    Geode
};
struct BPOption {
    const char* name;
    std::string id;
    std::string desc;
    BPOptionType type;
    BPValueType valueType;
    matjson::Value definedValue;
    std::function<void(matjson::Value)> callback;
};

struct BPCategory {
    const char* name;
    const char* spriteName;
    std::vector<BPOption> options;
};

/*
# General
Battery Percentage
Show Chest Icon
Show Battery Icon
Skip Intro
Low Detail Mode

# Audio
Music
SFX
Menu Music

# Graphics
Fullscreen [Button]
Borderless [Button]
Borderless Fix [Button]
Windowed Resolution [Dropdown]
Texture Quality [Dropdown]

## Advanced
Vertical Sync [Button]
Smooth Fix [Button]
Show FPS [Button]
Unlock FPS [Button]
FPS [Input]

---
# Geometry Dash
## Visual
(bunch of gd settings)

## Gameplay
## Practice
## Performance
## Audio
## Other
## Parental Controls
Disable Comments
Disable Account Comments
Only Allow Featured Levels

---
# Misc 
Rate [Button]
Keybinds [Button]
# Debug

*/

class BPOptionsLayer : public CCLayerGradient {
    protected:
        CCMenu* m_sideMenu;
        CCMenu* m_scrollContent;
        ScrollLayerExt* m_scrollLayer;
        std::vector<BPCategory> m_categories;

        CCScale9Sprite* m_sideSelectionBG;
        CCScale9Sprite* m_selectionBG;
        bool isOnSide = true;
        int sideMenuIndex = 0;
        int currentSelIndex = 0;
        void setSMSelected();
        void setSelected(int tag);
        void enterSelected();

        void updateOption(BPOption, matjson::Value);
        void onSliderChange(CCObject*);

        void regenCategory(BPCategory);
        virtual bool init(std::vector<BPCategory> categories);
    public:
        bool keyPressDown(enumKeyCodes key);
        static BPOptionsLayer* create(std::vector<BPCategory> categories);
        void killMe() {
            if (auto menuLayer = MenuLayer::get()) {
                if (auto bigPicture = menuLayer->getChildByType<BigPictureLayer>(0)) {
                    bigPicture->toggleBarChangeTop(false);
                    bigPicture->m_bottomLayer->changeButtonState(3);
                }
            }
            this->removeFromParentAndCleanup(true);
        }
        void show() {
            auto scene = MenuLayer::get();
            if (!scene) return;
            if (!scene->getChildByType<BPOptionsLayer>(0)) {
                scene->addChild(this, 80);
                this->runAction(CCFadeIn::create(0.25F));
            }
        }
        void show(bool inScene) {
            if (!inScene) return show();
            auto scene = CCScene::get();
            this->setScale(0.98F);
            if (!scene->getChildByType<BPOptionsLayer>(0)) {
                int highZOrder = scene->getHighestChildZ();
                if (104 < highZOrder) {
                    highZOrder++;
                } else {
                    highZOrder = 105;
                }
                scene->addChild(this, highZOrder);
                this->runAction(CCSpawn::createWithTwoActions(CCFadeIn::create(0.25F), CCScaleTo::create(0.25F, 1.0F)));
            }
        }
};
