#include "IconNode.hpp"

bool IconNode::init(CCSprite* sprite, const ccColor3B& color) {
    if (!CCNode::init()) return false;
    this->setContentSize(sprite->getContentSize());
    auto bgSpr = cocos2d::extension::CCScale9Sprite::create("square.png");
    bgSpr->setColor(color);
    bgSpr->setAnchorPoint({0, 0});
    bgSpr->setContentSize(this->getContentSize());

    m_clippingNode = CCClippingNode::create();
    m_clippingNode->setAnchorPoint({ 0.5f, 0.5f });
    m_clippingNode->setPosition(bgSpr->getPosition());
    m_clippingNode->setContentSize(bgSpr->getScaledContentSize());
    m_clippingNode->setAlphaThreshold(0.5f);

    sprite->setPosition(m_clippingNode->getContentSize()/2);
    m_clippingNode->setStencil(sprite);
    m_clippingNode->addChild(bgSpr);
    this->addChildAtPosition(m_clippingNode, Anchor::Center);
    this->setAnchorPoint({0.5, 0.5});
    return true;
}
IconNode* IconNode::create(CCSprite* spr, const ccColor3B& color) {
    auto pRet = new IconNode();
    if (pRet) {
        if (pRet->init(spr, color)) {
            pRet->autorelease();
            return pRet;
        }
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
};
