#pragma once
#include "../nodes/ScrollLayerExt.hpp"
#include <Geode/Geode.hpp>
using namespace geode::prelude;

// thanks https://github.com/Minemaker0430/GDDPIntegration/blob/1eff196dafae79999fee3aacb60aec0b96198644/src/menus/DPListLayer.hpp

enum TopLevelsCategory {
    SavedLevels,
    MainLevels
};

class TopLevels : public CCLayer {
    protected:
        virtual bool init(TopLevelsCategory category);

        CCMenu* m_buttonMenu;
        TopLevelsCategory m_category;
        ScrollLayerExt* m_scrollLayer;
        CCMenu* m_scrollContent;
        CCClippingNode* m_currentSelectionBG;
        CCLabelBMFont* m_label;
        int currentIndex = 1;
        int maxIndex = 1;
        int m_folderID = 0;
        void updateThumbnail();
        void regenerateLevels();
        void regenerateLevels(int folderID) {
            m_folderID = folderID;
            if (auto GM = GameManager::sharedState()) {
                GM->setIntGameVariable("0092", m_folderID);
            }
            regenerateLevels();
        }
        //TracklessScrollbar* m_scrollLayer;
    public:
        void showFilter();
        int getCurrentIndex() { return currentIndex; };
        int getMaxIndex() { return maxIndex; };
        void updateSelected(bool inc);
        void enterSelected();
        static TopLevels* create(TopLevelsCategory category);
        /*void loadLevels(int);
        void loadLevelsFinished(CCArray*, const char*) override;
        void loadLevelsFailed(const char*) override;
        void loadLevelsFinished(CCArray* levels, const char* key, int) override {
            loadLevelsFinished(levels, key);
        }
        void loadLevelsFailed(const char* key, int) override {
            loadLevelsFailed(key);
        }
        */
};
