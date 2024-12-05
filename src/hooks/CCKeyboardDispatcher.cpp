#include "../layers/BigPictureLayer.hpp"
#include <Geode/modify/CCKeyboardDispatcher.hpp>

/*
class $modify(CCKeyboardDispatcher) {
    bool dispatchKeyboardMSG(enumKeyCodes key, bool isKeyDown, bool isKeyRepeat) {
        bool ret = CCKeyboardDispatcher::dispatchKeyboardMSG(key, isKeyDown, isKeyRepeat);
        auto scene = CCScene::get();
        if (!scene) return ret;
        /*if (auto bigPicture = scene->getChildByType<BigPictureLayer>(0)) {
            if (bigPicture->isVisible() && isKeyDown) {
                if (!bigPicture->keyPressDown(key)) return ret;
            }
        }*\/ 
        if (auto menuLayer = scene->getChildByType<MenuLayer>(0)) {
            if (auto bigPicture = menuLayer->getChildByType<BigPictureLayer>(0)) {
                if (bigPicture->isVisible() && isKeyDown) {
                    if (!bigPicture->keyPressDown(key)) return ret;
                }
            }
        }
        return ret;
    }
};
*/
