#include <Geode/modify/MenuLayer.hpp>
#include "../utils.hpp"
#include "../layers/BigPictureLayer.hpp"
#include "../layers/BPOptionsLayer.hpp"
#include "../layers/TopLevels.hpp"

bool hasMenuInit = false;

class $modify(BigPic, MenuLayer) {
    static void onModify(auto& self) {
        (void)self.setHookPriorityBeforePost("MenuLayer::init", "alphalaneous.vanilla_pages");
    }
    bool init() {
        if (!MenuLayer::init()) return false;
        //auto bottomMenu = this->getChildByID("bottom-menu");
        //if (!bottomMenu) return true;
        auto size = CCDirector::sharedDirector()->getWinSize();

        Utils::invisibleInVec(this, {"main-title", "more-games-menu", "social-media-menu", "level-editor-hint", "character-select-hint", "player-username"});
        Utils::invisibleInVec(this, {"profile-menu", "right-side-menu", "close-menu", "main-menu", "bottom-menu"});
        auto gradient = CCLayerGradient::create(ccc4(22, 22, 37, 255), ccc4(0, 0, 15, 254));
        gradient->setID("menu-gradient"_spr);
        this->addChild(gradient);
        /*if (!hasMenuInit) {
            auto bigPic = BigPictureLayer::create();
            bigPic->setZOrder(90);
            //SceneManager::get()->keepAcrossScenes(bigPic);
            this->addChild(bigPic);
            hasMenuInit = true;
        */
        auto bigPic = BigPictureLayer::create();
        this->addChild(bigPic, 90);

        auto scrollLayer = ScrollLayerExt::create({ 0, 0, size.width, size.height }, true);
        scrollLayer->setPosition({size.width / 2, -(size.height) - 40});
        auto content = CCMenu::create();
        content->setZOrder(2);
        content->setContentWidth(150.F);
        scrollLayer->m_contentLayer->setContentSize({
            content->getContentSize().width,
            1000.F
        });
        //m_scrollLayer->m_contentLayer->setRotation(-90);
        content->setContentHeight(scrollLayer->m_contentLayer->getContentHeight());
        content->registerWithTouchDispatcher();
        
        scrollLayer->m_contentLayer->addChild(content);
        scrollLayer->setTouchEnabled(true);
        /*content->setLayout(
            ColumnLayout::create()
                ->setAxisAlignment(AxisAlignment::Even)
                ->setCrossAxisOverflow(false)
                ->setGap(2)
                ->setGrowCrossAxis(true)
                ->setAutoScale(false)
                ->setAxisReverse(false)
        );*/

        content->setPosition({220, 100});

        content->setContentWidth(100);
        scrollLayer->setPosition({size.width / 2, size.height / 2});
        this->addChild(scrollLayer);
        scrollLayer->m_contentLayer->setPositionX(-220);
        scrollLayer->m_contentLayer->setPositionY(0);
        scrollLayer->fixTouchPrio();
        Loader::get()->queueInMainThread([scrollLayer]() {
            cocos::handleTouchPriority(scrollLayer->m_contentLayer);
        });

        auto topLevels = TopLevels::create(TopLevelsCategory::SavedLevels);
        this->addChild(topLevels);

        /*auto scene = CCScene::get();
        if (!scene) return true;
        if (auto bigPicture = scene->getChildByType<BigPictureLayer>(0)) {
            log::debug("Enable all buttons again");
            bigPicture->setShown(true);
            bigPicture->enableAllButtons();
        }*/ 

        Loader::get()->queueInMainThread([bigPic]() {
            bigPic->createStatusbar();
        });
        return true;
    }
    void keyDown(enumKeyCodes key) {
        if (auto bpOptions = getChildByType<BPOptionsLayer>(0)) {
            if (bpOptions->keyPressDown(key)) return;
        }
        if (auto bigPicture = getChildByType<BigPictureLayer>(0)) {
            if (bigPicture->isVisible()) {
                if (bigPicture->keyPressDown(key)) return;
            }
        }
        MenuLayer::keyDown(key);
    }
    void keyBackClicked() {
        // assume CONTROLLER_Back because WHY DOES IT NOT EMIT
        if (auto bigPicture = getChildByType<BigPictureLayer>(0)) {
            if (bigPicture->isVisible()) {
                bigPicture->keyPressDown(cocos2d::CONTROLLER_Back);
            }
        }
    }
};
