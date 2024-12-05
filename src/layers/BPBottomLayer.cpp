#include "BPBottomLayer.hpp"
#include "BigPictureLayer.hpp"

bool BPBottomLayer::init() {
	if (!CCLayer::init()) return false;
    setContentHeight(30);
    auto size = CCDirector::sharedDirector()->getWinSize();
    m_buttonMenu = CCMenu::create();
    m_buttonMenu->setID("button-menu"_spr);

    auto bottomMenuBG = CCScale9Sprite::create("square02_001.png");
    bottomMenuBG->setID("bg"_spr);
    bottomMenuBG->setPositionY(15);
    bottomMenuBG->setAnchorPoint({0.5, 0.5});
    //bottomMenuBG->setOpacity(100);
    bottomMenuBG->setContentSize({ size.width + 20, 30 });

    m_buttonMenu->setPosition(bottomMenuBG->getPosition());
    m_buttonMenu->setAnchorPoint(bottomMenuBG->getAnchorPoint());
    m_buttonMenu->setContentSize(bottomMenuBG->getContentSize());

    auto menuBtn = CCScale9Sprite::create("square04_001.png");
    menuBtn->setContentSize({180, 65});
    menuBtn->setScale(0.3F);
    auto menuBtnIcon = CCLabelBMFont::create("START", "NunitoBold.fnt"_spr);
    menuBtnIcon->setScale(2.5F);
    menuBtnIcon->setColor({0, 0, 0});
    menuBtn->addChildAtPosition(menuBtnIcon, geode::Anchor::Center);
    

    m_menuBtn = CCMenuItemExt::createSpriteExtra(menuBtn, [this](CCObject*) {
        if (auto menuLayer = MenuLayer::get()) {
            if (auto bigPicture = menuLayer->getChildByType<BigPictureLayer>(0)) {
                if (bigPicture->isVisible()) {
                    bigPicture->keyPressDown(cocos2d::CONTROLLER_Back);
                }
            }
        }
    });

    m_buttonMenu->addChildAtPosition(m_menuBtn, geode::Anchor::Left, {50, 0});
    m_menuBtnLbl = CCLabelBMFont::create("MENU", "Nunito.fnt"_spr);
    m_menuBtnLbl->setScale(0.65F);
    m_menuBtnLbl->setAnchorPoint({0.5, 0});
    bottomMenuBG->addChildAtPosition(m_menuBtnLbl, Anchor::Left, {100, 0});

    // ----

    auto optBtn = CCScale9Sprite::create("square04_001.png");
    optBtn->setContentSize({130, 65});
    optBtn->setScale(0.3F);

    auto optBtnIcon = CCSprite::create("feather.png"_spr);
    optBtnIcon->setColor({0,0,0});
    optBtnIcon->setScale(1.25F);
    optBtnIcon->setColor({0, 0, 0});
    optBtn->addChildAtPosition(optBtnIcon, geode::Anchor::Center);
    
    m_optBtn = CCMenuItemExt::createSpriteExtra(optBtn, [](CCObject*) {
        CCKeyboardDispatcher::get()->dispatchKeyboardMSG(enumKeyCodes::CONTROLLER_Start, true, false);
        CCKeyboardDispatcher::get()->dispatchKeyboardMSG(enumKeyCodes::CONTROLLER_Start, false, false);
    });
    m_buttonMenu->addChildAtPosition(m_optBtn, geode::Anchor::Right, {-255, 0});
    m_optBtnLbl = CCLabelBMFont::create("OPTIONS", "Nunito.fnt"_spr);
    m_optBtnLbl->setScale(0.65F);
    m_optBtnLbl->setAnchorPoint({0.5, 0});
    bottomMenuBG->addChildAtPosition(m_optBtnLbl, Anchor::Right, {-205, 0});

    // ----
    
    auto selBtn = CCSprite::create("circle.png");
    selBtn->setScale(1.25F);
    auto selBtnIcon = CCLabelBMFont::create("A", "NunitoBold.fnt"_spr);
    selBtnIcon->setColor({0, 0, 0});
    selBtnIcon->setScale(0.8F);
    selBtn->addChildAtPosition(selBtnIcon, geode::Anchor::Center);
    
    m_selBtn = CCMenuItemExt::createSpriteExtra(selBtn, [](CCObject*) {
        CCKeyboardDispatcher::get()->dispatchKeyboardMSG(enumKeyCodes::CONTROLLER_A, true, false);
        CCKeyboardDispatcher::get()->dispatchKeyboardMSG(enumKeyCodes::CONTROLLER_A, false, false);
    });
    m_buttonMenu->addChildAtPosition(m_selBtn, geode::Anchor::Right, {-100, 0});
    m_selBtnLbl = CCLabelBMFont::create("SELECT", "Nunito.fnt"_spr);
    m_selBtnLbl->setScale(0.65F);
    m_selBtnLbl->setAnchorPoint({0.5, 0});
    bottomMenuBG->addChildAtPosition(m_selBtnLbl, Anchor::Right, {-65, 0});

    // ----
    
    auto backBtn = CCSprite::create("circle.png");
    backBtn->setScale(1.25F);
    auto backBtnIcon = CCLabelBMFont::create("B", "NunitoBold.fnt"_spr);
    backBtnIcon->setColor({0, 0, 0});
    backBtnIcon->setScale(0.8F);
    backBtn->addChildAtPosition(backBtnIcon, geode::Anchor::Center);

    m_backBtn = CCMenuItemExt::createSpriteExtra(backBtn, [](CCObject*) {
        CCKeyboardDispatcher::get()->dispatchKeyboardMSG(enumKeyCodes::CONTROLLER_B, true, false);
        CCKeyboardDispatcher::get()->dispatchKeyboardMSG(enumKeyCodes::CONTROLLER_B, false, false);
    });
    m_buttonMenu->addChildAtPosition(m_backBtn, geode::Anchor::Right, {-80, 0});
    m_backBtnLbl = CCLabelBMFont::create("BACK", "Nunito.fnt"_spr);
    m_backBtnLbl->setScale(0.65F);
    m_backBtnLbl->setAnchorPoint({0.5, 0});
    bottomMenuBG->addChildAtPosition(m_backBtnLbl, Anchor::Right, {-50, 0});

    // ---

    bottomMenuBG->setPositionX(size.width / 2);
    m_buttonMenu->setPositionX(size.width / 2);

    this->addChild(bottomMenuBG);
    this->addChild(m_buttonMenu);

    this->setID("BPBottomLayer"_spr);

    changeButtonState(3);
    return true;
}

BPBottomLayer* BPBottomLayer::create() {
	auto pRet = new BPBottomLayer();
	if (pRet && pRet->init()) {
		pRet->autorelease();
		return pRet;
	}
	CC_SAFE_DELETE(pRet);
	return nullptr;
}
