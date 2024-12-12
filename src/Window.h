#pragma once

#include "Forward.h"
#include "Environment.h"


class GameWindow final : public JS::GlobalObject {
    JS_OBJECT(GameWindow, JS::GlobalObject);
    // GC_DECLARE_ALLOCATOR(Window);

private:
    GC::Ptr<Environment> m_associated_environment;


public:
    [[nodiscard]] static GC::Ref<GameWindow> create(JS::Realm&);

    GameWindow(JS::Realm& realm) : JS::GlobalObject(realm) {}
    ~GameWindow() {}

    void set_associated_environment(Environment&);
    JS::Realm& realm() const;

private:
    virtual void initialize(JS::Realm&) override;
};
