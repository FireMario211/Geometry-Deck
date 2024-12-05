#include "../layers/BigPictureLayer.hpp"
#include "../Variables.hpp"
#include <Geode/modify/CCDirector.hpp>
/*
class $modify(CCDirector) {
    void setTheCache() {
        auto scene = CCScene::get();
        if (!scene) return;
        if (auto bigPicture = scene->getChildByType<BigPictureLayer>(0)) {
            if (bigPicture->lastIDClicked == "") return;
            log::debug("Set last clicked to NONE");
            bigPicture->setShown(false);
            bigPicture->lastIDClicked = "";
            //SceneManager::get()->forget(bigPicture);
            //Variables::hasMenuInit = false;
        }
    }
    bool pushScene(CCScene *pScene) {
        if (!CCDirector::pushScene(pScene)) return false;
        setTheCache();
        
        return true;
    }
    void popScene() {
        CCDirector::popScene();
        Loader::get()->queueInMainThread([]() {
            auto scene = CCScene::get();
            if (!scene) return;
            if (auto menuLayer = scene->getChildByType<MenuLayer>(0)) {
                if (auto bigPicture = scene->getChildByType<BigPictureLayer>(0)) {
                    if (bigPicture->lastIDClicked == "") return;
                    log::debug("Set last clicked to NONE");
                    bigPicture->setShown(false);
                    bigPicture->lastIDClicked = "";
                    //SceneManager::get()->forget(bigPicture);
                    //Variables::hasMenuInit = false;
                }
            }
        });
    }
    bool replaceScene(CCScene *pScene) {
        if (!CCDirector::replaceScene(pScene)) return false;
        setTheCache();
        return true;
    }
};
*/
