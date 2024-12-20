

## 构建

构建Piccolo的sln文件，运行在Visual Studio2022中

![build](/images/Piccolo/build.png)



## 源码解读

在Runtime文件夹下面有四个子文件夹，核心层、功能层、资源层、平台层，以及engine.cpp和engine.h（整个引擎的入口）。

![structure](D:\GeeSite\static\images\Piccolo\structure.png)

打开engine.cpp其中有4个重要的函数，**StartEngine**、**shutdownEngine**、**run**、**tickOneFrame**。

**StartEngine**、**shutdownEngine**一个注册类型源信息，一个取消注册，相对的函数。

```c++
void PiccoloEngine::run()
{
    std::shared_ptr<WindowSystem> window_system = g_runtime_global_context.m_window_system;
    ASSERT(window_system);

    while (!window_system->shouldClose())
    {
        const float delta_time = calculateDeltaTime();
        tickOneFrame(delta_time);
    }
}
```

run中，当窗口没有关闭时，不断执行**tickOneFrame**。点击g_runtime_global_context进入到global_context文件中。

发现声明了一个全局变量，并且实例化很多Manager（物理、粒子等）、System（窗口、渲染等）

```c++
RuntimeGlobalContext g_runtime_global_context;

void RuntimeGlobalContext::startSystems(const std::string& config_file_path)
{
    m_config_manager = std::make_shared<ConfigManager>();
    m_config_manager->initialize(config_file_path);

    m_file_system = std::make_shared<FileSystem>();
    ...
}
```

tickOneFrame中**logicalTick**(delta_time);逻辑Tick、**calculateFPS**(delta_time);计算FPS、**rendererTick**(delta_time);渲染Tick。

```c++
void PiccoloEngine::logicalTick(float delta_time)
{
    // tick world 和 用户输入
    g_runtime_global_context.m_world_manager->tick(delta_time);
    g_runtime_global_context.m_input_system->tick();
}
```

```c++
void WorldManager::tick(float delta_time)
{
    if (!m_is_world_loaded)
    {
        loadWorld(m_current_world_url);
    }

    // tick the active level
    std::shared_ptr<Level> active_level = m_current_active_level.lock();
    if (active_level)
    {
        active_level->tick(delta_time);
        m_level_debugger->tick(active_level);
    }
}
```

```c++
void Level::tick(float delta_time)
{
    if (!m_is_loaded)
    {
        return;
    }
	// tick go
    for (const auto& id_object_pair : m_gobjects)
    {
        assert(id_object_pair.second);
        if (id_object_pair.second)
        {
            id_object_pair.second->tick(delta_time);
        }
    }
    if (m_current_active_character && g_is_editor_mode == false)
    {
        m_current_active_character->tick(delta_time);
    }

    std::shared_ptr<PhysicsScene> physics_scene = m_physics_scene.lock();
    if (physics_scene)
    {
        physics_scene->tick(delta_time);
    }
}
```

```c++
void GObject::tick(float delta_time)
{
    // tick components
    for (auto& component : m_components)
    {
        if (shouldComponentTick(component.getTypeName()))
        {
            component->tick(delta_time);
        }
    }
}
```

每帧都更新逻辑和渲染，其中逻辑tick中，主体是World，一个World由很多Level组成，而Level最重要组成部分就是GameObject，只需要更新GameObject上所有的Component。

## 添加Lua、Sol2

Sol是一个用于C++绑定Lua脚本的库，仅由头文件组成，方便集成（只需要引一个头文件），可以便利地将C++代码和Lua脚本绑定起来。

下载Sol2、Lua源文件，解压缩到engine - 3rdParty中（第三方库），构建第三方库，在3rdParty下CMakeList添加如下代码，同时为lua添加cmake脚本。

```c++
if(NOT TARGET sol2)
    add_subdirectory(sol2-3.3.0)
endif()

if(NOT TARGET lua)
    include(lua.cmake)
endif()
```

在runtime - function - framework - component中新建一个lua文件夹，按照其他库一样添加一些代码。

```c++
    REFLECTION_TYPE(LuaComponent)
    CLASS(LuaComponent : public Component, WhiteListFields)
    {
        REFLECTION_BODY(LuaComponent)

    public:
        LuaComponent() = default;
        void postLoadResource(std::weak_ptr<GObject> parent_object) override;
        void tick(float delta_time) override;
        ...
     }
```

```c++
void LuaComponent::postLoadResource(std::weak_ptr<GObject> parent_object)
{
    m_parent_object = parent_object;
    // 添加一些基础函数
    m_lua_state.open_libraries(sol::lib::base);
    m_lua_state.set_function("set_float", &LuaComponent::set<float>);
    m_lua_state.set_function("get_bool", &LuaComponent::get<bool>);
    m_lua_state.set_function("invoke", &LuaComponent::invoke);
    m_lua_state["GameObject"] = m_parent_object;
}

void LuaComponent::tick(float delta_time)
{
    //LOG_INFO(m_lua_script);
    m_lua_state.script(m_lua_script);
}
```

在player.object.json中添加

```json
    {
        "$context": {
            "lua_script": "if (get_bool(GameObject, \"MotorComponent.m_is_moving\")) then set_float(GameObject, \"MotorComponent.m_motor_res.m_jump_height\", 10) else set_float(GameObject, \"MotorComponent.m_motor_res.m_jump_height\", 5) end invoke(GameObject, \"MotorComponent.getOffStuckDead\")"
        },
        "$typeName": "LuaComponent"
    }
```