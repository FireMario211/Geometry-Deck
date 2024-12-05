#include "BPAlert.hpp"
#include "Geode/utils/cocos.hpp"

bool BPAlert::init(std::string title, std::vector<BigPicButton> options) {
    if (!this->initWithColor({ 0, 0, 0, 105 })) return false;
    options.push_back({ "Cancel", [this]() { onClose(nullptr); } });
    m_options = options;
    this->m_mainLayer = cocos2d::CCLayer::create();
    this->addChild(this->m_mainLayer);
    this->m_buttonMenu = CCMenu::create();
    this->m_mainLayer->addChild(this->m_buttonMenu);
    m_buttonMenu->setContentWidth(150);

    auto titleLabel = CCLabelBMFont::create(title.c_str(), "Nunito.fnt"_spr);
    titleLabel->limitLabelWidth(100, 1.0F, 0.5F);
    m_buttonMenu->addChild(titleLabel);
    
    for (int i = 0; i < m_options.size(); i++) {
        auto option = m_options[i];
        auto bg = CCScale9Sprite::create("square.png");
        bg->setColor({20, 20, 33});
        bg->setContentSize({150, 25});
        auto label = CCLabelBMFont::create(option.name.c_str(), "Nunito.fnt"_spr);
        label->setColor({200, 200, 200});
        label->setScale(0.65F);
        label->setAnchorPoint({0, 0.1});
        bg->addChildAtPosition(label, Anchor::Left, {10, 0});
        auto menuItem = CCMenuItemExt::createSpriteExtra(bg, [this, option](CCObject*) {
            option.callback();
            onClose(nullptr);
        });
        menuItem->setTag(i);
        m_buttonMenu->addChild(menuItem);
    }
    m_buttonMenu->setLayout(
        RowLayout::create()
        ->setGap(0)
        ->setGrowCrossAxis(true)
        //->setCrossAxisOverflow(false)
        ->setCrossAxisOverflow(true)
        ->setAutoScale(false)
    );
    titleLabel->setPositionY(titleLabel->getPositionY() + 15.F);

    this->setKeypadEnabled(true);
    this->registerWithTouchDispatcher();
    this->setKeyboardEnabled(true);
    this->setTouchEnabled(true);

    cocos::handleTouchPriority(this);

    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(1.5F), CCCallFunc::create(this, callfunc_selector(BPAlert::willTouch))));
    return true;
}

void BPAlert::setSelected(int tag, bool selected) {
    if (auto menuItem = as<CCMenuItemSpriteExtra*>(m_buttonMenu->getChildByTag(tag))) {
        if (auto bg = menuItem->getChildByType<CCScale9Sprite>(0)) {
            if (auto label = bg->getChildByType<CCLabelBMFont>(0)) {
                if (tag >= m_options.size()) return;
                if (!selected) {
                    bg->setColor({20, 20, 33});
                    label->setColor({200, 200, 200});
                } else {
                    bg->setColor(m_options[tag].selectedBG);
                    label->setColor(m_options[tag].selectedText);
                }
            }
        }
    }
}
void BPAlert::enterSelected() {
    if (currentlySelected == -1) return;
    if (currentlySelected >= m_options.size()) return;
    m_options[currentlySelected].callback();
    onClose(nullptr);
}

void BPAlert::onClose(cocos2d::CCObject* pSender) {
    this->setKeyboardEnabled(false);
    this->removeFromParentAndCleanup(true);
};

void BPAlert::ccTouchEnded(cocos2d::CCTouch *touch, cocos2d::CCEvent *p1) {
    if (!canTouch) return;
    FLAlertLayer::ccTouchEnded(touch, p1);
    onClose(nullptr);
}

void BPAlert::keyDown(cocos2d::enumKeyCodes key) {
    if (key == cocos2d::enumKeyCodes::KEY_Escape || key == cocos2d::enumKeyCodes::CONTROLLER_B)
        return onClose(nullptr);
    if (key == enumKeyCodes::KEY_Enter || key == enumKeyCodes::CONTROLLER_A)
        return enterSelected();
    if (key == enumKeyCodes::KEY_Up || key == enumKeyCodes::KEY_ArrowUp || key == enumKeyCodes::CONTROLLER_Up) {
        if (currentlySelected == -2) {
            currentlySelected = m_options.size() - 1;
            setSelected(currentlySelected, true);
        } else {
            setSelected(currentlySelected, false);
            currentlySelected--;
            if (currentlySelected < 0) currentlySelected = m_options.size() - 1;
            setSelected(currentlySelected, true);
        }
        return;
    } else if (key == enumKeyCodes::KEY_Down || key == enumKeyCodes::KEY_ArrowDown || key == enumKeyCodes::CONTROLLER_Down) {
        if (currentlySelected == -2) {
            currentlySelected = 0;
            setSelected(currentlySelected, true);
        } else {
            setSelected(currentlySelected, false);
            currentlySelected++;
            if (currentlySelected > (m_options.size() - 1)) currentlySelected = 0;
            setSelected(currentlySelected, true);
        }
        return;
    }    
}

BPAlert* BPAlert::create(std::string title, std::vector<BigPicButton> options) {
    auto pRet = new BPAlert();
    if (pRet) {
        if (pRet->init(title, options)) {
            pRet->autorelease();
            return pRet;
        }
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
};
