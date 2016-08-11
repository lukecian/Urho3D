//
// Copyright (c) 2008-2016 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include "../Precompiled.h"

#include "../LuaScript/LuaFile.h"
#include "../LuaScript/LuaScript.h"
#include "../LuaScript/LuaScriptInstance.h"
#include "../LuaScript/LuaScriptUtils.h"

#include <kaguya.hpp>

namespace Urho3D
{

static void RegisterCoroutine(kaguya::State& lua)
{
    // [Function] void coroutine.start(LuaFunction function)
    // [Function] void coroutine.sleep(float time)
    // [Function] void coroutine.waitevent(StringHash eventType)
    // [Function] void coroutine.sendevent(StringHash eventType)

    lua.dostring(
        "local totalTime_ = 0                                                \n"
        "local sleepedCoroutines_ = {}                                       \n"
        "local waitEventCoroutines_ = {}                                     \n"
        "function coroutine.start(func)                                      \n"
        "    if func == nil then                                             \n"
        "        return nil                                                  \n"
        "    end                                                             \n"
        "    local co = coroutine.create(func)                               \n"
        "    return coroutine.resume(co)                                     \n"
        "end                                                                 \n"
        "function coroutine.sleep(time)                                      \n"
        "    local co = coroutine.running()                                  \n"
        "    if co == nil then                                               \n"
        "        return                                                      \n"
        "    end                                                             \n"
        "    sleepedCoroutines_[co] = totalTime_ + time                      \n"
        "    return coroutine.yield(co)                                      \n"
        "end                                                                 \n"
        "function coroutine.update(steptime)                                 \n"
        "    totalTime_ = totalTime_ + steptime                              \n"
        "    local coroutines = {}                                           \n"
        "    for co, wakeupTime in pairs(sleepedCoroutines_) do              \n"
        "        if wakeupTime < totalTime_ then                             \n"
        "            table.insert(coroutines, co)                            \n"
        "        end                                                         \n"
        "    end                                                             \n"
        "    for _, co in ipairs(coroutines) do                              \n"
        "        sleepedCoroutines_[co] = nil                                \n"
        "        coroutine.resume(co)                                        \n"
        "    end                                                             \n"
        "end                                                                 \n"
        "function coroutine.waitevent(event)                                 \n"
        "    local co = coroutine.running()                                  \n"
        "    if co == nil then                                               \n"
        "        return                                                      \n"
        "    end                                                             \n"
        "    if waitEventCoroutines_[event] == nil then                      \n"
        "        waitEventCoroutines_[event] = { co }                        \n"
        "    else                                                            \n"
        "        table.insert(waitEventCoroutines_[event], co)               \n"
        "    end                                                             \n"
        "    return coroutine.yield(co)                                      \n"
        "end                                                                 \n"
        "function coroutine.sendevent(event)                                 \n"
        "    local coroutines = waitEventCoroutines_[event]                  \n"
        "    if coroutines == nil then                                       \n"
        "        return                                                      \n"
        "    end                                                             \n"
        "    waitEventCoroutines_[event] = nil                               \n"
        "                                                                    \n"
        "    for _, co in ipairs(coroutines) do                              \n"
        "        coroutine.resume(co)                                        \n"
        "    end                                                             \n"
        "end                                                                 \n"
    );
}

static LuaScript* GetLuaScript()
{
    return globalContext->GetSubsystem<LuaScript>();
}

static void LuaScriptAddEventHandler0(StringHash eventType, const String& functionName)
{
    GetLuaScript()->AddEventHandler(eventType, functionName);
}

static void LuaScriptAddEventHandler1(Object* sender, StringHash eventType, const String& functionName)
{
    GetLuaScript()->AddEventHandler(sender, eventType, functionName);
}

static void LuaScriptAddEventHandler2(StringHash eventType, const kaguya::LuaFunction& function)
{
    GetLuaScript()->AddEventHandler(eventType, 2); // 2 is the stack index of function
}

static void LuaScriptAddEventHandler3(Object* sender, StringHash eventType, const kaguya::LuaFunction& function)
{
    GetLuaScript()->AddEventHandler(sender, eventType, 3); // 3 is the stack index of function
}

static void LuaScriptRemoveEventHandler0(StringHash eventType)
{
    GetLuaScript()->RemoveEventHandler(eventType);
}

static void LuaScriptRemoveEventHandler1(Object* sender, StringHash eventType)
{
    GetLuaScript()->RemoveEventHandler(sender, eventType);
}

static void LuaScriptRemoveEventHandlers(Object* sender)
{
    GetLuaScript()->RemoveEventHandlers(sender);
}

static void LuaScriptRemoveAllEventHandlers()
{
    GetLuaScript()->RemoveAllEventHandlers();
}

static void LuaScriptRemoveEventHandlersExcept(const PODVector<StringHash>& exceptionTypes)
{
    GetLuaScript()->RemoveEventHandlersExcept(exceptionTypes);
}

static bool LuaScriptHasSubscribedToEvent0(StringHash eventType)
{
    return GetLuaScript()->HasSubscribedToEvent(eventType);
}

static bool LuaScriptHasSubscribedToEvent1(Object* sender, StringHash eventType)
{
    return GetLuaScript()->HasSubscribedToEvent(sender, eventType);
}

static void LuaScriptSendEvent(StringHash eventType, VariantMap& eventData)
{
    GetLuaScript()->SendEvent(eventType, eventData);
}

static void LuaScriptSetExecuteConsoleCommands(bool enable)
{
    GetLuaScript()->SetExecuteConsoleCommands(enable);
}

static bool LuaScriptGetExecuteConsoleCommands()
{
    return GetLuaScript()->GetExecuteConsoleCommands();
}

static void LuaScriptSetGlobalVar(const String& key, Variant value)
{
    GetLuaScript()->SetGlobalVar(key, value);
}

static Variant LuaScriptGetGlobalVar(const String& key)
{
    return GetLuaScript()->GetGlobalVar(key);
}

static const VariantMap& LuaScriptGetGlobalVars()
{
    return GetLuaScript()->GetGlobalVars();
}

static void RegisterEventName(const char* eventName)
{
    EventNameRegistrar::RegisterEventName(eventName);
}

void RegisterLuaScript(kaguya::State &lua)
{
    using namespace kaguya;

    lua["SubscribeToEvent"] = overload(
        // [Function] void SubscribeToEvent(StringHash eventType, const String& functionName)
        &LuaScriptAddEventHandler0,
        // [Function] void SubscribeToEvent(Object* sender, StringHash eventType, const String& functionName)
        &LuaScriptAddEventHandler1,
        // [Function] void SubscribeToEvent(StringHash eventType, const kaguya::LuaFunction& function)
        &LuaScriptAddEventHandler2,
        // [Function] void SubscribeToEvent(Object* sender, StringHash eventType, const kaguya::LuaFunction& function)
        &LuaScriptAddEventHandler3);

    lua["UnsubscribeFromEvent"] = overload(
        // [Function] void UnsubscribeFromEvent(StringHash eventType)
        &LuaScriptRemoveEventHandler0,
        // [Function] void UnsubscribeFromEvent(Object* sender, StringHash eventType)
        &LuaScriptRemoveEventHandler1);

    // [Function] void UnsubscribeFromEvents(Object* sender)
    lua["UnsubscribeFromEvents"] = function(LuaScriptRemoveEventHandlers);
    // [Function] void UnsubscribeFromAllEvents()
    lua["UnsubscribeFromAllEvents"] = function(LuaScriptRemoveAllEventHandlers);
    // [Function] void UnsubscribeFromAllEventsExcept(const PODVector<StringHash>& exceptionTypes)
    lua["UnsubscribeFromAllEventsExcept"] = function(LuaScriptRemoveEventHandlersExcept);

    lua["HasSubscribedToEvent"] = overload(
        // [Function] bool HasSubscribedToEvent(StringHash eventType)
        &LuaScriptHasSubscribedToEvent0,
        // [Function] bool HasSubscribedToEvent(Object* sender, StringHash eventType)
        &LuaScriptHasSubscribedToEvent1);
    
    // [Function] void SendEvent(StringHash eventType, VariantMap& eventData)
    lua["SendEvent"] = function(LuaScriptSendEvent);
    // [Function] void SetExecuteConsoleCommands(bool enable)
    lua["SetExecuteConsoleCommands"] = function(LuaScriptSetExecuteConsoleCommands);
    // [Function] bool GetExecuteConsoleCommands()
    lua["GetExecuteConsoleCommands"] = function(LuaScriptGetExecuteConsoleCommands);
    // [Function] void SetGlobalVar(const String& key, Variant value)
    lua["SetGlobalVar"] = function(LuaScriptSetGlobalVar);
    // [Function] Variant GetGlobalVar(const String& key)
    lua["GetGlobalVar"] = function(LuaScriptGetGlobalVar);
    // [Function] const VariantMap& GetGlobalVars()
    lua["GetGlobalVars"] = function(LuaScriptGetGlobalVars);

    // [Function] void RegisterEventName()
    lua["RegisterEventName"] = function(RegisterEventName);
}

static void RegisterLuaScriptInstance(kaguya::State &lua)
{
    using namespace kaguya;

    // [Class] ScriptObject
    // [Constructor] ScriptObject()
    // [Method] Node* GetNode()
    // [Method] void SubscribeToEvent(StringHash eventType, const String& luaFunctionName)
    // [Method] void SubscribeToEvent(StringHash eventType, LuaFunction luaFunction)
    // [Method] void SubscribeToEvent(Object* sender, StringHash eventType, const String& luaFunctionName)
    // [Method] void SubscribeToEvent(Object* sender, StringHash eventType, LuaFunction luaFunction)
    // [Method] void UnsubscribeFromEvent(StringHash eventType)
    // [Method] void UnsubscribeFromEvent(Object* sender, StringHash eventType)
    // [Method] void UnsubscribeFromEvents(Object* sender)
    // [Method] void UnsubscribeFromAllEvents()
    // [Method] void UnsubscribeFromAllEventsExcept(const PODVector<StringHash>& eventTypes)
    // [Method] bool HasSubscribedToEvent(StringHash eventType)
    // [Method] bool HasSubscribedToEvent(Object* sender, StringHash eventType)
    // [ClassEnd]

    lua.dostring(
        "LuaScriptObject = {}                                                \n"
        "function LuaScriptObject:Start()                                    \n"
        "end                                                                 \n"
        "function LuaScriptObject:Stop()                                     \n"
        "end                                                                 \n"
        "function LuaScriptObject:GetNode()                                  \n"
        "    return self.node                                                \n"
        "end                                                                 \n"
        "function LuaScriptObject:SubscribeToEvent(param1, param2, param3)   \n"
        "    local instance = self.instance                                  \n"
        "    if instance == nil then                                         \n"
        "        return                                                      \n"
        "    end                                                             \n"
        "    if param3 == nil then                                           \n"
        "        instance:SubscribeToEvent(param1, param2)                   \n"
        "    else                                                            \n"
        "        instance:SubscribeToEvent(param1, param2, param3)           \n"
        "    end                                                             \n"
        "end                                                                 \n"
        "function LuaScriptObject:UnsubscribeFromEvent(param1, param2)       \n"
        "    local instance = self.instance                                  \n"
        "    if instance == nil then                                         \n"
        "        return                                                      \n"
        "    end                                                             \n"
        "    if param2 == nil then                                           \n"
        "        instance:UnsubscribeFromEvent(param1)                       \n"
        "    else                                                            \n"
        "        instance:UnsubscribeFromEvent(param1, param2)               \n"
        "    end                                                             \n"
        "end                                                                 \n"
        "function LuaScriptObject:UnsubscribeFromEvents(sender)              \n"
        "    local instance = self.instance                                  \n"
        "    if instance == nil then                                         \n"
        "        return                                                      \n"
        "    end                                                             \n"
        "    instance:UnsubscribeFromEvents(sender)                          \n"
        "end                                                                 \n"
        "function LuaScriptObject:UnsubscribeFromAllEvents()                 \n"
        "    local instance = self.instance                                  \n"
        "    if instance == nil then                                         \n"
        "        return                                                      \n"
        "    end                                                             \n"
        "    instance:UnsubscribeFromAllEvents()                             \n"
        "end                                                                 \n"
        "function LuaScriptObject:UnsubscribeFromAllEventsExcept(eventTypes) \n"
        "    local instance = self.instance                                  \n"
        "    if instance == nil then                                         \n"
        "        return                                                      \n"
        "    end                                                             \n"
        "    instance:UnsubscribeFromAllEventsExcept(eventTypes)             \n"
        "end                                                                 \n"
        "function LuaScriptObject:HasSubscribedToEvent(param1, param2)       \n"
        "    local instance = self.instance                                  \n"
        "    if instance == nil then                                         \n"
        "        return                                                      \n"
        "    end                                                             \n"
        "    if param2 == nil then                                           \n"
        "        return instance:HasSubscribedToEvent(param1)                \n"
        "    else                                                            \n"
        "        return instance:HasSubscribedToEvent(param1, param2)        \n"
        "    end                                                             \n"
        "end                                                                 \n"
        "function ScriptObject()                                             \n"
        "    local o = {}                                                    \n"
        "    setmetatable(o, LuaScriptObject)                                \n"
        "    LuaScriptObject.__index = LuaScriptObject                       \n"
        "    return o                                                        \n"
        "end                                                                 \n"
        "function CreateScriptObjectInstance(object, instance)               \n"
        "    local o = {}                                                    \n"
        "    setmetatable(o, object)                                         \n"
        "    object.__index = object                                         \n"
        "    o.instance = instance                                           \n"
        "    o.node = instance:GetNode()                                     \n"
        "    local keys = {}                                                 \n"
        "    for k, v in pairs(o) do                                         \n"
        "        keys[k] = true                                              \n"
        "    end                                                             \n"
        "    -- Call start function                                          \n"
        "    o:Start()                                                       \n"
        "    local attrNames = {}                                            \n"
        "    for k, v in pairs(o) do                                         \n"
        "        if keys[k] == nil then                                      \n"
        "            table.insert(attrNames, k)                              \n"
        "        end                                                         \n"
        "    end                                                             \n"
        "    return o, attrNames                                             \n"
        "end                                                                 \n"
    );

    // [Class] LuaScriptInstance : Component
    lua["LuaScriptInstance"].setClass(UserdataMetatable<LuaScriptInstance, Component>()

        .addOverloadedFunctions("CreateObject", 
            // [Method] bool CreateObject(const String& scriptObjectType)
            static_cast<bool (LuaScriptInstance::*)(const String&)>(&LuaScriptInstance::CreateObject),
            // [Method] bool CreateObject(LuaFile* scriptFile, const String& scriptObjectType)
            static_cast<bool (LuaScriptInstance::*)(LuaFile*, const String&)>(&LuaScriptInstance::CreateObject))

        // [Method] void SetScriptFile(LuaFile* scriptFile)
        .addFunction("SetScriptFile", &LuaScriptInstance::SetScriptFile)
        // [Method] void SetScriptObjectType(const String& scriptObjectType)
        .addFunction("SetScriptObjectType", &LuaScriptInstance::SetScriptObjectType)
        
        .addOverloadedFunctions("SubscribeToEvent",
            // [Method] void SubscribeToEvent(StringHash eventType, int functionIndex)
            static_cast<void (LuaScriptInstance::*)(StringHash, int)>(&LuaScriptInstance::AddEventHandler),
            // [Method] void SubscribeToEvent(StringHash eventType, const String& functionName)
            static_cast<void (LuaScriptInstance::*)(StringHash, const String&)>(&LuaScriptInstance::AddEventHandler),
            // [Method] void SubscribeToEvent(Object* sender, StringHash eventType, int functionIndex)
            static_cast<void (LuaScriptInstance::*)(Object*, StringHash, int)>(&LuaScriptInstance::AddEventHandler),
            // [Method] void SubscribeToEvent(Object* sender, StringHash eventType, const String& functionName)
            static_cast<void (LuaScriptInstance::*)(Object*, StringHash, const String&)>(&LuaScriptInstance::AddEventHandler))

        .addOverloadedFunctions("UnsubscribeFromEvent", 
                // [Method] void UnsubscribeFromEvent(StringHash eventType)
                static_cast<void (LuaScriptInstance::*)(StringHash)>(&LuaScriptInstance::RemoveEventHandler),
                // [Method] void UnsubscribeFromEvent(Object* sender, StringHash eventType)
                static_cast<void (LuaScriptInstance::*)(Object*, StringHash)>(&LuaScriptInstance::RemoveEventHandler))

        // [Method] void RemoveEventHandlers(Object* sender)
        .addFunction("UnsubscribeFromEvents", &LuaScriptInstance::RemoveEventHandlers)
        // [Method] void RemoveAllEventHandlers()
        .addFunction("UnsubscribeFromAllEvents", &LuaScriptInstance::RemoveAllEventHandlers)
        // [Method] void RemoveEventHandlersExcept(const PODVector<StringHash>& exceptionTypes)
        .addFunction("UnsubscribeFromAllEventsExcept", &LuaScriptInstance::RemoveEventHandlersExcept)

        .addOverloadedFunctions("HasSubscribedToEvent", 
            // [Method] bool HasEventHandler(StringHash eventType) const
            static_cast<bool (LuaScriptInstance::*)(StringHash) const>(&LuaScriptInstance::HasEventHandler),
            // [Method] bool HasEventHandler(Object* sender, StringHash eventType) const
            static_cast<bool (LuaScriptInstance::*)(Object*, StringHash) const>(&LuaScriptInstance::HasEventHandler))

        // [Method] LuaFile* GetScriptFile() const
        .addFunction("GetScriptFile", &LuaScriptInstance::GetScriptObjectType)
        // [Method] const String& GetScriptObjectType() const
        .addFunction("GetScriptObjectType", &LuaScriptInstance::GetScriptObjectType)

        // [Property(Readonly)] LuaFile* scriptFile
        .addProperty("scriptFile", &LuaScriptInstance::GetScriptObjectType)
        // [Property(Readonly)] const String& scriptObjectType
        .addProperty("scriptObjectType", &LuaScriptInstance::GetScriptObjectType)
        );
}

void RegisterLuaScriptLuaAPI(kaguya::State& lua)
{
    using namespace kaguya;

    RegisterCoroutine(lua);
    RegisterLuaScript(lua); 
    RegisterLuaScriptInstance(lua);    
}

}
