#include "Forward.h"
#include "Document.h"
#include "Window.h"

GC::Ref<GameWindow> GameWindow::create(JS::Realm& realm) {
    return realm.create<GameWindow>(realm);
}

void GameWindow::initialize(JS::Realm& realm) {
    Base::initialize(realm);
}

void GameWindow::set_associated_environment(Document& environment) {
    m_associated_environment = environment;
}

JS::Realm& GameWindow::realm() const {
    return shape().realm();
}

