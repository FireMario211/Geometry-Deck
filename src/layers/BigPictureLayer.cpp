#include "BigPictureLayer.hpp"
#include "../nodes/IconNode.hpp"
#include "../utils.hpp"
#include "BPAlert.hpp"
#include "BPOptionsLayer.hpp"
#include "BigProfilePage.hpp"
#include "Geode/ui/Notification.hpp"
#include "Geode/utils/general.hpp"
#include "TopLevels.hpp"

// ai generated because i need to submit!! rushing!!
std::string capitalizeFirstLetter(const std::string& str) {
    if (str.empty()) return str;
    std::string result = str;
    result[0] = std::toupper(result[0]);
    return result;
}

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> parts;
    std::stringstream ss(str);
    std::string part;
    while (std::getline(ss, part, delimiter)) {
        parts.push_back(part);
    }
    return parts;
}

bool endsWith(const std::string& str, const std::string& suffix) {
    if (str.length() >= suffix.length()) {
        return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
    }
    return false;
}

// Main function to process the string
std::string processString(const std::string& input) {
    // some hardcoded
    if (input == "dankmeme.globed2/main-menu-button") {
        return "Globed";
    } else if (input == "km7dev.gdps-switcher/gdps-switch-button") {
        return "GDPS Switcher";
    }
    std::string processed = input;

    // Step 1: Remove anything before the '/' if it exists
    size_t slashPos = processed.find('/');
    if (slashPos != std::string::npos) {
        processed = processed.substr(slashPos + 1);
    }

    if (endsWith(processed, "button")) {
        processed = processed.substr(0, processed.length() - 6); // Remove "button" (6 characters)
        if (!processed.empty() && processed.back() == '-') {
            processed.pop_back();
        }
    }

    // Step 2: Split by '-' if it exists
    size_t dashPos = processed.find('-');
    if (dashPos != std::string::npos) {
        std::vector<std::string> parts = split(processed, '-');
        for (std::string& part : parts) {
            part = capitalizeFirstLetter(part);
        }
        // Join the parts with spaces
        std::ostringstream result;
        for (size_t i = 0; i < parts.size(); ++i) {
            if (i > 0) result << " ";
            result << parts[i];
        }
        return result.str();
    }

    // Step 3: If the conditions aren't met, just return the string as is
    return capitalizeFirstLetter(processed);

/*
    // Step 2: Check if it contains a '-' and split by it
    size_t dashPos = processed.find('-');
    if (dashPos != std::string::npos) {
        std::string firstPart = processed.substr(0, dashPos);
        return capitalizeFirstLetter(firstPart);
    }

    // Step 3: If the conditions aren't met, just return the string as is
    return processed;
*/
}

struct BatteryStatus {
    uint8_t percent;
    bool charging;
    bool found;
};
#ifdef GEODE_IS_WINDOWS
#include <windows.h>

BatteryStatus GetBatteryPercentage() {
    BatteryStatus status = {0, false, false};
    SYSTEM_POWER_STATUS sps;
    if (GetSystemPowerStatus(&sps)) {
        if (sps.BatteryFlag >= 100) return status;
        status.percent = std::clamp((int)sps.BatteryLifePercent, 0, 100);
        status.found = true;
        status.charging = (sps.ACLineStatus == 1);
    }
    return status;
}
#else 
BatteryStatus GetBatteryPercentage() {
    return {0, false, false};
}
#endif

ccColor3B GetBatteryColor(int percentage) {
    percentage = std::clamp(percentage, 0, 100);
    ccColor3B color;
    if (percentage >= 60) {
        color.r = 0;
        color.g = 255;
        color.b = 0;
    } else if (percentage >= 50) {
        color.r = 255;
        color.g = 255;
        color.b = 0;
    } else if (percentage >= 25) {
        color.r = 255;
        color.g = 165;
        color.b = 0;
    } else {
        color.r = 255;
        color.g = 0;
        color.b = 0;
    }
    return color;
}

// ai generated because 

class ButtonCallbackData {
    public:
    SEL_MenuHandler selector;
    std::function<void(CCObject*)> callback;
};

void BigPictureLayer::buttonCallback(CCObject* sender) {
    if (auto menuItem = as<CCMenuItemSpriteExtra*>(sender)) {
        auto data = static_cast<ButtonCallbackData*>(menuItem->getUserData());
        //data->callback(sender);
        if (data->selector != nullptr) {
            lastIDClicked = menuItem->getID();
            if ((std::find(blacklistedMenuIDs.begin(), blacklistedMenuIDs.end(), menuItem->getID())) != blacklistedMenuIDs.end()) {
                auto scene = CCScene::get();
                if (!scene) return;

                if (auto menuLayer = scene->getChildByType<MenuLayer>(0)) {} else {
                    return;
                }
            }
            menuItem->setTarget(this, data->selector);
            menuItem->activate();
            menuItem->setTarget(this, menu_selector(BigPictureLayer::buttonCallback));
            log::debug("currently blacklisted from everything other than MenuLayer: {}", blacklistedMenuIDs);
            Loader::get()->queueInMainThread([this, menuItem]() {
                if (this->lastIDClicked == "") {
                    menuItem->setEnabled(false);
                    if (!((std::find(blacklistedMenuIDs.begin(), blacklistedMenuIDs.end(), menuItem->getID())) != blacklistedMenuIDs.end())) {
                        auto scene = CCScene::get();
                        if (!scene) return;
                        log::debug("Added {} to blacklist", menuItem->getID());
                        blacklistedMenuIDs.push_back(menuItem->getID());
                    }
                }
            });
        }
    }
}

CCMenuItemSpriteExtra* BigPictureLayer::createButton(CCSprite* iconSpr, std::string id, SEL_MenuHandler selector) {
    auto node = CCNode::create();
    auto icon = IconNode::create(iconSpr, {184, 188, 191});
    auto name = processString(id);
    auto label = CCLabelBMFont::create(name.c_str(), "Nunito.fnt"_spr);
    label->limitLabelWidth(60, 1.0F, 0.6F);
    label->setAnchorPoint({0, 0});
    node->setContentSize({100, 15});
    node->setPositionX(-5);
    node->setAnchorPoint({0.5, 0.5});

    icon->setScale(node->getScaledContentHeight() / icon->getScaledContentHeight());
    if (name == "GDPS Switcher") {
        icon->setScale(0.5F); // i shouldnt need to do this km7dev
    }
    node->addChildAtPosition(icon, Anchor::Left);
    node->addChildAtPosition(label, Anchor::Center, {-35, 0});
    auto menuItem = CCMenuItemSpriteExtra::create(node, this, menu_selector(BigPictureLayer::buttonCallback));
    ButtonCallbackData* data = new ButtonCallbackData();
    data->selector = selector;
    //data->callback = callback;
    menuItem->setUserData(static_cast<ButtonCallbackData*>(data));
    //auto menuItem = CCMenuItemSpriteExtra::create(node, this, selector);
    menuItem->setAnchorPoint({0, 0.5});
    menuItem->setID(id);
    return menuItem;
}

CCMenuItemSpriteExtra* BigPictureLayer::createButton(CCSprite* iconSpr, std::string id, std::function<void(CCObject*)> callback) {
    auto node = CCNode::create();
    auto icon = IconNode::create(iconSpr, {184, 188, 191});
    auto name = processString(id);
    auto label = CCLabelBMFont::create(name.c_str(), "Nunito.fnt"_spr);
    label->limitLabelWidth(60, 1.0F, 0.6F);
    label->setAnchorPoint({0, 0});
    node->setContentSize({100, 15});
    node->setPositionX(-5);
    node->setAnchorPoint({0.5, 0.5});

    icon->setScale(node->getScaledContentHeight() / icon->getScaledContentHeight());
    if (name == "GDPS Switcher") {
        icon->setScale(0.5F); // i shouldnt need to do this km7dev
    }
    node->addChildAtPosition(icon, Anchor::Left);
    node->addChildAtPosition(label, Anchor::Center, {-35, 0});
    auto menuItem = CCMenuItemExt::createSpriteExtra(node, callback);
    menuItem->setAnchorPoint({0, 0.5});
    menuItem->setID(id);
    return menuItem;
}

bool BigPictureLayer::init() {
	if (!CCLayer::init()) return false;
    auto size = CCDirector::sharedDirector()->getWinSize();
    m_coverAllBG->setOpacity(0);
    m_coverAllBG->setZOrder(-10);
    this->addChild(m_coverAllBG);
    m_bottomLayer = BPBottomLayer::create();
    this->addChild(m_bottomLayer);
    this->setID("BigPictureLayer"_spr);

    m_sideMenu = CCMenu::create();
    m_sideMenu->setZOrder(-1);
    m_sideMenu->setContentSize({128, 265});
    m_sideMenu->setAnchorPoint({0, 0.5});

    m_sideMenuBG = CCSprite::create("BGWithShadow.png"_spr);
    m_sideMenuBG->setScaleY(1.5F);
    m_sideMenuBG->setColor({22, 22, 37});
    m_sideMenuBG->setAnchorPoint({0, 0.5});
    m_sideMenuBG->setPositionY(130);

    auto scene = CCScene::get();
    if (!scene) return true;

    if (auto menuLayer = MenuLayer::get()) {
        auto homeBtn = createButton(CCSprite::create("home.png"_spr), "Home", [this, scene](CCObject*) {
            BPAlert::create(
                "Home", 
                {
                    {"Icon Kit", [this, scene]() {
                        if (auto menuLayer = MenuLayer::get()) {
                            toggleBar(false);
                            menuLayer->onGarage(nullptr);
                        } else {
                            if (!scene->getChildByType<GJGarageLayer>(0)) {
                                CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5F, GJGarageLayer::scene()));
                            }
                        }
                    }},
                    {"Main Levels", [this]() {
                        if (auto menuLayer = MenuLayer::get()) {
                            toggleBar(false);
                            menuLayer->onPlay(nullptr);
                        }
                    }},
                    {"Online", [this, scene]() {
                        if (auto menuLayer = MenuLayer::get()) {
                            toggleBar(false);
                            menuLayer->onCreator(nullptr);
                        } else {
                            if (!scene->getChildByType<CreatorLayer>(0)) {
                                CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5F, CreatorLayer::scene()));
                            }
                        }
                    }},
                    /*{"Return to Main Menu", [this, scene, willClose]() {
                        if (!scene->getChildByType<MenuLayer>(0)) {
                            toggleBar(false);
                            CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5F, MenuLayer::scene(false)));
                        }
                    }},*/
                }
            )->show();
        });
        m_sideMenu->addChild(homeBtn);
        //m_sideMenu->addChild(createButton(this, CCSprite::createWithSpriteFrameName("edit_delBtnSmall_001.png"), "Achievements", nullptr));
        //m_sideMenu->addChild(createButton(this, CCSprite::createWithSpriteFrameName("edit_delBtnSmall_001.png"), "Stats", nullptr));
        //m_sideMenu->addChild(createButton(this, CCSprite::createWithSpriteFrameName("geode.loader/geode-logo-outline-gold.png"), "Geode", nullptr));
        if (auto bottomMenu = as<CCMenu*>(menuLayer->getChildByID("bottom-menu"))) {
            CCArrayExt<CCMenuItemSpriteExtra*> children = bottomMenu->getChildren();
            auto toggleBarCallback = [this](CCObject*){
                toggleBar();
            };
            for (int i = children.size() - 1; i >= 0; i--) {
                auto child = children[i];
                std::string name = child->getID();
                CCMenuItemSpriteExtra* button;
                //child->m_pfnSelector
                if (i < 4) {
                    auto id = processString(child->getID());
                    if (id == "Newgrounds") continue;
                    CCSprite* sprite;
                    if (id == "Achievements") {
                        sprite = CCSprite::createWithSpriteFrameName("rankIcon_1_001.png"); // GJ_featuredIcon_001.png
                    } else if (id == "Settings") {
                        continue;
                    } else if (id == "Stats") {
                        sprite = CCSprite::createWithSpriteFrameName("GJ_sMagicIcon_001.png");
                    } else {
                        continue;
                    }
                    button = createButton(sprite, name, child->m_pfnSelector);
                } else {
                    if (auto circleButton = child->getChildByType<CircleButtonSprite>(0)) {
                        if (auto sprBtn = circleButton->getChildByType<CCSprite>(0)) {
                            button = createButton(sprBtn, name, child->m_pfnSelector);
                        }
                    } else {
                        if (auto sprBtn = child->getChildByType<CCSprite>(0)) {
                            button = createButton(sprBtn, name, child->m_pfnSelector);
                        } else {
                            button = createButton(CCSprite::create("geode.loader/geode-logo-outline-gold.png"), name, child->m_pfnSelector);
                        }
                    }
                }
                if (button) m_sideMenu->addChild(button);
            }
        }
        m_sideMenu->addChild(createButton(CCSprite::create("gear.png"_spr), "Settings", [this](CCObject*) {
            if (auto GM = GameManager::sharedState()) {
                if (MenuLayer::get() != nullptr) {
                    if (auto menuLayer = MenuLayer::get()) {
                        menuLayer->onOptions(nullptr);
                    }
                    return;
                    toggleBar(false);
                    toggleBarChangeTop(true);
                    std::vector<BPCategory> options;
                    auto emptyFunc = [](matjson::Value) {};
                    options.push_back({"General", "gear.png"_spr, {
                        {"Battery Percentage", "battery-percentage", "{geode}", BPOptionType::Toggle, BPValueType::Geode, nullptr, emptyFunc},
                        {"Show Chest Icon", "show-chest", "{geode}", BPOptionType::Toggle, BPValueType::Geode, nullptr, [this](matjson::Value v) {
                            bool value = v.asBool().unwrapOrDefault();
                            m_chestBtn->setVisible(value);
                        }},
                        {"Show Battery Icon", "show-battery", "{geode}", BPOptionType::Toggle, BPValueType::Geode, nullptr, [this](matjson::Value v) {
                            bool value = v.asBool().unwrapOrDefault();
                            m_batteryBtn->setVisible(value);
                        }},
                        {"Skip Intro", "skip-intro", "{geode}", BPOptionType::Toggle, BPValueType::Geode, nullptr, emptyFunc},
                        {"Low Detail Mode", "performanceMode", "Decreases any particles that may lag the game.", BPOptionType::Toggle, BPValueType::DSDict, true, [GM](matjson::Value v) {
                            auto value = v.asBool().unwrapOrDefault();
                            GM->m_performanceMode = value;
                        }}
                    }});
                    options.push_back({"Audio", "audio.png"_spr, {
                        {"Music", "bgVolume", "", BPOptionType::Slider, BPValueType::DSDict, nullptr, [GM](matjson::Value v) {
                            float value = v.asDouble().unwrapOrDefault();
                            auto fmod = FMODAudioEngine::sharedEngine();
                            if (value < 0.03F) return fmod->setBackgroundMusicVolume(0.F);
                            auto backgroundVolume = fmod->getBackgroundMusicVolume();
                            fmod->setBackgroundMusicVolume(value);
                            if (backgroundVolume <= 0.0 && (0.0 < backgroundVolume)) {
                                GM->playMenuMusic();
                            }
                        }},
                        {"SFX", "sfxVolume", "", BPOptionType::Slider, BPValueType::DSDict, nullptr, [GM](matjson::Value v) {
                            float value = v.asDouble().unwrapOrDefault();
                            auto fmod = FMODAudioEngine::sharedEngine();
                            if (value < 0.03F) value = 0.0F;
                            fmod->setEffectsVolume(value);
                        }},
                        {"Menu Music", "0122", "", BPOptionType::Toggle, BPValueType::IntVar, true, [GM](matjson::Value v) {
                            bool value = v.asBool().unwrapOrDefault();
                            if (!value) return GM->fadeInMenuMusic();
                            auto fmod = FMODAudioEngine::sharedEngine();
                            fmod->stopAllMusic(true);
                        }},
                        #ifndef GEODE_IS_MOBILE
                        {"Change Custom Songs Location", "0033", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        #endif
                        {"Disable Song Alert", "0083", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"No Song Limit", "0018", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"Reduce Quality", "0142", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"Audio Fix 01", "0159", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"Music Offset", "0083", "", BPOptionType::Slider, BPValueType::Geode, nullptr, emptyFunc},
                        {"", "debug-btn", "", BPOptionType::Button, BPValueType::Geode, "FMOD Debug", [GM](matjson::Value v) {
                            if (auto fmod = FMODAudioEngine::sharedEngine()) {
                                FLAlertLayer::create(nullptr,"FMOD Debug",fmod->getFMODStatus(0),"OK",0x0,380.0)->show();
                            }
                        }},
                        {"", "debug-btn", "", BPOptionType::Button, BPValueType::Geode, "Debug", [GM](matjson::Value v) {
                            if (auto menuLayer = MenuLayer::get()) {
                                menuLayer->onOptions(nullptr);
                            }
                        }}
                    }});
                    std::vector<matjson::Value> resolutions;
                    std::vector<matjson::Value> texQualities = {"Auto", "High", "Medium", "Low"};
                    for (int ix = 0; ix < 27; ix++) {
                        auto res = GM->resolutionForKey(ix);
                        resolutions.push_back(fmt::format("{}x{}", res.width, res.height));
                    }
                    options.push_back({"Video", "video.png"_spr, {
                        {"Display Resolution", "resolution", "", BPOptionType::Slider, BPValueType::DSDict, resolutions, [GM](matjson::Value v) {
                            GM->m_resolution = v.asInt().unwrapOrDefault();
                        }},
                        {"Texture Quality", "texQuality", "", BPOptionType::Slider, BPValueType::DSDict, texQualities, [GM](matjson::Value v) {
                            GM->m_texQuality = v.asInt().unwrapOrDefault();
                        }},
                        {"Fullscreen", "0025", "", BPOptionType::Toggle, BPValueType::IntVar, true, emptyFunc},
                        {"Borderless", "0170", "", BPOptionType::Toggle, BPValueType::IntVar, nullptr, emptyFunc},
                        {"", "", "Applies changes", BPOptionType::Button, BPValueType::Geode, "Apply Changes", [GM](matjson::Value v) {
                            auto ghostVideoLayer = VideoOptionsLayer::create();
                            bool isFullscreen = GameManager::get()->getGameVariable("0025");
                            bool borderless = GM->getGameVariable("0170");
                            ghostVideoLayer->m_windowed = isFullscreen;
                            ghostVideoLayer->m_borderless = borderless;
                            ghostVideoLayer->m_currentResolution = GM->m_resolution - 1;
                            ghostVideoLayer->m_textureQuality = GM->m_texQuality;
                            /*CCSize resolution = GM->resolutionForKey(GM->m_resolution);
                            #ifdef GEODE_IS_MOBILE
                            PlatformToolbox::resizeWindow(resolution.width, resolution.height);
                            PlatformToolbox::toggleFullScreen(!isFullscreen, borderless, false);
                            #else
                            CCEGLView::sharedOpenGLView()->resizeWindow(resolution.width, resolution.height);
                            CCEGLView::sharedOpenGLView()->toggleFullScreen(!isFullscreen, CCEGLView::sharedOpenGLView()->getIsBorderless(), false);
                            #endif
                            GM->reloadAll(true, !isFullscreen, false);*/ 
                            ghostVideoLayer->onApply(nullptr);
                            ghostVideoLayer->release();
                        }},
                        {"ADVANCED", "", "advanced-title", BPOptionType::Title},
                        {"Smooth Fix", "0023", "Makes some optimizations that can reduce lag. Disable if game speed becomes inconsistent.", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"Show FPS", "0115", "Shows frames per second while playing.", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        #ifndef GEODE_IS_MOBILE

                        {"FPS", "fps", "Change the FPS!", BPOptionType::Input, BPValueType::IntVar, nullptr, emptyFunc},
                        {"", "", "Changes the FPS", BPOptionType::Button, BPValueType::Geode, "Apply FPS", [GM](matjson::Value v) {
                        
                        }}
                        #endif
                    }});
                    // this is horrible, an example of bad coding
                    // it probably wouldve been so much easier to hook addToggle, but considering the function is inlined on windows, it would not be worth mid-hooking
                    options.push_back({"Geometry Dash", "gdlogo-cube.png"_spr, {
                        {"GAMEPLAY", "", "gameplay-title", BPOptionType::Title},
                        {"Auto-Retry", "0026", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"Enable Faster Reset", "0052", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        #ifndef GEODE_IS_MOBILE
                        {"Lock Cursor In-Game", "0128", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        #endif
                        {"Flip 2-Player Controls", "0010", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"Always Limit Controls", "0011", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"Disable Thumbstick", "0028", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"Enable Quick Keys", "0163", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"VISUAL", "", "visual-title", BPOptionType::Title},
                        #ifndef GEODE_IS_MOBILE
                        {"Show Cursor In-Game", "0024", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        #endif
                        {"Hide Attempts", "0135", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"Flip Pause Button", "0015", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"Disable Portal Guide","0129", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"Enable Orb Guide", "0130", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"Disable Orb Scale","0140", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"Disable Trigger Orb Scale","0141", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"Disable Shake","0172", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"Disable Explosion Shake","0014", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"Disable Gravity Effect","0072", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"Default Mini Icon","0060", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"Switch Spider Teleport Color", "0061", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"Switch Dash Fire Color", "0062", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"Switch Wave Trail Color","0096", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"Hide Playtest Text","0174", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"PRACTICE", "", "performance-title", BPOptionType::Title},
                        {"Hide Practice Buttons", "0071", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"Hide Attempts", "0134", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"Enable Auto-Checkpoints","0027", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"Enable Quick Checkpoints","0068", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"Enable Death Effect","0100", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"Enable Normal Music In Editor","0125", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"Show Hitboxes","0166", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"Disable Player Hitbox","0171", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"PERFORMANCE", "", "performance-title", BPOptionType::Title},
                        {"Increase Draw Capacity","0066", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"Enable Low Detail","0108", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"Disable High Object Alert","0082", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"Enable Extra LDM","0136", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"Increase Maximum Levels","0042", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"Disable Level Saving","0119", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"Save Gauntlets","0127", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"Disable Shader Anti-Aliasing","0155", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"OTHER", "", "other-title", BPOptionType::Title},
                        {"More Comments","0094", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"Load Comments","0090", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"New Completed Filter","0073", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"Increase Local Levels Per Page","0093", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"Manual Level Order","0084", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"Percentage Decimals","0126", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"Show Leaderboard Percentage","0099", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"Do Not...","0095", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"Confirm Exit","0167", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"Fast Menu","0168", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"PARENTAL CONTROLS", "", "parental-title", BPOptionType::Title},
                        {"Disable Comments","0075", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"Disable Account Comments","0076", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc},
                        {"Only Allow Featured Levels","0077", "", BPOptionType::Toggle, BPValueType::IntVar, false, emptyFunc}
                    }});
                    auto bpOptions = BPOptionsLayer::create(options);
                    bpOptions->show();
                    m_bottomLayer->changeButtonState(4);
                }
            }
        }));
        m_sideMenu->addChild(createButton(CCSprite::createWithSpriteFrameName("edit_delBtnSmall_001.png"), "Exit", [this](CCObject*) {
            BPAlert::create(
                "Exit", 
                {
                    {"Restart Game", [this]() {
                        game::restart();
                    }, {255, 70, 40}, {255, 255, 255}},
                    {"Exit Game", [this]() {
                        game::exit();
                    }, {255, 70, 40}, {255, 255, 255}},
                }
            )->show();
        }));
        
    }

    m_sideMenu->setLayout(RowLayout::create()
        ->setAxisAlignment(AxisAlignment::Center)
        ->setAutoScale(true)
        ->setCrossAxisOverflow(false)
        ->setGrowCrossAxis(true)
        ->setCrossAxisAlignment(AxisAlignment::Center)
        ->setCrossAxisReverse(false)
        ->setGap(5)
    );

    CCArrayExt<CCMenuItemSpriteExtra*> sideMenuChildren = m_sideMenu->getChildren();
    maxIndex = sideMenuChildren.size();
    for (int i = 0; i < sideMenuChildren.size(); i++) {
        sideMenuChildren[i]->setTag(i);
    }

    m_selectionBG = CCScale9Sprite::create("square.png");
    m_selectionBG->setColor({44, 44, 66});
    m_selectionBG->setContentSize({115, 22});
    m_selectionBG->setAnchorPoint({0, 0.5});
    m_selectionBG->setZOrder(-1);
    m_sideMenu->addChild(m_selectionBG);

    if (auto m = as<CCMenuItemSpriteExtra*>(m_sideMenu->getChildByTag(0))) {
        m_selectionBG->setPositionY(m->getPositionY());
    }
    this->addChild(m_sideMenuBG, -1);
    m_sideMenuBG->setPosition({-130, size.height / 2});
    //m_sideMenu->setPosition({-130, 30});
    m_sideMenu->setPosition({-130, size.height / 2});
    this->addChild(m_sideMenu);
    setTouchEnabled(true);
    setKeyboardEnabled(true);
    return true;
}

void BigPictureLayer::enableAllButtons() {
    CCArrayExt<CCMenuItemSpriteExtra*> children = m_sideMenu->getChildren();
    for (auto child : children) {
        child->setEnabled(true);
    }
}

/*
void BigPictureLayer::keyDown(enumKeyCodes key) {
    //if (!this->isVisible()) return cocos2d::CCLayer::keyDown(key);
    auto scene = CCScene::get();
    if (!scene) return cocos2d::CCLayer::keyDown(key);
    if (auto profilePage = scene->getChildByType<BigProfilePage>(0)) {
        if (key == enumKeyCodes::KEY_Escape || key == enumKeyCodes::CONTROLLER_B) {
            profilePage->killMe();
            return;
        }
        return cocos2d::CCLayer::keyDown(key);
    }
    if (!(m_coverAllBG->getOpacity() == 0)) {
        if (key == enumKeyCodes::KEY_Escape || key == enumKeyCodes::CONTROLLER_B || key == enumKeyCodes::CONTROLLER_Start) {
            toggleBar();
            return;
        }
        return cocos2d::CCLayer::keyDown(key);
    }
    if (auto menuLayer = scene->getChildByType<MenuLayer>(0)) {
        if (auto topLevels = menuLayer->getChildByType<TopLevels>(0)) {
            if (!(m_coverAllBG->getOpacity() == 0)) return;
            if (key == enumKeyCodes::KEY_Left || key == enumKeyCodes::KEY_ArrowLeft || key == enumKeyCodes::CONTROLLER_Left || key == enumKeyCodes::CONTROLLER_LTHUMBSTICK_LEFT) {
                if (topLevels->getCurrentIndex() > 1) return topLevels->updateSelected(false);
            } else if (key == enumKeyCodes::KEY_Right || key == enumKeyCodes::KEY_ArrowRight || key == enumKeyCodes::CONTROLLER_Right || key == enumKeyCodes::CONTROLLER_LTHUMBSTICK_RIGHT) {
                if (!(topLevels->getCurrentIndex() > topLevels->getMaxIndex())) return topLevels->updateSelected(true);
            } else if (key == enumKeyCodes::KEY_Enter || key == enumKeyCodes::CONTROLLER_A) {
                this->setVisible(false);
                return topLevels->enterSelected();
            } else if (key == enumKeyCodes::CONTROLLER_Back || key == enumKeyCodes::KEY_Backspace) {
                return topLevels->showFilter();
            }
        }
    }
    if ((m_coverAllBG->getOpacity() == 0)) {
        if (!m_allowSelecting) return;
        if (key == enumKeyCodes::KEY_Escape || key == enumKeyCodes::CONTROLLER_B || key == enumKeyCodes::CONTROLLER_Start) {
            toggleBar();
        } else {
            cocos2d::CCLayer::keyDown(key);
        }
    }
}*/ 

void BigPictureLayer::enterSelected() {
    if (auto menuItem = as<CCMenuItemSpriteExtra*>(m_sideMenu->getChildByTag(currentIndexSelection))) {
        menuItem->activate();
    }
}
void BigPictureLayer::setSelected(int tag) {
    if (auto menuItem = as<CCMenuItemSpriteExtra*>(m_sideMenu->getChildByTag(tag))) {
        if (tag >= maxIndex) return;
        m_selectionBG->setPositionY(menuItem->getPositionY());
    }
}

bool BigPictureLayer::keyPressDown(enumKeyCodes key) {
    //if (!this->isVisible()) return cocos2d::CCLayer::keyDown(key);
    auto scene = CCScene::get();
    if (!scene) return false;
    
    if (auto profilePage = scene->getChildByType<BigProfilePage>(0)) {
        if (key == enumKeyCodes::KEY_Escape || key == enumKeyCodes::CONTROLLER_B) {
            profilePage->killMe();
            return true;
        }
        return false;
    }
    if (!(m_coverAllBG->getOpacity() == 0)) {
        if (key == enumKeyCodes::KEY_Escape || key == enumKeyCodes::CONTROLLER_B || key == enumKeyCodes::CONTROLLER_Back) {
            toggleBar();
            return true;
        }
        if (key == enumKeyCodes::KEY_Enter || key == enumKeyCodes::CONTROLLER_A) {
            enterSelected();
            return true;
        } else if (key == enumKeyCodes::KEY_Up || key == enumKeyCodes::KEY_ArrowUp || key == enumKeyCodes::CONTROLLER_Up) {
            currentIndexSelection--;
            if (currentIndexSelection < 0) currentIndexSelection = maxIndex - 1;
            setSelected(currentIndexSelection);
            return true;
        } else if (key == enumKeyCodes::KEY_Down || key == enumKeyCodes::KEY_ArrowDown || key == enumKeyCodes::CONTROLLER_Down) {
            currentIndexSelection++;
            if (currentIndexSelection > (maxIndex - 1)) currentIndexSelection = 0;
            setSelected(currentIndexSelection);
            return true;
        }
        return false;
    }
    if (auto menuLayer = scene->getChildByType<MenuLayer>(0)) {
        if (auto topLevels = menuLayer->getChildByType<TopLevels>(0)) {
            if (!(m_coverAllBG->getOpacity() == 0)) return true;
            if (key == enumKeyCodes::KEY_Left || key == enumKeyCodes::KEY_ArrowLeft || key == enumKeyCodes::CONTROLLER_Left) {
                if (topLevels->getCurrentIndex() > 1) {
                    topLevels->updateSelected(false);
                    return true;
                }
            } else if (key == enumKeyCodes::KEY_Right || key == enumKeyCodes::KEY_ArrowRight || key == enumKeyCodes::CONTROLLER_Right) {
                if (!(topLevels->getCurrentIndex() > topLevels->getMaxIndex())) {
                    topLevels->updateSelected(true);
                    return true;
                }
            } else if (key == enumKeyCodes::KEY_Enter || key == enumKeyCodes::CONTROLLER_A) {
                topLevels->enterSelected();
                return true;
            } else if (key == enumKeyCodes::CONTROLLER_Start || key == enumKeyCodes::KEY_Backspace) {
                topLevels->showFilter();
                return true;
            }
        }
    }
    if ((m_coverAllBG->getOpacity() == 0)) {
        if (!m_allowSelecting) return true;
        if (key == enumKeyCodes::KEY_Escape || key == enumKeyCodes::CONTROLLER_B || key == enumKeyCodes::CONTROLLER_Back) {
            toggleBar();
        } else {
            return false;
        }
    }
    return true;
}
/*
void TopLevels::keyDown(enumKeyCodes key) {
    log::info("hi {}", static_cast<int>(key));
    
}
*/
int oldZOrder = 0;

void BigPictureLayer::toggleBar() {
    if (!m_allowSelecting) return;
    auto scene = CCScene::get();
    if (scene == nullptr) return;
    m_allowSelecting = false;
    bool allowTouch = !(m_coverAllBG->getOpacity() == 0);
    if (auto layer = scene->getChildByType<CCLayer>(0)) {
        if (allowTouch) {
            //layer->setZOrder(oldZOrder);
        } else {
            //oldZOrder = layer->getZOrder();
            //layer->setZOrder(-500);
        }
    }
    if (auto menuLayer = scene->getChildByType<MenuLayer>(0)) {
        if (m_coverAllBG->getOpacity() == 0) {
            m_bottomLayer->changeButtonState(2);
        } else {
            m_bottomLayer->changeButtonState(3);
        }
    }
    toggleBarChangeTop(m_coverAllBG->getOpacity() == 0);
    if (m_coverAllBG->getOpacity() == 0) {
        m_coverAllBG->runAction(CCSequence::create(
            CCEaseExponentialOut::create(
                CCFadeTo::create(0.5F, 200)
            ),
            nullptr
        ));
        m_sideMenu->runAction(CCSequence::create(
            CCEaseExponentialOut::create(
                CCMoveTo::create(0.5F, { 0, 160, })
            ),
            CCDelayTime::create(0.05F),
            CCCallFunc::create(this, callfunc_selector(BigPictureLayer::allowSelect)),
            nullptr
        ));
        m_sideMenuBG->runAction(CCSequence::create(
            CCEaseExponentialOut::create(
                CCMoveTo::create(0.5F, { 0, 160, })
            ),
            nullptr
        ));
    } else {
        m_coverAllBG->runAction(CCSequence::create(
            CCEaseExponentialOut::create(
                CCFadeOut::create(0.5F)
            ),
            nullptr
        ));
        m_sideMenu->runAction(CCSequence::create(
            CCEaseExponentialOut::create(
                CCMoveTo::create(0.5F, { -130, 160 })
            ),
            CCDelayTime::create(0.15F),
            CCCallFunc::create(this, callfunc_selector(BigPictureLayer::allowSelect)),
            nullptr
        ));
        m_sideMenuBG->runAction(CCSequence::create(
            CCEaseExponentialOut::create(
                CCMoveTo::create(0.5F, { -130, 160 })
            ),
            nullptr
        ));
    }
}

void BigPictureLayer::toggleBarChangeTop(bool force) {
    auto size = CCDirector::sharedDirector()->getWinSize();
    if (force) {
        m_barBG->runAction(CCSequence::create(
            CCEaseExponentialOut::create(
                CCMoveTo::create(0.5F, { size.width / 2, 340 - 30 })
            ),
            nullptr
        ));
    } else {
        m_barBG->runAction(CCSequence::create(
            CCEaseExponentialOut::create(
                CCMoveTo::create(0.5F, { size.width / 2, 340 })
            ),
            nullptr
        ));
    }
}

void BigPictureLayer::toggleBar(bool force) {
    auto scene = CCScene::get();
    if (scene == nullptr) return;
    m_allowSelecting = false;
    bool allowTouch = !(m_coverAllBG->getOpacity() == 0);
    if (auto layer = scene->getChildByType<CCLayer>(0)) {
        if (allowTouch) {
            //layer->setZOrder(oldZOrder);
        } else {
            //oldZOrder = layer->getZOrder();
            //layer->setZOrder(-500);
        }
    }
    toggleBarChangeTop(m_coverAllBG->getOpacity() == 0);
    if (m_coverAllBG->getOpacity() == 0) {
        if (!force) return;
 /*
        m_barBG->runAction(CCSequence::create(
            CCEaseExponentialOut::create(
                CCMoveTo::create(0.5F, { size.width / 2, 340 - 30 })//CCMoveBy::create(0.5F, { 0, -30 })
            ),
            nullptr
        ));
*/
        m_coverAllBG->runAction(CCSequence::create(
            CCEaseExponentialOut::create(
                CCFadeTo::create(0.5F, 200)
            ),
            nullptr
        ));
        m_sideMenu->runAction(CCSequence::create(
            CCEaseExponentialOut::create(
                CCMoveTo::create(0.5F, { 0, 160, })
            ),
            CCDelayTime::create(0.05F),
            CCCallFunc::create(this, callfunc_selector(BigPictureLayer::allowSelect)),
            nullptr
        ));
        m_sideMenuBG->runAction(CCSequence::create(
            CCEaseExponentialOut::create(
                CCMoveTo::create(0.5F, { 0, 160, })
            ),
            nullptr
        ));
    } else {
        if (force) return;
        m_coverAllBG->runAction(CCSequence::create(
            CCEaseExponentialOut::create(
                CCFadeOut::create(0.5F)
            ),
            nullptr
        ));
        m_sideMenu->runAction(CCSequence::create(
            CCEaseExponentialOut::create(
                CCMoveTo::create(0.5F, { -130, 160 })
            ),
            CCDelayTime::create(0.15F),
            CCCallFunc::create(this, callfunc_selector(BigPictureLayer::allowSelect)),
            nullptr
        ));
        m_sideMenuBG->runAction(CCSequence::create(
            CCEaseExponentialOut::create(
                CCMoveTo::create(0.5F, { -130, 160 })
            ),
            nullptr
        ));
    }
}

void BigPictureLayer::createStatusbar() {
    auto size = CCDirector::sharedDirector()->getWinSize();
        
    m_barBG = CCScale9Sprite::create("square02_001.png");
    m_barBG->setID("bg"_spr);
    m_barBG->setPosition({size.width / 2, size.height + 20}); // 310
    m_barBG->setAnchorPoint({0.5, 0.5});
    m_barBG->setContentSize({ size.width + 20, 30 });

    this->addChild(m_barBG);

    auto topRightMenu = CCMenu::create();
    topRightMenu->setPosition({size.width - 195.f / 2, size.height - 25.f / 2});
    topRightMenu->setID("top-right-menu");
    topRightMenu->setContentSize({ 180.f, 50.f });
    topRightMenu->setLayout(RowLayout::create()
        ->setAxisReverse(true)
        ->setAxisAlignment(AxisAlignment::End)
        ->setCrossAxisAlignment(AxisAlignment::End)
        ->setCrossAxisLineAlignment(AxisAlignment::End)
        ->setAutoScale(false)
        ->setGap(10)
    );
    this->addChild(topRightMenu);
    if (auto gm = GameManager::sharedState()) {
        auto pIcon = SimplePlayer::create(gm->getPlayerFrame());
        pIcon->setColor(gm->colorForIdx(gm->getPlayerColor()));
        pIcon->setSecondColor(gm->colorForIdx(gm->getPlayerColor2()));
        pIcon->setGlowOutline(gm->colorForIdx(gm->getPlayerGlowColor()));
        pIcon->m_hasGlowOutline = gm->getPlayerGlow();
        pIcon->updateColors();
        if (auto spr = typeinfo_cast<CCSprite*>(pIcon->getChildren()->objectAtIndex(0))) {
            pIcon->setContentSize(spr->getContentSize());
            spr->setAnchorPoint({0, 0});
            pIcon->setAnchorPoint({0, 0});
        }
        pIcon->setScale(0.6F);
        topRightMenu->addChild(
            /*CCMenuItemExt::createSpriteExtra(pIcon, [this](CCObject*) {
                if (MenuLayer::get() != nullptr) {
                    BigProfilePage::create()->show(MenuLayer::get()->getZOrder() + 1);
                })
            })*/ 
            CCMenuItemSpriteExtra::create(pIcon, this, (GJAccountManager::get()->m_accountID == -1) ? nullptr : menu_selector(MenuLayer::onMyProfile))
        );
        m_timeLabel = CCLabelBMFont::create("", "Nunito.fnt"_spr);
        m_timeLabel->setAnchorPoint({1, 0.5});
        updateTime();
        m_timeLabel->setScale(0.75F);
        topRightMenu->addChild(m_timeLabel);
        auto batteryIcon = CCSprite::create("batteryOutline.png"_spr);
        batteryIcon->setScale(0.35F);
        m_batteryFill = CCSprite::create("square.png");
        m_batteryFill->setAnchorPoint({0, 0.5});
        m_batteryFill->setScaleX(0.F); // max of 2.0F scale
        m_batteryLabel = CCLabelBMFont::create("100%", "Nunito.fnt"_spr);
        m_batteryLabel->setScale(1.5F);
        m_batteryLabel->setAnchorPoint({1, 0});

        m_batteryWarning = CCSprite::create("batteryWarn.png"_spr);
        m_batteryWarning->setScale(0.45F);
        m_batteryWarning->setVisible(false);
        m_batteryCharging = CCSprite::create("batteryCharging.png"_spr);
        m_batteryCharging->setVisible(false);
        m_batteryLabel->setVisible(false);
        batteryIcon->addChildAtPosition(m_batteryFill, Anchor::Left, {9, 0});
        batteryIcon->addChildAtPosition(m_batteryWarning, Anchor::Center, {-3, 0});
        batteryIcon->addChildAtPosition(m_batteryCharging, Anchor::Center, {-3, 0});
        batteryIcon->addChildAtPosition(m_batteryLabel, Anchor::Left, {-10, 0});
        m_chestBtn = CCMenuItemExt::createSpriteExtraWithFilename("chest-icon.png"_spr, 0.3F, [this](CCObject* sender) {
            RewardsPage::create()->show();
        });
        m_batteryBtn = CCMenuItemExt::createSpriteExtra(batteryIcon, [this](CCObject* sender) {
            auto batteryStatus = GetBatteryPercentage();
            if (!batteryStatus.found) {
                Notification::create("Battery not found.", NotificationIcon::Warning)->show();
            } else {
                Notification::create(fmt::format("{}%{}", batteryStatus.percent, (batteryStatus.charging) ? " Charging" : ""), NotificationIcon::Info)->show();
            }
        });

        topRightMenu->addChild(m_batteryBtn);
        topRightMenu->addChild(m_chestBtn);
        topRightMenu->updateLayout();
        m_chestBtn->setPositionY(9);
        m_batteryBtn->setPositionY(9);
        m_timeLabel->setAnchorPoint({1, 0.65});
        m_timeLabel->runAction(
            CCRepeatForever::create(
                CCSequence::createWithTwoActions(
                    CCDelayTime::create(0.5F),
                    CCCallFunc::create(this, callfunc_selector(BigPictureLayer::updateTime))
                )
            )
        );
        if (GetBatteryPercentage().found) {
            batteryIcon->runAction(
                CCRepeatForever::create(
                    CCSequence::createWithTwoActions(
                        CCDelayTime::create(3.30F),
                        CCCallFunc::create(this, callfunc_selector(BigPictureLayer::updateBattery))
                    )
                )
            );
        }
        updateBattery();
    }
}

void BigPictureLayer::updateBattery() {
    auto batteryStatus = GetBatteryPercentage();
    float scale = (batteryStatus.percent >= 0) ? batteryStatus.percent * 0.02F : 0;
    if (!batteryStatus.found) {
        m_batteryWarning->setVisible(true);
        return;
    }
    m_chestBtn->setVisible(Mod::get()->getSettingValue<bool>("show-chest"));
    
    m_batteryCharging->setVisible(batteryStatus.charging);
    m_batteryFill->setScaleX(scale);
    m_batteryFill->setColor(GetBatteryColor(batteryStatus.percent));
    m_batteryLabel->setString(fmt::format("{}%", batteryStatus.percent).c_str());
    m_batteryLabel->setVisible(Mod::get()->getSettingValue<bool>("battery-percentage"));
    if (Mod::get()->getSettingValue<bool>("battery-percentage")) {
        m_chestBtn->setPositionX(25.7F);
    } else {
        m_chestBtn->setPositionX(51.7F);
    }
    m_batteryBtn->setVisible(Mod::get()->getSettingValue<bool>("show-battery"));
}

void BigPictureLayer::updateTime() {
    m_timeLabel->setString(Utils::get_current_time_string().c_str());
}

BigPictureLayer* BigPictureLayer::create() {
	auto pRet = new BigPictureLayer();
	if (pRet && pRet->init()) {
		pRet->autorelease();
		return pRet;
	}
	CC_SAFE_DELETE(pRet); //don't crash if it fails
	return nullptr;
}
