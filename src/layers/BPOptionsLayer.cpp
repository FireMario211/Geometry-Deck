#include "BPOptionsLayer.hpp"
#include "../nodes/BreakLine.hpp"
#include "BPAlert.hpp"

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
void BPOptionsLayer::setSelected() {
    if (auto menuItem = as<CCMenuItemSpriteExtra*>(m_scrollContent->getChildByTag(currentSelIndex))) {
        m_selectionBG->setPositionY(menuItem->getPositionY() - 35);
        if (currentSelIndex < 5) {
            m_scrollLayer->moveToTop();
        } else {
            m_scrollLayer->m_contentLayer->setPositionY(((currentSelIndex) * 50.F) - 650.F);
        }/* else if (currentSelIndex > m_categories[sideMenuIndex].options.size() - 4){
            m_scrollLayer->m_contentLayer->setPositionY(-m_scrollLayer->m_contentLayer->getContentHeight() + m_scrollLayer->getContentHeight());
        } else {
        }*/
    }
}

namespace GMDS_Dictionary {
    float getFloatForKey(GameManager* GM, std::string id) {
        if (auto fmod = FMODAudioEngine::sharedEngine()) {
            if (id == "bgVolume") return fmod->getBackgroundMusicVolume();
            if (id == "sfxVolume") return fmod->getEffectsVolume();
            if (id == "timeOffset") return GM->m_timeOffset;
            if (id == "customFPSTarget") return GM->m_customFPSTarget;
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
            switch (option.type) {
                case BPOptionType::Toggle: {
                    if (auto menuItem = menu->getChildByType<CCMenuItemSpriteExtra>(0)) {
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
                        bool newValue = (option.definedValue.asBool().unwrapOrDefault() == true) ? value.asBool().unwrapOrDefault() : !value.asBool().unwrapOrDefault();
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
                            GM->setGameVariable(option.id.c_str(), newValue);
                            option.callback(GM->getGameVariable(option.id.c_str()));
                        }
                    }
                    break;
                }
                case BPOptionType::Dropdown: {
                    std::vector<matjson::Value> emptyArray;
                    auto array = option.definedValue.asArray().unwrapOr(emptyArray);
                    if (auto bg = as<CCScale9Sprite*>(menu->getChildByID("Button-BG"))) {
                        if (auto label = bg->getChildByType<CCLabelBMFont>(0)) {
                            label->setString(array[value.asInt().unwrapOrDefault()].asString().unwrapOrDefault().c_str());
                        }
                    }
                    option.callback(value);
                    break;
                }
                case BPOptionType::Input: {
                    int actualValue = 0;
                    auto str = value.asString().unwrapOrDefault();
                    if (!str.empty()) actualValue = std::stoi(str);
                    option.callback(actualValue);
                    break;
                }
                default: {
                    option.callback(value);
                    break;
                }
            }
        }
    }
}



void BPOptionsLayer::onSliderChange(CCObject* sender) {
    // nesting hell
    if (auto sliderThumb = typeinfo_cast<SliderThumb*>(sender)) {
        if (auto sliderTouchLogic = typeinfo_cast<SliderTouchLogic*>(sliderThumb->getParent())) {
            if (auto slider = typeinfo_cast<Slider*>(sliderTouchLogic->getParent())) {
                if (auto menu = typeinfo_cast<CCMenu*>(slider->getParent())) {
                    if (auto bar = typeinfo_cast<CCScale9Sprite*>(menu->getChildByID("Bar-BG"))) {
                        if (auto fill = bar->getChildByType<CCScale9Sprite>(0)) {
                            float value = sliderThumb->getValue();
                            float normalizedValue = std::clamp((value + 0.24999996f) / (1.2499999f + 0.24999996f), 0.0F, 1.0F);
                            fill->setScaleX(normalizedValue * 37.F);
                            int index = menu->getTag();
                            // probably will result in undefined behavior if this is wrong, hA!
                            auto option = m_categories[currentSelIndex + 1].options[index];
                            option.callback((double)normalizedValue);
                        }
                    }
                }
            }
        }
    }
}

void BPOptionsLayer::regenCategory(BPCategory category) {
    m_scrollContent->removeAllChildrenWithCleanup(true);
    auto titleLabel = CCLabelBMFont::create(category.name, "NunitoBold.fnt"_spr);
    titleLabel->limitLabelWidth(100, 1.2F, 1.0F);
    m_scrollContent->addChild(titleLabel);
    auto colorAccent = Mod::get()->getSettingValue<ccColor3B>("color-accent");
    m_scrollLayer->m_contentLayer->setContentHeight(category.options.size() * 60.F);
    m_scrollContent->setContentHeight(m_scrollLayer->m_contentLayer->getContentHeight());
    auto GM = GameManager::sharedState();
    for (int i = 0; i < category.options.size(); i++) {
        auto option = category.options[i];
        auto node = CCMenu::create();
        auto label = CCLabelBMFont::create(option.name, "Nunito.fnt"_spr);
        if (option.type == BPOptionType::Title) {
            label->setColor({200, 200, 200});
        }
        label->setScale(0.8F);
        CCMenuItemSpriteExtra* button;
        node->setContentSize({400, 30});
        switch (option.type) {
            case BPOptionType::Toggle: {
                bool value;
                switch (option.valueType) {
                    case BPValueType::Geode:
                        value = Mod::get()->getSettingValue<bool>(option.id);
                        break;
                    case BPValueType::IntVar:
                        value = (option.definedValue.asBool().unwrapOrDefault() == true) ? !GM->getGameVariable(option.id.c_str()) : GM->getGameVariable(option.id.c_str());
                        break;
                    case BPValueType::DSDict:
                        value = GMDS_Dictionary::getBoolForKey(GM, option.id);
                        break;
                }
                auto bg = CCScale9Sprite::create("square04_001.png");
                auto circle = CCSprite::create("circle.png"_spr);
                circle->setScale(3.75F);
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
            case BPOptionType::Slider: {
                auto accompanySpr = CCSprite::create(option.definedValue.asString().unwrapOrDefault().c_str());
                accompanySpr->setScale((20.F / accompanySpr->getContentHeight())); // 0.3F
                accompanySpr->setAnchorPoint({0, 0});
                node->addChildAtPosition(accompanySpr, Anchor::BottomLeft);
                auto bar = CCScale9Sprite::create("square.png");
                bar->setColor({30, 30, 40});
                bar->setAnchorPoint({0.5, 0});
                bar->setContentSize({370, 10}); // 325, 1.6F
                bar->setID("Bar-BG");
                auto barFill = CCScale9Sprite::create("square.png");
                float currValue = GMDS_Dictionary::getFloatForKey(GM, option.id);
                barFill->setScaleX(currValue * 37.F);
                barFill->setContentSize({10, 10});
                barFill->setColor(colorAccent);
                barFill->setAnchorPoint({0, 0.5});
                barFill->setID("Fill-BG");
                //auto sel = CCSprite::create("circle.png"_spr);
                auto slider = Slider::create(this, menu_selector(BPOptionsLayer::onSliderChange), "sliderBar.png", "square.png", "circle.png"_spr, "circle.png"_spr, 1.8F);
                slider->setValue(currValue);
                slider->getThumb()->setScale(1.2F);
                slider->hideGroove(true);
                slider->setBarVisibility(true);
                //slider->setAnchorPoint({1, 0.5});
                node->addChildAtPosition(bar, Anchor::Bottom, {15, 5});
                bar->addChildAtPosition(barFill, Anchor::Left, {0, 0});
                node->addChildAtPosition(slider, Anchor::Bottom, {15, 10});
                node->setContentHeight(40);
                break;
            }
            case BPOptionType::Input: {
                auto input = TextInput::create(100.F, "...", "Nunito.fnt"_spr);
                int value;
                switch (option.valueType) {
                    case BPValueType::Geode:
                        value = (int)Mod::get()->getSettingValue<double>(option.id);
                        break;
                    case BPValueType::IntVar:
                        value = GM->getIntGameVariable(option.id.c_str());
                        break;
                    case BPValueType::DSDict:
                        value = GMDS_Dictionary::getFloatForKey(GM, option.id);
                        break;
                }
                input->setCallback([option, this](const std::string str) {
                    updateOption(option, str);
                });
                input->setString(std::to_string(value));
                input->setFilter("0123456789");
                input->setMaxCharCount(20);
                input->setAnchorPoint({1.0, 0.5});
                node->addChildAtPosition(input, Anchor::Right);
                break;
            }
            case BPOptionType::Dropdown: {
                std::vector<matjson::Value> emptyArray;
                auto array = option.definedValue.asArray().unwrapOr(emptyArray);
                std::vector<BigPicButton> dropdown_options;
                int value;
                switch (option.valueType) {
                    case BPValueType::Geode:
                        value = Mod::get()->getSettingValue<int64_t>(option.id);
                        break;
                    case BPValueType::IntVar:
                        value = GM->getIntGameVariable(option.id.c_str());
                        break;
                    case BPValueType::DSDict:
                        value = GMDS_Dictionary::getIntegerForKey(GM, option.id);
                        break;
                }
                for (int i = 0; i < array.size(); i++) {
                    BigPicButton doption = {
                        array[i].asString().unwrapOrDefault(),
                        [i, option, this]() {
                            updateOption(option, i);
                        }
                    };
                    dropdown_options.push_back(doption);
                }
                if (value < 0) continue;
                if (value >= array.size()) continue;
                auto str = array[value];
                auto bg = CCScale9Sprite::create("square.png");
                bg->setColor({30, 30, 40});
                auto label = CCLabelBMFont::create(str.asString().unwrapOrDefault().c_str(), "Nunito.fnt"_spr);
                label->setScale(0.75F);
                label->setAnchorPoint({0, 0.5});
                bg->setContentSize({label->getScaledContentWidth() + 100.F, 20});
                bg->setAnchorPoint({1, 0.5});
                bg->addChildAtPosition(label, Anchor::Left, {10, 4});
                button = CCMenuItemExt::createSpriteExtraWithFilename("dropdown.png"_spr, 0.5F, [dropdown_options](CCObject*) {
                    BPAlert::create("", dropdown_options)->show();
                });
                bg->setID("Button-BG");
                node->addChildAtPosition(bg, Anchor::Right, {0, 0});
                break;
            }
            case BPOptionType::Title: break;
            default:
                continue;
        }
        if (option.type != BPOptionType::Title && option.type != BPOptionType::Slider && option.type != BPOptionType::Input && button != nullptr) {
            button->setAnchorPoint({1, 0.5});
            node->addChildAtPosition(button, Anchor::Right, {-15, 0});
        }
        label->setAnchorPoint({0, 0.5});
        if (option.type == BPOptionType::Slider) {
            node->addChildAtPosition(label, Anchor::Left, {0, 15});
        } else {
            node->addChildAtPosition(label, Anchor::Left, {0, 5});
        }
        node->setAnchorPoint({0, 0.5});
        node->setID(option.id);
        node->setTag(i);
        node->updateLayout();
        // i * 40
        m_scrollContent->addChild(node);
        if ((i + 1) < category.options.size() && category.options[i].type != BPOptionType::Title) {
            m_scrollContent->addChild(BreakLine::create(400.F));
        }
    }
    m_scrollContent->updateLayout();
    m_scrollLayer->moveToTop();
    m_scrollLayer->m_contentLayer->removeChildByID("selection-bg");
    m_selectionBG = CCScale9Sprite::create("square.png");
    m_selectionBG->setColor({64, 64, 76});
    m_selectionBG->setAnchorPoint({0, 0.5});
    m_selectionBG->setID("selection-bg");
    m_selectionBG->setZOrder(-1);
    m_selectionBG->setVisible(false);
    if (auto m = as<CCMenu*>(m_scrollContent->getChildByTag(0))) {
        m_scrollLayer->m_contentLayer->addChild(m_selectionBG);
        m_selectionBG->setPositionY(m->getPositionY() - 35);
        m_selectionBG->setContentSize({435, m->getContentHeight() + 10.F});
        m_selectionBG->runAction(CCRepeatForever::create(
            CCSequence::createWithTwoActions(CCFadeTo::create(1.0F, 255 * 0.1), CCFadeTo::create(1.0F, 255))
        ));
    }
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
        } else {
            currentSelIndex--;
            if (currentSelIndex < 0) currentSelIndex = m_categories[sideMenuIndex].options.size();
            setSelected();
        }
        return true;
    } else if (key == enumKeyCodes::KEY_Down || key == enumKeyCodes::KEY_ArrowDown || key == enumKeyCodes::CONTROLLER_Down) {
        if (isOnSide) {
            sideMenuIndex++;
            if (sideMenuIndex > (m_categories.size() - 1)) sideMenuIndex = 0;
            setSMSelected();
        } else {
            currentSelIndex++;
            if (currentSelIndex > m_categories[sideMenuIndex].options.size() - 1) currentSelIndex = 0;
            setSelected();
        }
        return true;
    } else if (key == enumKeyCodes::KEY_Right || key == enumKeyCodes::KEY_ArrowRight || key == enumKeyCodes::CONTROLLER_Right) {
        if (isOnSide) {
            isOnSide = false;
            m_selectionBG->setVisible(true);
        }
        return true;
    } else if (key == enumKeyCodes::KEY_Left || key == enumKeyCodes::KEY_ArrowLeft || key == enumKeyCodes::CONTROLLER_Left) {
        if (!isOnSide) {
            isOnSide = true;
            m_selectionBG->setVisible(false);
        }
        return true;
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
