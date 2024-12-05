#include "BPOptionsLayer.hpp"
#include "../nodes/BreakLine.hpp"

bool BPOptionsLayer::init(std::vector<BPCategory> categories) {
    if (!CCLayerGradient::initWithColor(ccc4(11, 11, 26, 255), ccc4(22, 22, 37, 255))) return false;
    m_categories = categories;
    auto size = CCDirector::sharedDirector()->getWinSize();
    auto sideBG = CCScale9Sprite::create("square.png");
    sideBG->setColor({30, 30, 40});
    sideBG->setContentSize({135, this->getContentHeight()});
    sideBG->setAnchorPoint({0, 0.5});


    m_scrollLayer = ScrollLayerExt::create({ 0, 0, size.width - 135, this->getContentHeight() }, true);
    m_scrollLayer->setAnchorPoint({0.5, 0.5});
    m_scrollLayer->setPosition({135, 0});
    m_scrollContent = CCMenu::create();
    m_scrollContent->setZOrder(2);
    m_scrollContent->setContentWidth(m_scrollLayer->getContentWidth() - 15.F);
    m_scrollContent->setAnchorPoint({0, 0});
    m_scrollLayer->m_contentLayer->setContentSize({
        m_scrollContent->getContentSize().width,
        //levels.size() * 240.F
        10 * 115.F
    });
    m_scrollContent->setContentHeight(m_scrollLayer->m_contentLayer->getContentHeight());
    m_scrollContent->registerWithTouchDispatcher();

    m_scrollLayer->m_contentLayer->addChild(m_scrollContent);
    m_scrollLayer->setTouchEnabled(true);

    m_sideMenu = CCMenu::create();
    m_sideMenu->setContentSize({135, this->getContentHeight()});
    m_sideMenu->setAnchorPoint({0, 0.5});

    sideBG->setPositionY(size.height / 2);

    for (int i = 0; i < categories.size(); i++) {
        auto category = categories[i];
        auto node = CCNode::create();
        auto icon = CCSprite::create(category.spriteName);
        auto label = CCLabelBMFont::create(category.name, "Nunito.fnt"_spr);
        label->limitLabelWidth(60, 1.0F, 0.6F);
        label->setAnchorPoint({0, 0});
        node->setContentSize({100, 15});
        node->setPositionX(-5);
        node->setAnchorPoint({0.5, 0.5});
        icon->setScale(node->getScaledContentHeight() / icon->getScaledContentHeight());
        node->addChildAtPosition(icon, Anchor::Left);
        node->addChildAtPosition(label, Anchor::Center, {-35, 0});
        auto menuItem = CCMenuItemExt::createSpriteExtra(node, [this, i](CCObject* sender) {
            sideMenuIndex = i;
            setSMSelected();
        });
        menuItem->setAnchorPoint({0, 0.5});
        menuItem->setTag(i);
        m_sideMenu->addChild(menuItem);
    }
    regenCategory(categories[0]);

    m_sideSelectionBG = CCScale9Sprite::create("square.png");
    m_sideSelectionBG->setColor({60, 60, 70});
    m_sideSelectionBG->setContentSize({135, 22});
    m_sideSelectionBG->setAnchorPoint({0, 0.5});
    m_sideSelectionBG->setZOrder(-1);
    m_sideMenu->addChild(m_sideSelectionBG);

    m_sideMenu->setLayout(RowLayout::create()
        ->setAxisAlignment(AxisAlignment::Center)
        ->setAutoScale(true)
        ->setCrossAxisOverflow(false)
        ->setGrowCrossAxis(true)
        ->setCrossAxisAlignment(AxisAlignment::End)
        ->setCrossAxisReverse(false)
        ->setGap(10)
    );
    m_scrollContent->setLayout(RowLayout::create()
        ->setAxisAlignment(AxisAlignment::Start)
        ->setAutoScale(true)
        ->setCrossAxisOverflow(false)
        ->setGrowCrossAxis(true)
        ->setCrossAxisAlignment(AxisAlignment::End)
        ->setCrossAxisReverse(false)
        ->setGap(5)
    );
    m_scrollContent->setPosition({15, -35});

    if (auto m = as<CCMenuItemSpriteExtra*>(m_sideMenu->getChildByTag(0))) {
        m_sideSelectionBG->setPositionY(m->getPositionY());
    }

    m_sideMenu->setPosition({0, m_sideMenu->getPositionY() - 5});
    this->addChild(sideBG);
    this->addChild(m_sideMenu);
    if (auto menuLayer = MenuLayer::get()) {
        if (auto bigPicture = menuLayer->getChildByType<BigPictureLayer>(0)) {
            bigPicture->toggleBar(false);
            bigPicture->toggleBarChangeTop(true);
        }
    }

    this->addChild(m_scrollLayer);

    this->setTouchEnabled(true);
    this->registerWithTouchDispatcher();

    handleTouchPriority(this);
    //m_scrollLayer->fixTouchPrio();
    return true;
}

/*
void BPBottomLayer::enterSelected() {
    if (auto menuItem = as<CCMenuItemSpriteExtra*>(m_sideMenu->getChildByTag(currentIndexSelection))) {
        menuItem->activate();
    }
}
*/
void BPOptionsLayer::setSMSelected() {
    if (auto menuItem = as<CCMenuItemSpriteExtra*>(m_sideMenu->getChildByTag(sideMenuIndex))) {
        m_sideSelectionBG->setPositionY(menuItem->getPositionY());
        regenCategory(m_categories[sideMenuIndex]);
    }
}

namespace GMDS_Dictionary {
    float getFloatForKey(GameManager* GM, std::string id) {
        if (auto fmod = FMODAudioEngine::sharedEngine()) {
            if (id == "bgVolume") return fmod->getBackgroundMusicVolume();
            if (id == "sfxVolume") return fmod->getEffectsVolume();
        }
        return 0;
    }
    int getIntegerForKey(GameManager* GM, std::string id) {
        if (id == "resolution") return GM->m_resolution;
        if (id == "texQuality") return GM->m_texQuality;
        return 0;
    }
    bool getBoolForKey(GameManager* GM, std::string id) {
        if (id == "performanceMode") return GM->m_performanceMode;
        return false;
    }
};

void BPOptionsLayer::updateOption(BPOption option, matjson::Value value) {
    if (auto GM = GameManager::sharedState()) {
        if (auto menu = as<CCMenu*>(m_scrollContent->getChildByID(option.id))) {
            if (auto menuItem = menu->getChildByType<CCMenuItemSpriteExtra>(0)) {
                switch (option.type) {
                    case BPOptionType::Toggle: {
                        switch (option.valueType) {
                            case BPValueType::Geode:
                                value = Mod::get()->getSettingValue<bool>(option.id);
                                break;
                            case BPValueType::IntVar:
                                value = GM->getGameVariable(option.id.c_str());
                                break;
                            case BPValueType::DSDict:
                                value = GMDS_Dictionary::getBoolForKey(GM, option.id);
                                break;
                        }
                        bool newValue = (option.definedValue.asBool().unwrapOrDefault()) ? value.asBool().unwrapOrDefault() : !value.asBool().unwrapOrDefault();
                        auto colorAccent = Mod::get()->getSettingValue<ccColor3B>("color-accent");
                        if (auto bg = as<CCScale9Sprite*>(menuItem->getChildByID("Toggle-BG"))) {
                            if (auto circle = bg->getChildByType<CCSprite>(0)) {
                                float pickPos = (newValue) ? -30 : 30;
                                circle->updateAnchoredPosition((newValue) ? Anchor::Right : Anchor::Left, {pickPos, 0});
                                bg->setColor((newValue) ? colorAccent : ccc3(30, 30, 40));
                            }
                        }
                        log::debug("Set {} to {}", option.id, newValue);
                        if (option.valueType == BPValueType::Geode) {
                            Mod::get()->setSettingValue(option.id, newValue);
                            option.callback(newValue);
                        } else if (option.valueType == BPValueType::IntVar) {
                            GM->toggleGameVariable(option.id.c_str());
                            option.callback(GM->getGameVariable(option.id.c_str()));
                        }
                        break;
                    }
                    case BPOptionType::Slider: {
                        auto colorAccent = Mod::get()->getSettingValue<ccColor3B>("color-accent");
                        /*if (auto bg = as<CCScale9Sprite*>(menuItem->getChildByID("Toggle-BG"))) {
                            if (auto circle = bg->getChildByType<CCSprite>(0)) {
                                float pickPos = (newValue) ? -30 : 30;
                                circle->updateAnchoredPosition((newValue) ? Anchor::Right : Anchor::Left, {pickPos, 0});
                                bg->setColor((newValue) ? colorAccent : ccc3(30, 30, 40));
                            }
                        }
                        log::debug("Set {} to {}", option.id, newValue);
                        if (option.valueType == BPValueType::Geode) {
                            Mod::get()->setSettingValue(option.id, newValue);
                            option.callback(newValue);
                        } else if (option.valueType == BPValueType::IntVar) {
                            GM->toggleGameVariable(option.id.c_str());
                            option.callback(GM->getGameVariable(option.id.c_str()));
                        }*/
                        break;
                    }
                    default: {
                        return;
                    }
                }
            }
        }
    }
}

void BPOptionsLayer::regenCategory(BPCategory category) {
    m_scrollContent->removeAllChildrenWithCleanup(true);
    auto titleLabel = CCLabelBMFont::create(category.name, "NunitoBold.fnt"_spr);
    titleLabel->limitLabelWidth(60, 1.2F, 1.0F);
    m_scrollContent->addChild(titleLabel);
    auto colorAccent = Mod::get()->getSettingValue<ccColor3B>("color-accent");
    m_scrollLayer->m_contentLayer->setContentHeight(category.options.size() * 75.F);
    m_scrollContent->setContentHeight(m_scrollLayer->m_contentLayer->getContentHeight());
    auto GM = GameManager::sharedState();
    for (int i = 0; i < category.options.size(); i++) {
        auto option = category.options[i];
        auto node = CCMenu::create();
        auto label = CCLabelBMFont::create(option.name, "Nunito.fnt"_spr);
        label->setScale(0.8F);
        CCMenuItemSpriteExtra* button;
        switch (option.type) {
            case BPOptionType::Toggle: {
                bool value;
                switch (option.valueType) {
                    case BPValueType::Geode:
                        value = Mod::get()->getSettingValue<bool>(option.id);
                        break;
                    case BPValueType::IntVar:
                        value = !GM->getGameVariable(option.id.c_str());
                        break;
                    case BPValueType::DSDict:
                        value = GMDS_Dictionary::getBoolForKey(GM, option.id);
                        break;
                }
                auto bg = CCScale9Sprite::create("square04_001.png");
                auto circle = CCSprite::create("circle.png");
                circle->setScale(4.0F);
                bg->setColor((value) ? colorAccent : ccc3(30, 30, 40));
                bg->setContentSize({120, 65});
                bg->setScale(0.3F);
                circle->setID("Toggle-Circle");
                float pickPos = (value) ? -30 : 30;
                bg->addChildAtPosition(circle, (value) ? Anchor::Right : Anchor::Left, {pickPos, 0});
                bg->setID("Toggle-BG");
                button = CCMenuItemExt::createSpriteExtra(bg, [this, option](CCObject*) {
                    updateOption(option, nullptr);
                });
                break;
            }
            case BPOptionType::Button: {
                auto bg = CCScale9Sprite::create("square.png");
                bg->setColor({30, 30, 40});
                auto label = CCLabelBMFont::create(option.definedValue.asString().unwrapOrDefault().c_str(), "Nunito.fnt"_spr);
                label->setScale(0.8F);
                bg->setContentSize({label->getScaledContentWidth() + 25.F, 20});
                bg->setID("Button-BG");
                bg->addChildAtPosition(label, Anchor::Center, {0, 4});
                button = CCMenuItemExt::createSpriteExtra(bg, [option](CCObject*) {
                    option.callback(nullptr);
                });
                break;
            }
            default:
                continue;
        }
        button->setAnchorPoint({1, 0.5});
        label->setAnchorPoint({0, 0.5});
        node->addChildAtPosition(label, Anchor::Left, {0, 5});
        node->addChildAtPosition(button, Anchor::Right, {-15, 0});
        node->setAnchorPoint({0, 0.5});
        node->setContentSize({400, 30});
        node->setID(option.id);
        node->setTag(i);
        node->updateLayout();
        // i * 40
        m_scrollContent->addChild(node);
        if ((i + 1) < category.options.size()) {
            m_scrollContent->addChild(BreakLine::create(400.F));
        }
    }
    m_scrollContent->updateLayout();
    m_scrollLayer->moveToTop();
}

bool BPOptionsLayer::keyPressDown(enumKeyCodes key) {
    if (key == enumKeyCodes::KEY_Escape || key == enumKeyCodes::CONTROLLER_B) {
        killMe();
        return true;
    } else if (key == enumKeyCodes::KEY_Up || key == enumKeyCodes::KEY_ArrowUp || key == enumKeyCodes::CONTROLLER_Up) {
        if (isOnSide) {
            sideMenuIndex--;
            if (sideMenuIndex < 0) sideMenuIndex = m_categories.size() - 1;
            setSMSelected();
        }
        return true;
    } else if (key == enumKeyCodes::KEY_Down || key == enumKeyCodes::KEY_ArrowDown || key == enumKeyCodes::CONTROLLER_Down) {
        if (isOnSide) {
            sideMenuIndex++;
            if (sideMenuIndex > (m_categories.size() - 1)) sideMenuIndex = 0;
            setSMSelected();
            return true;
        }
    }
    return true;
}

BPOptionsLayer* BPOptionsLayer::create(std::vector<BPCategory> categories) {
    auto pRet = new BPOptionsLayer();
    if (pRet) {
        if (pRet->init(categories)) {
            pRet->autorelease();
            return pRet;
        }
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
};
