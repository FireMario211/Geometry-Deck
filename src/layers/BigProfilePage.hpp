#pragma once 
#include <Geode/Geode.hpp>
using namespace geode::prelude;

class BigProfilePage : public CCLayerGradient {
    protected:
        CCMenu* m_buttonMenu;
        virtual bool init() override;
        
    public:
        static BigProfilePage* create();
        void byeBye() {
            this->removeFromParentAndCleanup(true);
        }
        void killMe() {
            this->runAction(CCSequence::create(
                CCSpawn::create(CCFadeOut::create(0.25F), CCScaleTo::create(0.25F, 0.98F)),
                CCDelayTime::create(0.25F),
                CCCallFunc::create(this, callfunc_selector(BigProfilePage::byeBye)),
                nullptr
            ));
        }
        void show(int zOrder) {
            auto scene = CCScene::get();
            this->setScale(0.98F);
            if (!scene->getChildByType<BigProfilePage>(0)) {
                /*int highZOrder = scene->getHighestChildZ();
                if (104 < highZOrder) {
                    highZOrder++;
                } else {
                    highZOrder = 105;
                }*/
                scene->addChild(this, zOrder);
                this->runAction(CCSpawn::createWithTwoActions(CCFadeIn::create(0.25F), CCScaleTo::create(0.25F, 1.0F)));
            }
        }
};
