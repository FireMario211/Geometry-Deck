#pragma once 
#include <Geode/Geode.hpp>
using namespace geode::prelude;

struct BigPicButton {
    std::string name;
    std::function<void()> callback;
    ccColor3B selectedBG = {255,255,255};
    ccColor3B selectedText = {0,0,0};
};

class BPAlert : public FLAlertLayer {
    protected:
        std::vector<BigPicButton> m_options;
        CCMenu* m_buttonMenu;
        bool isScrollable = false;
        bool canTouch = false;
        void willTouch() {
            canTouch = true;
        }
        virtual bool init(std::string title, std::vector<BigPicButton>);
        void keyDown(enumKeyCodes) override;
        void keyBackClicked() override {};
        void ccTouchEnded(CCTouch*, CCEvent*) override;
        int currentlySelected = -2;
        void regenItems(int page);
        void setSelected(int tag, bool selected);
        void enterSelected();
    public:
        void onClose(cocos2d::CCObject* pSender);
        static BPAlert* create(std::string title, std::vector<BigPicButton>);
};
