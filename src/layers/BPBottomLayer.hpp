#pragma once 
#include <Geode/Geode.hpp>
using namespace geode::prelude;

class BPBottomLayer : public CCLayer {
    protected:
        CCMenu* m_buttonMenu;
        CCMenuItemSpriteExtra* m_menuBtn;
        CCMenuItemSpriteExtra* m_optBtn;
        CCMenuItemSpriteExtra* m_selBtn;
        CCMenuItemSpriteExtra* m_backBtn;
        CCLabelBMFont* m_menuBtnLbl;
        CCLabelBMFont* m_optBtnLbl;
        CCLabelBMFont* m_selBtnLbl;
        CCLabelBMFont* m_backBtnLbl;

        virtual bool init();
    public:
        void changeButtonState(int visibility) {
            m_optBtn->setVisible(false);
            m_backBtn->setVisible(false);
            m_optBtnLbl->setVisible(false);
            m_backBtnLbl->setVisible(false);
            m_menuBtnLbl->setVisible(true);
            m_menuBtn->setVisible(true);
            switch (visibility) {
                case 1: // only SELECT
                    m_selBtn->updateAnchoredPosition(geode::Anchor::Right, {-100, 0});
                    m_selBtnLbl->updateAnchoredPosition(Anchor::Right, {-65, 0});
                    break;
                case 2: // only SELECT & BACK
                    m_backBtn->setVisible(true);
                    m_backBtnLbl->setVisible(true);
                    m_selBtn->updateAnchoredPosition(geode::Anchor::Right, {-160, 0});
                    m_selBtnLbl->updateAnchoredPosition(Anchor::Right, {-125, 0});
                    break;
                case 3: // EVERYTHING
                    m_optBtn->setVisible(true);
                    m_backBtn->setVisible(true);
                    m_backBtnLbl->setVisible(true);
                    m_optBtnLbl->setVisible(true);
                    m_selBtn->updateAnchoredPosition(geode::Anchor::Right, {-160, 0});
                    m_selBtnLbl->updateAnchoredPosition(Anchor::Right, {-125, 0});
                    break;
                case 4: // settings exclusive
                    m_menuBtn->setVisible(false);
                    m_menuBtnLbl->setVisible(false);
                    m_backBtn->setVisible(true);
                    m_backBtnLbl->setVisible(true);
                    m_selBtn->updateAnchoredPosition(geode::Anchor::Right, {-160, 0});
                    m_selBtnLbl->updateAnchoredPosition(Anchor::Right, {-125, 0});
                    break;
            }
        }
        //std::function<void()> pressSelectCB = [](){};
        static BPBottomLayer* create();
        void setBackCallback(SEL_MenuHandler callback) {};
};
