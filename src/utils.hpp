#pragma once

#include <Geode/Geode.hpp>
using namespace geode::prelude;

class Utils {
    public:
    static void removeAllInVec(cocos2d::CCLayer* layer, std::vector<std::string> ids) {
        for (auto id : ids) {
            if (auto node = layer->getChildByID(id)) {
                node->removeMeAndCleanup();
            }
        }
    }
    static void invisibleInVec(cocos2d::CCLayer* layer, std::vector<std::string> ids) {
        for (auto id : ids) {
            if (auto node = layer->getChildByID(id)) {
                node->setVisible(false);
            }
        }
    }
    static std::string get_current_time_string_localized() {
        auto now = std::chrono::system_clock::now();
        auto now_c = std::chrono::system_clock::to_time_t(now);

        std::tm now_tm;
        #ifdef _WIN32
            localtime_s(&now_tm, &now_c);
        #else
            localtime_r(&now_c, &now_tm);
        #endif

        std::stringstream ss;
        ss << std::put_time(&now_tm, "%X");
        return ss.str();
    }
    static std::string get_current_time_string() {
        bool use_24_hour = false;
        #ifdef GEODE_IS_WINDOWS
            wchar_t time_format[256];
            GetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_STIMEFORMAT, time_format, 256);
            std::wstring format_str(time_format);
            use_24_hour = format_str.find(L"H") != std::wstring::npos;
        #endif


        auto now = std::chrono::system_clock::now();
        auto now_c = std::chrono::system_clock::to_time_t(now);

        std::tm now_tm;
        #ifdef GEODE_IS_WINDOWS
            localtime_s(&now_tm, &now_c);
        #else
            localtime_r(&now_c, &now_tm);
        #endif

        std::stringstream ss;
        if (use_24_hour) {
            ss << std::put_time(&now_tm, "%H:%M");
        } else {
            ss << std::put_time(&now_tm, "%I:%M %p");
        }

        return ss.str();
    }
    template <typename T>
    static geode::cocos::CCArrayExt<T> shrinkCCArray(cocos2d::CCArray* arr, int amount) {
        if (amount >= arr->count() || amount < 0) return arr;
        geode::cocos::CCArrayExt<T> convertedArray;
        for (int i = 0; i < amount; i++) {
            convertedArray.push_back(static_cast<T>(arr->objectAtIndex(i)));
        }
        return convertedArray;
    }

    //stolen from https://github.com/Alphalaneous/Projected-Stars/blob/main/src/ParticleHelper.hpp
    static CCParticleSystemQuad* createStarParticles(float duration, int maxParticles, ccColor3B colorBefore, CCPoint speed, CCPoint radius = {30, 10}) {

        struct Color {
            float r;
            float g;
            float b;
        };
        Color color = {
            (float)colorBefore.r / 256.F,
            (float)colorBefore.g / 256.F,
            (float)colorBefore.b / 256.F,
        };
        auto dict = CCDictionary::createWithContentsOfFileThreadSafe("dragEffect.plist");
        if(!dict) dict = CCDictionary::create();

        dict->setObject(CCString::create("1"), "emitterType");
        dict->setObject(CCString::createWithFormat("%f", duration), "duration");
        dict->setObject(CCString::createWithFormat("%i", maxParticles), "maxParticles");
        dict->setObject(CCString::create("1.5"), "particleLifespan");
        dict->setObject(CCString::create("0"), "angle");
        dict->setObject(CCString::create("360"), "angleVariance");
        dict->setObject(CCString::create(fmt::format("{}", radius.y)), "maxRadius");
        dict->setObject(CCString::create(fmt::format("{}", radius.x)), "minRadius");
        dict->setObject(CCString::create(fmt::format("{}", speed.x)), "speed");
        dict->setObject(CCString::create(fmt::format("{}", speed.y)), "speedVariance");
        dict->setObject(CCString::create("0"), "sourcePositionx");
        dict->setObject(CCString::create("0"), "sourcePositiony");
        dict->setObject(CCString::create("0"), "sourcePositionVariancex");
        dict->setObject(CCString::create("0"), "sourcePositionVariancey");
        dict->setObject(CCString::create("4"), "startParticleSize");
        dict->setObject(CCString::create("4"), "finishParticleSize");
        dict->setObject(CCString::create("0"), "startParticleSizeVariance");
        dict->setObject(CCString::create(fmt::format("{}", color.r)), "startColorRed");
        dict->setObject(CCString::create(fmt::format("{}", color.g)), "startColorGreen");
        dict->setObject(CCString::create(fmt::format("{}", color.b)), "startColorBlue");
        dict->setObject(CCString::create("1"), "startColorAlpha");
        dict->setObject(CCString::create("0"), "startColorVarianceRed");
        dict->setObject(CCString::create("0"), "startColorVarianceBlue");
        dict->setObject(CCString::create("0"), "startColorVarianceGreen");
        dict->setObject(CCString::create("0"), "startColorVarianceAlpha");
        dict->setObject(CCString::create(fmt::format("{}", color.r)), "finishColorRed");
        dict->setObject(CCString::create(fmt::format("{}", color.g)), "finishColorGreen");
        dict->setObject(CCString::create(fmt::format("{}", color.b)), "finishColorBlue");
        dict->setObject(CCString::create("0"), "finishColorAlpha");
        dict->setObject(CCString::create("0"), "finishColorVarianceRed");
        dict->setObject(CCString::create("0"), "finishColorVarianceBlue");
        dict->setObject(CCString::create("0"), "finishColorVarianceGreen");
        dict->setObject(CCString::create("0"), "finishColorVarianceAlpha");
        dict->setObject(CCString::create("square.png"), "textureFileName");

        auto p = CCParticleSystemQuad::create();
        p->initWithDictionary(dict, false);
        
        return p;
    }
};
