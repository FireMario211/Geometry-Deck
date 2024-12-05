#include "BigProfilePage.hpp"

bool BigProfilePage::init() {
    if (!CCLayerGradient::initWithColor(ccc4(11, 11, 26, 255), ccc4(22, 22, 37, 255))) return false;
    return true;
}
BigProfilePage* BigProfilePage::create() {
    auto pRet = new BigProfilePage();
    if (pRet) {
        if (pRet->init()) {
            pRet->autorelease();
            return pRet;
        }
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
};
