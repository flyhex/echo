#include "App.h"
#include "Log.h"
#include <engine/core/main/Engine.h>
#include <engine/core/render/gles/GLES.h>

namespace Echo
{
    App* App::instance()
    {
        static App app;
        return &app;
    }

    void App::initRes(const String& resDir, const String& userDir)
    {
        m_resDir = resDir;
        m_userDir = userDir;
    }

    void App::initEngine(i32 width, i32 height)
    {
        // Log
        Echo::Log::instance()->addOutput(EchoNew(GameLog("Game")));

        // Initialize
        Echo::initRender(0, width, height);
        Echo::initEngine( m_resDir + "app.echo", true);
    }

    void App::tick()
    {
        Echo::Engine::instance()->tick(0.01f);
    }
}