#pragma once
#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
using namespace geode::prelude;

class LevelItem : public CCScale9Sprite {
    protected:
        EventListener<web::WebTask> m_listener;

        CCClippingNode* m_clippingNode;
        GJGameLevel* m_level;
        virtual bool init(GJGameLevel*, bool expand);
        virtual bool init();
        CCScale9Sprite* bgSpr;
        CCScale9Sprite* selectedBgSpr;
        void loadImage();
        void finishLoadImage(CCImage* image);
        void errorLoadImage();
        bool m_expanded;
        std::function<void(bool)> m_callback = [](bool){};
    public:
        CCTexture2D* m_levelPreview;
        CCLabelBMFont* m_levelInfoName;
        CCLabelBMFont* m_levelInfoAuthor;
        bool m_loaded;
        void setCallback(std::function<void(bool)> callback) {
            m_callback = callback;
        }
        void setSelected(bool visible) {
            selectedBgSpr->setVisible(visible);
            this->runAction(CCEaseBackOut::create(CCScaleTo::create(0.25F, (visible) ? 1.05F : 1.0F)));
            if (m_level != nullptr) {
                if (visible) {
                    m_levelInfoName->runAction(CCSpawn::createWithTwoActions(
                        CCFadeIn::create(0.25F),
                        CCMoveTo::create(0.25F, {0, -6})
                    ));
                    m_levelInfoAuthor->runAction(CCSpawn::createWithTwoActions(
                        CCFadeIn::create(0.25F),
                        CCMoveTo::create(0.25F, {0, -10})
                    ));
                } else {
                    m_levelInfoName->runAction(CCSpawn::createWithTwoActions(
                        CCFadeOut::create(0.25F),
                        CCMoveTo::create(0.25F, {0, -11})
                    ));
                    m_levelInfoAuthor->runAction(CCSpawn::createWithTwoActions(
                        CCFadeOut::create(0.25F),
                        CCMoveTo::create(0.25F, {0, -15})
                    ));
                }
            }
        }
        void selectLevel();
        static LevelItem* create(GJGameLevel*, bool expand);
        static LevelItem* create();
};
