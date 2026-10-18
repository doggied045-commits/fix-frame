#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

// 12 frames a 60 FPS = 0.2 segundos
static constexpr float BUFFER_TIME = 12.f / 60.f;

class $modify(FixFrameLayer, GJBaseGameLayer) {
    struct Fields {
        float timeLeft = 0.f;
        bool held = false;
        bool releaseNext = false;
        bool inside = false;
    };

    bool isAirborneCube() {
        auto p = m_player1;
        if (!p) return false;
        bool cube = !(p->m_isShip || p->m_isBird || p->m_isBall || p->m_isDart ||
                      p->m_isRobot || p->m_isSpider || p->m_isSwing);
        return cube && !p->m_isOnGround;
    }

    void pushButton(int button, bool isPlayer2) {
        auto f = m_fields.self();
        if (!f->inside && button == 1 && !isPlayer2) {
            f->held = true;
            if (this->isAirborneCube()) {
                f->timeLeft = BUFFER_TIME;
            }
        }
        GJBaseGameLayer::pushButton(button, isPlayer2);
    }

    void releaseButton(int button, bool isPlayer2) {
        auto f = m_fields.self();
        if (!f->inside && button == 1 && !isPlayer2) {
            f->held = false;
        }
        GJBaseGameLayer::releaseButton(button, isPlayer2);
    }

    void processCommands(float dt) {
        GJBaseGameLayer::processCommands(dt);

        auto f = m_fields.self();

        if (f->releaseNext) {
            f->releaseNext = false;
            f->inside = true;
            GJBaseGameLayer::releaseButton(1, false);
            f->inside = false;
        }

        if (f->timeLeft > 0.f) {
            f->timeLeft -= dt;
            if (m_player1 && m_player1->m_isOnGround) {
                f->timeLeft = 0.f;
                if (!f->held) {
                    f->inside = true;
                    GJBaseGameLayer::pushButton(1, false);
                    f->inside = false;
                    f->releaseNext = true;
                }
            }
        }
    }
};

class $modify(FixFramePlayLayer, PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;
        m_isTestMode = true;
        return true;
    }
};
