#include "TopLevels.hpp"
#include "../nodes/LevelItem.hpp"
#include "../utils.hpp"
#include "BPAlert.hpp"
#include "Geode/utils/cocos.hpp"

void TopLevels::showFilter() {
    if (auto GLM = GameLevelManager::sharedState()) {
    CCArrayExt<CCString*> folders = GLM->m_onlineFolders->allKeys();
        std::vector<BigPicButton> folderOpts = {{"Default (All)", [this]() {
            m_label->setString("Saved Levels");
            regenerateLevels(0);
        }}};
        for (auto folder : folders) {
            int folderID = folder->intValue();
            auto folderName = GLM->getFolderName(folderID, false);
            folderOpts.push_back({fmt::format("{}: {}", folderID, folderName), [this, folderName, folderID] {
                m_label->setString(fmt::format("Saved Levels ({})", folderName).c_str());
                regenerateLevels(folderID);
            }});
        }
        BPAlert::create(
            "Select folder", 
            folderOpts
        )->show();
    }
}
bool TopLevels::init(TopLevelsCategory category) {
	if (!CCLayer::init()) return false;
    m_category = category;
    auto size = CCDirector::sharedDirector()->getWinSize();

    this->setPosition({size.width / 2, size.height});
    this->setAnchorPoint({0.5, 1.0});
    this->setContentHeight(120);
    m_buttonMenu = CCMenu::create();

    m_label = CCLabelBMFont::create("Saved Levels", "Nunito.fnt"_spr);
    m_label->setAnchorPoint({0, 0.5});
    m_label->setScale(0.6F);
    m_buttonMenu->setAnchorPoint(this->getAnchorPoint());
    m_buttonMenu->setContentSize(this->getContentSize());
        
    auto filterBtn = CCMenuItemExt::createSpriteExtraWithFrameName("GJ_filterIcon_001.png", 0.45F, [this](CCObject*) {
        showFilter();
    });
    m_buttonMenu->addChildAtPosition(filterBtn, Anchor::Left, {12, 32});
    this->addChildAtPosition(m_label, Anchor::Left, {20, 35});
    this->addChildAtPosition(m_buttonMenu, Anchor::Top);
    m_scrollLayer = ScrollLayerExt::create({ 0, 0, 105.F, size.width }, true);
    m_scrollLayer->setPosition({size.width / 2, -(size.height) - 40});
    m_scrollContent = CCMenu::create();
    m_scrollContent->setZOrder(2);
    m_scrollContent->setContentWidth(150.F);
    m_scrollLayer->setAnchorPoint({0.5, 0});
    m_scrollLayer->m_contentLayer->setContentSize({
        m_scrollContent->getContentSize().width,
        //levels.size() * 240.F
        10 * 115.F
    });
    //m_scrollLayer->m_contentLayer->setRotation(-90);
    m_scrollContent->setContentHeight(m_scrollLayer->m_contentLayer->getContentHeight());
    m_scrollContent->registerWithTouchDispatcher();
    
    m_scrollLayer->m_contentLayer->addChild(m_scrollContent);
    m_scrollLayer->setTouchEnabled(true);
    regenerateLevels();
    /*content->setLayout(
        ColumnLayout::create()
            ->setAxisAlignment(AxisAlignment::Even)
            ->setCrossAxisOverflow(false)
            ->setGap(2)
            ->setGrowCrossAxis(true)
            ->setAutoScale(false)
            ->setAxisReverse(false)
    );*/
    m_scrollContent->setAnchorPoint({0.5, 0});
    m_scrollContent->setPosition({220, 100});

    m_scrollContent->setContentWidth(100);
    this->addChildAtPosition(m_scrollLayer, Anchor::BottomLeft, {-20, 20});
    m_scrollLayer->m_contentLayer->setPositionX(-220);
    m_scrollLayer->m_contentLayer->setPositionY(0);
    m_scrollLayer->setRotation(90);
    m_scrollLayer->fixTouchPrio();
    Loader::get()->queueInMainThread([this]() {
        cocos::handleTouchPriority(m_scrollLayer->m_contentLayer);
    });
        
    this->setID("TopLevels"_spr);
    return true;
}

float calculateContentHeight(int size) {
    switch (size) {
        case 4:
            return size * 157.F;
        case 5:
            return size * 145.F;
        case 6:
            return size * 135.F;
        case 7:
            return size * 125.F;
        case 8:
            return size * 120.F;
        case 9:
        default:
        case 10:
            return size * 115.F;
    }
}

void TopLevels::regenerateLevels() {
    if (auto GLM = GameLevelManager::sharedState()) {
        m_scrollContent->removeAllChildrenWithCleanup(true);
        auto searchObject = GJSearchObject::create(SearchType::SavedLevels);
        searchObject->m_folder = m_folderID;
        CCArrayExt<GJGameLevel*> levels = Utils::shrinkCCArray<GJGameLevel*>(GLM->getSavedLevels(false, searchObject->m_folder), 10);
        bool hasDoneFirst = true;
        currentIndex = 1;
        maxIndex = levels.size();
        m_scrollLayer->m_contentLayer->setContentSize({
            m_scrollContent->getContentSize().width,
            //levels.size() * 240.F
            calculateContentHeight(levels.size())
        });
        m_scrollLayer->setTouchEnabled(levels.size() > 3);
        for (size_t i = 0; i < levels.size(); i++) {
            auto level = levels[i];
            auto item = LevelItem::create(level, i == 0);
            item->setSelected(i == 0);
            if (i == 0) {
                item->setCallback([this](bool val) {
                    if (val) {
                        updateThumbnail();
                    }
                });
            }
            auto menuItem = CCMenuItemExt::createSpriteExtra(item, [item](CCObject*) {
                auto scene = CCScene::get();
                if (!scene) return;
                /*if (auto menuLayer = MenuLayer::get()) {
                    if (auto bigPicture = menuLayer->getChildByType<BigPictureLayer>(0)) {
                        bigPicture->setShown(false);
                    }
                }*/
                item->selectLevel();
            });
            if (i == 0) {
                menuItem->setPosition({50, 0});
            } else {
                menuItem->setPosition({50, (i + 1) * 85.75F});
            }
            menuItem->setRotation(-90);
            menuItem->setTag(i + 1);
            m_scrollContent->addChild(menuItem);
            hasDoneFirst = false;
            if ((i + 1) >= levels.size()) {
                auto lastItem = LevelItem::create();
                auto menuItem2 = CCMenuItemSpriteExtra::create(lastItem, this, menu_selector(CreatorLayer::onSavedLevels));
                menuItem2->setRotation(-90);
                menuItem2->setPosition({50, (i + 2) * 85.75F});
                menuItem2->setTag(i + 2);
                m_scrollContent->addChild(menuItem2);
            }
        }
        m_scrollLayer->m_contentLayer->runAction(CCEaseExponentialOut::create(
            CCMoveTo::create(0.5F, {m_scrollLayer->m_contentLayer->getPositionX(), 0})
        ));
    }
}

void TopLevels::updateSelected(bool inc) {
    if (auto menuItem = as<CCMenuItemSpriteExtra*>(m_scrollContent->getChildByTag(currentIndex))) {
        if (auto item = typeinfo_cast<LevelItem*>(menuItem->getChildren()->firstObject())) {
            item->setSelected(false);
            if (inc) {
                currentIndex += 1;
            } else {
                currentIndex -= 1;
            }
            if (currentIndex < 3) {
                m_scrollLayer->m_contentLayer->runAction(CCEaseExponentialOut::create(
                    CCMoveTo::create(0.5F, {m_scrollLayer->m_contentLayer->getPositionX(), 0})
                ));
            } else if (currentIndex > 8) {
                m_scrollLayer->m_contentLayer->runAction(CCEaseExponentialOut::create(
                    CCMoveTo::create(0.5F, {m_scrollLayer->m_contentLayer->getPositionX(), -m_scrollLayer->m_contentLayer->getContentHeight() + m_scrollLayer->getContentHeight()})
                ));
            } else {
                m_scrollLayer->m_contentLayer->runAction(CCEaseExponentialOut::create(
                    CCMoveTo::create(0.5F, {m_scrollLayer->m_contentLayer->getPositionX(), (currentIndex - 2) * -85.75F})
                ));
            }
            updateThumbnail();
        }
    }
}

void TopLevels::enterSelected() {
    if (auto menuItem = as<CCMenuItemSpriteExtra*>(m_scrollContent->getChildByTag(currentIndex))) {
        if (auto item = typeinfo_cast<LevelItem*>(menuItem->getChildren()->firstObject())) {
            item->selectLevel();
        }
    }
}

void TopLevels::updateThumbnail() {
    //currentIndex
    if (auto menuItem = as<CCMenuItemSpriteExtra*>(m_scrollContent->getChildByTag(currentIndex))) {
        if (auto item = typeinfo_cast<LevelItem*>(menuItem->getChildren()->firstObject())) {
            if (!item->m_loaded) return;
            item->setSelected(true);
            if (item->m_levelPreview == nullptr) return;
            if (m_currentSelectionBG != nullptr) m_currentSelectionBG->removeFromParentAndCleanup(true);
            m_currentSelectionBG = CCClippingNode::create();
            if (m_currentSelectionBG == nullptr) return; // why do i need this? to prevent crashes of course!
            m_currentSelectionBG->setContentSize({this->getContentWidth(), 170.F});
            m_currentSelectionBG->setAnchorPoint({ 0.5f, 1.0f });
            m_currentSelectionBG->setAlphaThreshold(0.05f);
            m_currentSelectionBG->setZOrder(-1);
            m_currentSelectionBG->setID("background-node");

            auto stencil = CCScale9Sprite::create("BoxShadow.png"_spr);
            stencil->setAnchorPoint({0.5F, 0.F});
            if (m_currentSelectionBG == nullptr) return; // why do i need this? to prevent crashes of course!
            stencil->setPosition({m_currentSelectionBG->getContentWidth() / 2, 0});
            stencil->setContentSize(this->getContentSize() * 2);

            auto sprite = CCSprite::createWithTexture(item->m_levelPreview);
            sprite->setAnchorPoint({0.5F, 0.75F});
            sprite->setScale(this->getScaledContentWidth() / sprite->getContentWidth());
            m_currentSelectionBG->setStencil(stencil);
            m_currentSelectionBG->addChildAtPosition(sprite, Anchor::Center, {0, 40});
            //stencil->setBlendFunc({ GL_SRC_ALPHA, GL_ONE });
            m_currentSelectionBG->addChild(CCLayerGradient::create(ccc4(22, 22, 37, 120), ccc4(11, 11, 26, 255)));
            this->addChildAtPosition(m_currentSelectionBG, Anchor::Top);
            sprite->setOpacity(0);
            sprite->runAction(CCSequence::create(
                CCFadeTo::create(0.25F, 220),
                CCScaleTo::create(25.F, 1.0F),
                CCMoveBy::create(25.F, {-50, 10}),
                CCMoveBy::create(25.F, {50, -10}),
                nullptr
            ));
        }
    }
}

TopLevels* TopLevels::create(TopLevelsCategory category) {
	auto pRet = new TopLevels();
	if (pRet && pRet->init(category)) {
		pRet->autorelease();
		return pRet;
	}
	CC_SAFE_DELETE(pRet); //don't crash if it fails
	return nullptr;
}

/*
void TopLevels::loadLevels(int page) {
    m_levelsLoaded = false;
    m_loadCircle = LoadingCircle::create();
    m_loadCircle->setParentLayer(this);
    m_loadCircle->show();
    if (auto GLM = GameLevelManager::sharedState()) {
        GLM->m_levelManagerDelegate = this;
        auto searchObject = GJSearchObject::create(SearchType::SavedLevels);
        auto storedLevels = GLM->getSavedLevels(false, searchObject->m_folder);
        loadLevelsFinished(storedLevels, "");
        /\*GLM->getStoredOnlineLevels(searchObject->getKey());
        if (storedLevels) {
            loadLevelsFinished(storedLevels, "");
        } else {
            //GLM->getSavedLevels(false, searchObject->m_folder)
            GLM->getOnlineLevels(searchObject);
        }*\/
    }
}

void TopLevels::loadLevelsFinished(CCArray* levels, const char*) {
	if (m_loadingCancelled) return;
    CCArrayExt<GJGameLevel*> levelsExt = levels;
    m_levelsLoaded = true;
    m_loadCircle->fadeAndRemove();
    for (auto level : levelsExt) {
        log::info("haha {}", level->m_levelName);
    }
    
}

void TopLevels::loadLevelsFailed(const char*) {
	m_levelsLoaded = true;
	m_loadCircle->fadeAndRemove();
	m_errorText->setVisible(true);
}*/
