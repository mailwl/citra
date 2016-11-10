// Copyright 2014 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include <algorithm>
#include <map>
#include <vector>
#include "common/assert.h"
#include "core/core_timing.h"
#include "core/hle/kernel/event.h"
#include "core/hle/kernel/kernel.h"
#include "core/hle/kernel/thread.h"

namespace Kernel {

static void EventCallback(u64 event_handle, int /*cycle_late*/) {
    Event* event = reinterpret_cast<Event*>(event_handle);
    if (event) {
        event->signaled = true;
        event->WakeupAllWaitingThreads();
    }
}

Event::Event() {
    event_callback_type = CoreTiming::RegisterEvent("EventCallback", EventCallback);
}

Event::~Event() {}

SharedPtr<Event> Event::Create(ResetType reset_type, std::string name) {
    SharedPtr<Event> evt(new Event);

    evt->signaled = false;
    evt->reset_type = reset_type;
    evt->name = std::move(name);

    return evt;
}

bool Event::ShouldWait() {
    return !signaled;
}

void Event::Acquire() {
    ASSERT_MSG(!ShouldWait(), "object unavailable!");

    // Release the event if it's not sticky...
    if (reset_type != ResetType::Sticky)
        signaled = false;
}

void Event::Signal(u64 delay) {
    if (delay == 0) {
        signaled = true;
        WakeupAllWaitingThreads();
    } else {
        CoreTiming::ScheduleEvent(usToCycles(delay), event_callback_type, (u64)this);
    }
}

void Event::Clear() {
    signaled = false;
}

} // namespace
