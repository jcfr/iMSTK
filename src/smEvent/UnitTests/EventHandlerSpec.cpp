// This file is part of the SimMedTK project.
// Copyright (c) Center for Modeling, Simulation, and Imaging in Medicine,
//                        Rensselaer Polytechnic Institute
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//---------------------------------------------------------------------------
//
// Authors:
//
// Contact:
//---------------------------------------------------------------------------

#include <bandit/bandit.h>
#include <memory>

// SimMedTK includes
#include "smCore/smCoreClass.h"
#include "smCore/smEventHandler.h"
#include "smEvent/smAudioEvent.h"
#include "smEvent/smCameraEvent.h"
#include "smEvent/smHapticEvent.h"
#include "smEvent/smKeyboardEvent.h"
#include "smEvent/smLightMotionEvent.h"
#include "smEvent/smMouseButtonEvent.h"
#include "smEvent/smMouseMoveEvent.h"
#include "smEvent/smObjectClickedEvent.h"

using namespace bandit;
using namespace smtk::Event;

struct MyObserver : public smCoreClass
{
    MyObserver(smtk::Event::EventType _eventType) : success(false), eventType(_eventType) {}

    void handleEvent(std::shared_ptr<smEvent> event) override
    {
        std::shared_ptr<smKeyboardEvent> keyboardEvent = std::static_pointer_cast<smKeyboardEvent>(event);
        if(keyboardEvent != nullptr && smKeyboardEvent::EventName == eventType)
        {
            success = true;
            std::cout << "its a keyboard event..." << std::endl;
        }
        std::shared_ptr<smAudioEvent> audioEvent = std::static_pointer_cast<smAudioEvent>(event);
        if(audioEvent != nullptr && smAudioEvent::EventName == eventType)
        {
            success = true;
            std::cout << "its an audio event..." << std::endl;
        }
        std::shared_ptr<smCameraEvent> cameraEvent = std::static_pointer_cast<smCameraEvent>(event);
        if(cameraEvent != nullptr && smCameraEvent::EventName == eventType)
        {
            success = true;
            std::cout << "its a camera event..." << std::endl;
        }
        std::shared_ptr<smHapticEvent> hapticEvent = std::static_pointer_cast<smHapticEvent>(event);
        if(hapticEvent != nullptr && smHapticEvent::EventName == eventType)
        {
            success = true;
            std::cout << "its a haptic event..." << std::endl;
        }
        std::shared_ptr<smLightMotionEvent> lightEvent = std::static_pointer_cast<smLightMotionEvent>(event);
        if(lightEvent != nullptr && smLightMotionEvent::EventName == eventType)
        {
            success = true;
            std::cout << "its a light event..." << std::endl;
        }
        std::shared_ptr<smMouseButtonEvent> mouseButtonEvent = std::static_pointer_cast<smMouseButtonEvent>(event);
        if(mouseButtonEvent != nullptr && smMouseButtonEvent::EventName == eventType)
        {
            success = true;
            std::cout << "its a mouse button event..." << std::endl;
        }
        std::shared_ptr<smMouseMoveEvent> mouseMoveEvent = std::static_pointer_cast<smMouseMoveEvent>(event);
        if(mouseMoveEvent != nullptr && smMouseMoveEvent::EventName == eventType)
        {
            success = true;
            std::cout << "its a mouse button event..." << std::endl;
        }
        std::shared_ptr<smObjectClickedEvent> objectClickedEvent = std::static_pointer_cast<smObjectClickedEvent>(event);
        if(objectClickedEvent != nullptr && smObjectClickedEvent::EventName == eventType)
        {
            success = true;
            std::cout << "its an object clicked event..." << std::endl;
        }

    }
    bool success;
    smtk::Event::EventType eventType;
};

go_bandit([](){

    describe("Event handler", []() {
        it("constructs ", []() {
            std::shared_ptr<smEventHandler> eventHandler = std::make_shared<smEventHandler>();
            AssertThat(eventHandler != nullptr, IsTrue());
        });
        it("attaches events ", []() {
            std::shared_ptr<smEventHandler> eventHandler = std::make_shared<smEventHandler>();

            smEvent::Pointer event = std::make_shared<smEvent>();
            smCoreClass::Pointer observer = std::make_shared<smCoreClass>();

            eventHandler->attachEvent(smtk::Event::EventType::Audio,observer);

            AssertThat(eventHandler->isAttached(smtk::Event::EventType::Audio,observer), IsTrue());

        });
        it("detaches events ", []() {
            std::shared_ptr<smEventHandler> eventHandler = std::make_shared<smEventHandler>();

            smEvent::Pointer event = std::make_shared<smEvent>();
            smCoreClass::Pointer observer = std::make_shared<smCoreClass>();

            eventHandler->attachEvent(smtk::Event::EventType::Audio,observer);

            AssertThat(eventHandler->isAttached(smtk::Event::EventType::Audio,observer), IsTrue());
            auto index = observer->getEventIndex(smtk::Event::EventType::Audio);

            eventHandler->detachEvent(smtk::Event::EventType::Audio,observer);

            AssertThat(eventHandler->isAttached(smtk::Event::EventType::Audio,index), IsFalse());
        });
        it("dispatches events ", []() {
            std::shared_ptr<smEventHandler> eventHandler = std::make_shared<smEventHandler>();

            smEvent::Pointer event = std::make_shared<smEvent>();
            std::shared_ptr<MyObserver> observer;

            observer = std::make_shared<MyObserver>(smtk::Event::EventType::None);
            eventHandler->attachEvent(smtk::Event::EventType::None,observer);
            eventHandler->triggerEvent(std::make_shared<smEvent>());
            AssertThat(observer->success, IsFalse());

            observer = std::make_shared<MyObserver>(smtk::Event::EventType::Audio);
            eventHandler->attachEvent(smtk::Event::EventType::Audio,observer);
            eventHandler->triggerEvent(std::make_shared<smAudioEvent>());
            AssertThat(observer->success, IsTrue());

            observer = std::make_shared<MyObserver>(smtk::Event::EventType::Keyboard);
            eventHandler->attachEvent(smtk::Event::EventType::Keyboard,observer);
            eventHandler->triggerEvent(std::make_shared<smKeyboardEvent>(smKey::A));
            AssertThat(observer->success, IsTrue());

            observer = std::make_shared<MyObserver>(smtk::Event::EventType::CameraUpdate);
            eventHandler->attachEvent(smtk::Event::EventType::CameraUpdate,observer);
            eventHandler->triggerEvent(std::make_shared<smCameraEvent>());
            AssertThat(observer->success, IsTrue());

            observer = std::make_shared<MyObserver>(smtk::Event::EventType::Haptic);
            eventHandler->attachEvent(smtk::Event::EventType::Haptic,observer);
            eventHandler->triggerEvent(std::make_shared<smHapticEvent>(0,"HapticDevice"));
            AssertThat(observer->success, IsTrue());

            observer = std::make_shared<MyObserver>(smtk::Event::EventType::LightMotion);
            eventHandler->attachEvent(smtk::Event::EventType::LightMotion,observer);
            eventHandler->triggerEvent(std::make_shared<smLightMotionEvent>(0));
            AssertThat(observer->success, IsTrue());

            observer = std::make_shared<MyObserver>(smtk::Event::EventType::MouseButton);
            eventHandler->attachEvent(smtk::Event::EventType::MouseButton,observer);
            eventHandler->triggerEvent(std::make_shared<smMouseButtonEvent>(smMouseButton::Button0));
            AssertThat(observer->success, IsTrue());

            observer = std::make_shared<MyObserver>(smtk::Event::EventType::MouseMove);
            eventHandler->attachEvent(smtk::Event::EventType::MouseMove,observer);
            eventHandler->triggerEvent(std::make_shared<smMouseMoveEvent>());
            AssertThat(observer->success, IsTrue());

            observer = std::make_shared<MyObserver>(smtk::Event::EventType::ObjectClicked);
            eventHandler->attachEvent(smtk::Event::EventType::ObjectClicked,observer);
            eventHandler->triggerEvent(std::make_shared<smObjectClickedEvent>(0));
            AssertThat(observer->success, IsTrue());

        });

    });

});
