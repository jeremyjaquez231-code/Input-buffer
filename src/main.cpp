#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

using namespace geode::prelude;

class $modify(BufferLayer, GJBaseGameLayer) {
    struct Fields {
        bool pending = false;
        bool released = false;
        float timeLeft = 0.f;
    };

    bool bufferOn() {
        return Mod::get()->getSettingValue<bool>("enabled");
    }

    float bufferWindow() {
        return static_cast<float>(Mod::get()->getSettingValue<double>("buffer-ms")) / 1000.f;
    }

    void handleButton(bool down, int button, bool isPlayer1) {
        if (!bufferOn() || button != 1 || !isPlayer1) {
            return GJBaseGameLayer::handleButton(down, button, isPlayer1);
        }

        auto f = m_fields.self();

        if (down) {
            if (m_player1 && m_player1->m_isOnGround) {
                f->pending = false;
                return GJBaseGameLayer::handleButton(true, button, isPlayer1);
            }
            f->pending = true;
            f->released = false;
            f->timeLeft = bufferWindow();
            return;
        }

        if (f->pending) {
            f->released = true;
            return;
        }
        GJBaseGameLayer::handleButton(false, button, isPlayer1);
    }

    void update(float dt) {
        auto f = m_fields.self();

        if (f->pending && bufferOn()) {
            f->timeLeft -= dt;

            if (m_player1 && m_player1->m_isOnGround) {
                f->pending = false;
                GJBaseGameLayer::handleButton(true, 1, true);
                if (f->released) {
                    GJBaseGameLayer::handleButton(false, 1, true);
                }
            } else if (f->timeLeft <= 0.f) {
                f->pending = false;
                if (!f->released) {
                    GJBaseGameLayer::handleButton(true, 1, true);
                }
            }
        }

        GJBaseGameLayer::update(dt);
    }
};
