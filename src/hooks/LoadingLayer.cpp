#include <Geode/Geode.hpp>
using namespace geode::prelude;
#include <Geode/modify/LoadingLayer.hpp>

#include "../utils.hpp"

int determineIntro() {
    int event1 = rand() % 1000;
    int event2 = rand() % 10000;
    log::debug("ROLL THE DICE :drum::drum:{}-{}", event1, event2);
    if (event1 == 1) {
        return 1;
    } else if (event2 == 1) {
        return 1; // 2
    } else {
        return 0;
    }
}

class $modify(BigPicIntro, LoadingLayer) {
    struct Fields {
        CCSprite* m_outlineSpr;
        CCSprite* fillSpr;
        CCSprite* m_gdLogo;
        CCSprite* whiteSquare;
        bool playedAnimation = false;
        bool skipped = false;
        int introType = determineIntro();
    };
    
    void introStep4() {
        if (m_fields->skipped) return;
        if (auto GM = GameManager::sharedState()) {
            //GM->fadeInMenuMusic();
            //CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, MenuLayer::scene(m_fromRefresh)));
            LoadingLayer::loadAssets();
        }
    }
    void introStep3() {
        if (m_fields->skipped) return;
        if (auto fmod = FMODAudioEngine::sharedEngine()) {
            fmod->playEffect("chest07.ogg");
        }
        m_fields->fillSpr->runAction(CCSequence::create(
            CCEaseExponentialOut::create(
                CCMoveBy::create(0.5F, {-165, 0})
            ),
            nullptr
        ));
        m_fields->m_gdLogo->setPositionX(m_fields->m_gdLogo->getPositionX() + 60);
        auto oldPos = m_fields->m_gdLogo->getPosition();
        if (m_fields->introType > 0) {
            m_fields->m_gdLogo->removeMeAndCleanup();
            m_fields->m_gdLogo = CCSprite::create("soggylogo.png"_spr);
            this->addChild(m_fields->m_gdLogo);
            m_fields->m_gdLogo->setPosition(oldPos);
        }
        m_fields->m_gdLogo->setVisible(true);
        m_fields->m_gdLogo->runAction(CCSequence::create(
            CCEaseExponentialOut::create(
                CCFadeIn::create(0.5F)
            ),
            CCDelayTime::create(1.5F),
            CCCallFunc::create(this, callfunc_selector(BigPicIntro::introStep4)),
            nullptr
        ));
        // 120 for fillSpr
        // 345 for gd logo
    }


    void introStepParticle2() {
        auto director = CCDirector::sharedDirector();
        auto boom2 = Utils::createStarParticles(0.25F, 150, {255, 255, 255}, { 500, 50 }, {200, 0});
        boom2->setScale(0.7F);
        boom2->setPosition(director->getWinSize() / 2);

        auto boom = Utils::createStarParticles(1.F, 40, {254, 209, 0}, { 500, 125 }, {150, 0});
        boom->setScale(0.7F);
        boom->setPosition(director->getWinSize() / 2);
        this->addChild(boom);
        this->addChild(boom2);
    }

    void introStep2() {
        if (m_fields->skipped) return;
        /*m_fields->fillSpr->stopAllActions();
        m_fields->whiteSquare->stopAllActions();
        //m_fields->fillSpr->setTexture(CCSprite::create("gdlogo-real"_spr)->getTexture());
        m_fields->whiteSquare->runAction(CCSequence::create(
            CCEaseExponentialOut::create(
                CCFadeOut::create(0.5F)
            )
        ));*/

        auto director = CCDirector::sharedDirector();
        m_fields->fillSpr->removeMeAndCleanup();
        if (m_fields->introType == 0) {
            m_fields->fillSpr = CCSprite::create("gdlogo-real.png"_spr);
            m_fields->fillSpr->setScale(0.7F);
        } else {
            m_fields->fillSpr = CCSprite::create("soggy.png"_spr);
            m_fields->fillSpr->setScale(0.2F);
        }
        m_fields->fillSpr->setPosition(director->getWinSize() / 2);

        this->addChild(m_fields->fillSpr);

        //m_fields->fillSpr->stopActionByTag(2322);
        //m_fields->fillSpr->setTexture(CCSprite::create("gdlogo-real"_spr)->getTexture());
        m_fields->whiteSquare->runAction(CCSequence::create(
            CCEaseExponentialOut::create(
                CCFadeOut::create(0.5F)
            ),
            CCDelayTime::create(1.5F),
            CCCallFunc::create(this, callfunc_selector(BigPicIntro::introStep3)),
            nullptr
        ));
    }

    void introAnimation() {
        if (m_fields->skipped) return;
        auto skipMenu = CCMenu::create();
        auto skipBtn = CCMenuItemExt::createSpriteExtra(CCLabelBMFont::create("Skip", "NunitoBold.fnt"_spr), [this](CCObject*) {
            m_fields->skipped = true;
            LoadingLayer::loadAssets();
        });
        skipBtn->setOpacity(0);
        skipMenu->addChildAtPosition(skipBtn, Anchor::TopLeft, {30, -20});
        this->addChild(skipMenu);
        skipBtn->runAction(CCSequence::create(
            CCDelayTime::create(0.94F),
            CCMoveBy::create(0.F, {0, -10}),
            CCSpawn::createWithTwoActions(CCEaseExponentialOut::create(CCFadeIn::create(0.25F)), CCEaseExponentialOut::create(CCMoveBy::create(0.25F, {0, 10}))),
            nullptr
        ));
        m_fields->playedAnimation = true;
        this->setKeypadEnabled(true);
        this->setKeyboardEnabled(true);
        m_fields->m_gdLogo->setScale(0.8F);
        m_caption->runAction(CCSequence::create(
            CCEaseExponentialOut::create(
                CCFadeOut::create(0.5F)
            ),
            nullptr
        ));
        m_textArea->runAction(CCSequence::create(
            CCEaseExponentialOut::create(
                CCFadeOut::create(0.5F)
            ),
            nullptr
        ));
        if (auto lbl1 = as<CCLabelBMFont*>(getChildByID("geode-small-label"))) {
            lbl1->runAction(CCSequence::create(
                CCEaseExponentialOut::create(
                    CCFadeOut::create(0.5F)
                ),
                nullptr
            ));
        }
        if (auto lbl2 = as<CCLabelBMFont*>(getChildByID("geode-small-label-2"))) {
            lbl2->runAction(CCSequence::create(
                CCEaseExponentialOut::create(
                    CCFadeOut::create(0.5F)
                ),
                nullptr
            ));
        }
        auto director = CCDirector::sharedDirector();
        if (auto fmod = FMODAudioEngine::sharedEngine()) {
            fmod->stopAllMusic(true);
            //fmod->playEffect("chest07.ogg");
            fmod->playEffect("unlockPath.ogg");
            //fmod->playMusic("menuLoop.mp3", false, 1.0, 0);
        }
        // have the animation thingy, then play chest07.ogg afterwards to show the "Geometry Dash"
        // basically, have the logo move to the left and the "Geometry Dash" move to the right (with clipping) then fade to the main menu
        m_fields->fillSpr->getParent()->removeMeAndCleanup();
        m_fields->m_outlineSpr->removeMeAndCleanup();
        //m_fields->fillSpr = CCSprite::create("gdlogo-real.png"_spr);
        m_fields->fillSpr = CCSprite::create("gdlogo-fill.png"_spr);
        m_fields->fillSpr->setPosition(director->getWinSize() / 2);
        this->addChild(m_fields->fillSpr);

        m_fields->whiteSquare = CCSprite::create("square02b_001.png");
        m_fields->whiteSquare->setScale(0.3F);
        m_fields->whiteSquare->setOpacity(0);
        m_fields->whiteSquare->setPosition(director->getWinSize() / 2);
        this->addChild(m_fields->whiteSquare, 1);

        float moveBy = 2;
        float amountTime = 0.05F;
        auto jitter = CCSequence::create(
            CCMoveBy::create(amountTime, {-moveBy, 0}),
            CCMoveBy::create(amountTime, {moveBy * 2, 0}),
            CCMoveBy::create(amountTime, {-moveBy * 2, 0}),
            CCMoveBy::create(amountTime, {moveBy * 2, 0}),
            CCMoveBy::create(amountTime, {-moveBy * 2, 0}),
            CCMoveBy::create(amountTime, {moveBy, 0}),
            nullptr
        );

        m_fields->fillSpr->setScale(0.25F);
        m_fields->whiteSquare->runAction(
            CCSequence::create(
                CCEaseExponentialOut::create(CCScaleTo::create(0.25f, 0.3F)),
                CCEaseSineIn::create(CCSpawn::createWithTwoActions(
                    CCScaleTo::create(2.2074f, 0.9F),
                    CCFadeIn::create(2.2074))
                ), // haha funny reference
                nullptr
            )
        );
        m_fields->fillSpr->runAction(
            CCSequence::create(
                CCEaseExponentialOut::create(CCScaleTo::create(0.25f, 0.25F)),
                CCEaseSineIn::create(CCScaleTo::create(2.2074f, 0.7F)), // haha funny reference
                nullptr
            )
        );
        
        m_fields->fillSpr->runAction(
            CCRepeatForever::create(
                jitter
            )
        );

        m_fields->fillSpr->runAction(CCSequence::create(
            CCDelayTime::create(2.2f),
            CCCallFunc::create(this, callfunc_selector(BigPicIntro::introStepParticle2)),
            nullptr
        ));
        m_fields->fillSpr->runAction(CCSequence::create(
            CCDelayTime::create(2.4574f),
            CCCallFunc::create(this, callfunc_selector(BigPicIntro::introStep2)),
            nullptr
        ));

        // 0.4F
        auto boom = Utils::createStarParticles(0.25F, 70, {255, 255, 250}, { 500, 205 }, {100, 10});
        boom->setScale(0.6F);
        boom->setPosition(director->getWinSize() / 2);
        this->addChild(boom);

        //m_fields->fillSpr->addChildAtPosition(Utils::createStarParticles(-1, 70, {255, 255, 255}, { 300, 125 }, {0, 90}), Anchor::Center);
        m_fields->fillSpr->addChildAtPosition(Utils::createStarParticles(-1, 70, {255, 255, 255}, { 500, 125 }, {0, 150}), Anchor::Center);
    }
    bool init(bool refresh) {
        if (!LoadingLayer::init(refresh)) return false;
        if (refresh) return true;
        auto mod = Mod::get();
        if (!mod) return true;
        auto size = CCDirector::sharedDirector()->getWinSize();
        if (auto bg = as<CCSprite*>(getChildByID("bg-texture"))) {
            bg->setTexture(CCSprite::create("GJ_gradientBG.png")->getTexture());
            bg->setColor({10,10,10});
        }
        if (auto node = as<CCSprite*>(getChildByID("gd-logo"))) {
            node->setVisible(false);
            m_fields->m_gdLogo = node;
        }
        Utils::removeAllInVec(this, { "robtop-logo", "cocos2d-logo", "fmod-logo" });
        auto searchPathRoot = dirs::getModRuntimeDir() / mod->getID() / "resources";
        CCFileUtils::get()->addSearchPath(searchPathRoot.string().c_str());
        m_fields->m_outlineSpr = CCSprite::create("gdlogo-outline.png"_spr);
        m_fields->m_outlineSpr->setPosition(size / 2);
        m_fields->m_outlineSpr->setColor({50, 50, 50});
        this->addChild(m_fields->m_outlineSpr);
        auto clippingNode = CCClippingNode::create();
        clippingNode->setContentSize({100, 100});
        clippingNode->setAnchorPoint({0, 0});
        clippingNode->setPosition(size / 2);

        auto mask2 = CCSprite::create("gdlogo-fill.png"_spr);
        mask2->setAnchorPoint({0.5, 0.5});
        //mask2->setRotation(45);
        //mask2->setContentSize(m_fields->clippingNode->getContentSize());
        clippingNode->setStencil(mask2);
        clippingNode->setAlphaThreshold(0.8f);
        clippingNode->setZOrder(1);
        this->addChild(clippingNode);

        m_fields->fillSpr = CCSprite::create("square02b_001.png");
        m_fields->fillSpr->setAnchorPoint({0.5, 0.5});
        m_fields->fillSpr->setPosition({-50, -50});
        m_fields->fillSpr->setScale(.15F);
        //m_fields->fillSpr->setScale(2.5F);
        m_fields->fillSpr->setRotation(45);
        clippingNode->addChild(m_fields->fillSpr);

        m_sliderBar->getParent()->setVisible(false);
        //m_textArea->setVisible(false);
        //m_caption->setVisible(false);

        m_caption->setFntFile("chatFont.fnt");
        m_textArea->m_fontFile = "chatFont.fnt";
        
        //this->stopAllActions();
        return true;
    }
    void updateProgress(int progress) {
        //m_textArea->setVisible(false);
        //m_caption->setVisible(false);
        m_caption->setFntFile("chatFont.fnt");
        m_textArea->m_fontFile = "chatFont.fnt";
        LoadingLayer::updateProgress(progress);
        // 3.4F 

        float progressScale = 0.15F + 3.25F * ((float)progress / 100);
        if (m_fields->fillSpr != nullptr) {
            m_fields->fillSpr->setScale(progressScale);
        }
    }
    void loadAssets() {
        if ((m_loadStep < 14 && !m_fromRefresh) || m_fields->playedAnimation) return LoadingLayer::loadAssets();
        //m_loadStep += 1;
        auto director = CCDirector::sharedDirector();
        auto appDelegate = AppDelegate::get();
        director->setSceneDelegate(appDelegate);
        auto GM = GameManager::sharedState();
        if (GM->getGameVariable("0115")) {
            director->toggleShowFPS(true, "chatFont.fnt", {0, 0});
        }
        director->setFastMenu(GM->getGameVariable("0168"));
        GM->updateCustomFPS();

#ifdef GEODE_IS_ANDROID
        appDelegate->loadingIsFinished();
#endif

        if (!m_fromRefresh) {
            //GM->updateMusic();
            //dumb inlined func!
            if (auto fmod = FMODAudioEngine::sharedEngine()) {
                fmod->setBackgroundMusicVolume(GM->m_bgVolume);
                fmod->setEffectsVolume(GM->m_sfxVolume);
                fmod->m_musicOffset = GM->m_timeOffset;
            }
            //GM->fadeInMenuMusic();
#ifdef GEODE_IS_ANDROID
            GM->syncPlatformAchievements(); // this is apparently android only
#endif
        }

        m_fields->playedAnimation = true;
        // NOW FOR THE AMAZING PART!
        if (!Mod::get()->getSettingValue<bool>("skip-intro")) {
            introAnimation();
        } else {
            LoadingLayer::loadAssets();
        }


        //m_fields->m_gdLogo

        //return LoadingLayer::loadAssets();
        // this is the part where the DUMB INLINED FUNC EXISTS 
        // loadingFinished();
        
        
        //director->replaceScene(MenuLayer::scene(m_fromRefresh));
    }
    /*
    // smh inlined
    void loadingFinished() {
        if (m_fromRefresh) {
            LoadingLayer::loadingFinished();
        } else {
            CCDirector::sharedDirector()->replaceScene(MenuLayer::scene(true));
        }
    }
*/
};
