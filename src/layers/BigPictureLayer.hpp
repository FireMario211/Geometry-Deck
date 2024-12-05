#pragma once 
#include "BPBottomLayer.hpp"
#include <Geode/Geode.hpp>
using namespace geode::prelude;

class BigPictureLayer : public CCLayer {
    protected:
        CCMenu* m_buttonMenu;
        CCSprite* m_sideMenuBG;
        CCMenu* m_sideMenu;
        CCMenuItemSpriteExtra* m_chestBtn;
        CCMenuItemSpriteExtra* m_batteryBtn;
        CCLabelBMFont* m_timeLabel;

        CCSprite* m_batteryFill;
        CCSprite* m_batteryCharging;
        CCSprite* m_batteryWarning;
        CCLabelBMFont* m_batteryLabel;

        CCScale9Sprite* m_barBG;
        CCLayerColor* m_coverAllBG = CCLayerColor::create({0, 0, 0, 200});
        bool m_allowSelecting = true;

        void buttonCallback(CCObject* sender);
        //CCMenuItemSpriteExtra* createButton(CCSprite* iconSpr, std::string name, SEL_MenuHandler selector, std::function<void(CCObject*)> callback);
        CCMenuItemSpriteExtra* createButton(CCSprite* iconSpr, std::string name, SEL_MenuHandler selector);
        CCMenuItemSpriteExtra* createButton(CCSprite* iconSpr, std::string name, std::function<void(CCObject*)> callback);

        virtual bool init() override;
        void updateTime();
        void allowSelect() {
            m_allowSelecting = true;
        }
        CCScale9Sprite* m_selectionBG;
        int currentIndexSelection = 0;
        int maxIndex = 0;
        void setSelected(int tag);
        void enterSelected();
        //void keyDown(enumKeyCodes) override;
    public:
        BPBottomLayer* m_bottomLayer;
        void updateBattery();
        void createStatusbar();
        bool keyPressDown(enumKeyCodes);
        std::string lastIDClicked = "";
        std::vector<std::string> blacklistedMenuIDs = {"geode.loader/geode-button", "dankmeme.globed2/main-menu-button"};
        static BigPictureLayer* create();
        void setBackCallback(SEL_MenuHandler callback) {
            m_bottomLayer->setBackCallback(callback);
        }
        void toggleBar();
        void toggleBarChangeTop(bool moveDown);
        void toggleBar(bool force);
        void enableAllButtons();
        void setShown(bool shown) {
            /*this->setVisible(shown);
            this->setKeyboardEnabled(shown);
            this->setTouchEnabled(shown);*/
        }
};
