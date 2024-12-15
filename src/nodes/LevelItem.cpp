#include "LevelItem.hpp"
#include "../layers/BPOptionsLayer.hpp"
#include "../Variables.hpp"

bool LevelItem::init(GJGameLevel* level, bool expand) {
    if (!CCScale9Sprite::init()) return false;
    m_level = level;
    m_expanded = expand;
    //this->setContentSize({ 495.F, 230.F });
    ///this->setContentSize({ 250.F, 100.F });
    if (m_expanded) {
        this->setContentSize({ 235.F, 107.F });
    } else {
        this->setContentSize({ 70.F, 107.F });
    }

    bgSpr = cocos2d::extension::CCScale9Sprite::create("square.png");
    bgSpr->setColor({20, 20, 33});
    bgSpr->setContentSize(this->getContentSize());
    selectedBgSpr = cocos2d::extension::CCScale9Sprite::create("square.png");
    selectedBgSpr->setContentSize({this->getContentWidth() + 3, this->getContentHeight() + 3});
    selectedBgSpr->setVisible(false);
    if (m_level->m_featured > 0 || m_level->m_isEpic > 0) {
        selectedBgSpr->setColor({255, 255, 0});
    }
    this->addChildAtPosition(selectedBgSpr, Anchor::Center);
    this->addChildAtPosition(bgSpr, Anchor::Center);
    this->setAnchorPoint({0.5, 0.5});
    this->setPosition({0, 0});
    //this->setRotation(-90);

    m_levelInfoName = CCLabelBMFont::create(m_level->m_levelName.c_str(), "NunitoBold.fnt"_spr);
    m_levelInfoName->setAnchorPoint({0, 0.5});
    m_levelInfoAuthor = CCLabelBMFont::create(fmt::format("By {}", m_level->m_creatorName).c_str(), "Nunito.fnt"_spr);
    m_levelInfoAuthor->setAnchorPoint({0, 1});
    m_levelInfoName->setScale(0.5F);
    m_levelInfoAuthor->setColor({100, 100, 100});
    m_levelInfoAuthor->setScale(0.4F);

    this->addChildAtPosition(m_levelInfoName, Anchor::BottomLeft, {0, -6});
    this->addChildAtPosition(m_levelInfoAuthor, Anchor::BottomLeft, {0, -10});
    m_levelInfoName->setOpacity(0);
    m_levelInfoAuthor->setOpacity(0);
    loadImage();
    return true;
}

LevelItem* LevelItem::create(GJGameLevel* level, bool expand) {
    auto pRet = new LevelItem();
    if (pRet) {
        if (pRet->init(level, expand)) {
            pRet->autorelease();
            return pRet;
        }
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
};

bool LevelItem::init() {
    if (!CCScale9Sprite::init()) return false;
    this->setContentSize({ 70.F, 107.F });
    bgSpr = cocos2d::extension::CCScale9Sprite::create("square.png");
    bgSpr->setColor({20, 20, 33});
    bgSpr->setContentSize(this->getContentSize());
    selectedBgSpr = cocos2d::extension::CCScale9Sprite::create("square.png");
    selectedBgSpr->setColor({200, 200, 200});
    selectedBgSpr->setContentSize({this->getContentWidth() + 3, this->getContentHeight() + 3});
    selectedBgSpr->setVisible(false);

    auto name = CCLabelBMFont::create("View more", "Nunito.fnt"_spr);
    name->setScale(0.8F);
    bgSpr->addChildAtPosition(name, Anchor::Center, {0, 3});

    this->addChildAtPosition(selectedBgSpr, Anchor::Center);
    this->addChildAtPosition(bgSpr, Anchor::Center);
    this->setAnchorPoint({0.5, 0.5});
    this->setPosition({0, 0});
    //this->setRotation(-90);
    m_loaded = true;
    return true;
}

void LevelItem::selectLevel() {
    auto scene = CCScene::get();
    if (!scene) return;
    auto menuLayer = scene->getChildByType<MenuLayer>(0);
    if (!menuLayer) return;
    if (menuLayer->getChildByType<BPOptionsLayer>(0)) return;
    if (m_level != nullptr) {
        CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, LevelInfoLayer::scene(m_level, false)));
    } else {
        // a recreation of CreatorLayer::onSavedLevels because static funcs are amazing
        if (auto GM = GameManager::sharedState()) {
            SearchType type = static_cast<SearchType>(GM->m_unkSize4_16);
            if (type != SearchType::FavouriteLists) {
                type = SearchType::SavedLevels;
            }
            auto searchObj = GJSearchObject::create(type);
            int folderID = GM->getIntGameVariable("0092");
            if (folderID < 1) {
                folderID = 0;
            } else if (folderID > 999) {
                folderID = 999;
            }
            searchObj->m_folder = folderID;
            CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, LevelBrowserLayer::scene(searchObj)));
        }
    }
}

LevelItem* LevelItem::create() {
    auto pRet = new LevelItem();
    if (pRet) {
        if (pRet->init()) {
            pRet->autorelease();
            return pRet;
        }
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
};

void LevelItem::finishLoadImage(CCImage* image) {
    auto key = fmt::format("thumbnail-{}", m_level->m_levelID.value());
    if (!image) return errorLoadImage();
    CCTexture2D* texture = new CCTexture2D();
    texture->autorelease();
    texture->initWithImage(image);
    m_clippingNode = CCClippingNode::create();
    m_clippingNode->setAnchorPoint({ 0.5f, 0.5f });
    m_clippingNode->setPosition(bgSpr->getPosition());
    m_clippingNode->setContentSize(bgSpr->getScaledContentSize());
    m_clippingNode->setAlphaThreshold(0.05f);
    m_clippingNode->setID("thumbnail-node");

    auto stencil = CCScale9Sprite::create("square.png");
    stencil->setPosition(m_clippingNode->getContentSize()/2);
    stencil->setContentSize(this->getContentSize());

    auto sprite = CCSprite::createWithTexture(texture);
    sprite->setPosition(stencil->getPosition());
    if (m_expanded) {
        sprite->setScale(stencil->getScaledContentWidth() / sprite->getContentWidth());
    } else {
        sprite->setScale(stencil->getScaledContentHeight() / sprite->getContentHeight());
    }
    sprite->setOpacity(0);

    m_clippingNode->setStencil(stencil);
    m_clippingNode->addChild(sprite);
    this->addChildAtPosition(m_clippingNode, Anchor::Center);
    sprite->runAction(CCFadeIn::create(0.25f));
    m_levelPreview = texture;
    m_callback(true);
    m_loaded = true;
}
void LevelItem::errorLoadImage() {
    auto name = CCLabelBMFont::create(m_level->m_levelName.c_str(), "Nunito.fnt"_spr);
    if (m_expanded) {
        name->limitLabelWidth(120, 1.0F, 0.5F);
    } else {
        name->limitLabelWidth(60, 0.8F, 0.3F);
    }
    bgSpr->addChildAtPosition(name, Anchor::Top, {0, -15});
    m_loaded = true;
    m_callback(false);
}
void LevelItem::loadImage() {
    // pretty much copied from the level thumbnails mod code lmao
    if (auto image = typeinfo_cast<CCImage*>(Variables::ThumbnailsDict->objectForKey(fmt::format("thumbnail-{}", m_level->m_levelID.value())))) {
        finishLoadImage(image);
    } else {
        m_listener.bind([this] (web::WebTask::Event* e) {
            if (auto res = e->getValue()) {
                if (!res->ok()) {
                    errorLoadImage();
                } else {
                    //log::info("https://raw.githubusercontent.com/cdc-sys/level-thumbnails/main/thumbs/{}.png", m_level->m_levelID.value());
                    auto data = res->data();
                    auto image = Ref(new CCImage());
                    image->autorelease();
                    image->initWithImageData(const_cast<uint8_t*>(data.data()), data.size());
                    Variables::ThumbnailsDict->setObject(image, fmt::format("thumbnail-{}", m_level->m_levelID.value()));
                    geode::Loader::get()->queueInMainThread([this, image](){
                        finishLoadImage(image);
                    });
                }
            }
        });

        auto req = web::WebRequest();
        m_listener.setFilter(req.get(fmt::format("https://raw.githubusercontent.com/cdc-sys/level-thumbnails/main/thumbs/{}.png", m_level->m_levelID.value())));
    }
}
