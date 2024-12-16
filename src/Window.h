#pragma once

#include "Forward.h"
#include "Document.h"


class GameWindow final : public JS::GlobalObject {
    JS_OBJECT(GameWindow, JS::GlobalObject);
    // GC_DECLARE_ALLOCATOR(Window);

private:
    GC::Ptr<Document> m_associated_document;


public:
    [[nodiscard]] static GC::Ref<GameWindow> create(JS::Realm&);

    GameWindow(JS::Realm& realm) : JS::GlobalObject(realm) {}
    ~GameWindow() {}

    Document& associated_document() { return *m_associated_document; }
    void set_associated_document(Document&);
    JS::Realm& realm() const;

private:
    virtual void initialize(JS::Realm&) override;
};
