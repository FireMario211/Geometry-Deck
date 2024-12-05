#pragma once
#include <Geode/Geode.hpp>
using namespace geode::prelude;
class BreakLine : public CCNode {
protected:
    void draw() override {
        // some nodes sometimes set the blend func to
        // something else without resetting it back
        ccGLBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        ccDrawSolidRect({ 0, 0 }, this->getContentSize(), { 1.f, 1.f, 1.f, .2f });
        CCNode::draw();
    }

public:
    static BreakLine* create(float width) {
        auto ret = new BreakLine;
        if (ret->init()) {
            ret->autorelease();
            ret->setContentSize({ width, 1.f });
            return ret;
        }
        delete ret;
        return nullptr;
    }
};
