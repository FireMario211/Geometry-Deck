#pragma once
#include <Geode/Geode.hpp>
using namespace geode::prelude;

class IconNode : public CCNode {
    protected:
        CCClippingNode* m_clippingNode;
        virtual bool init(CCSprite* spr, const ccColor3B& color);
    public:
        static IconNode* create(CCSprite* spr, const ccColor3B& color);
};
