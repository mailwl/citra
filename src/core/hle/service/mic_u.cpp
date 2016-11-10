// Copyright 2014 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include "common/logging/log.h"
#include "core/hle/kernel/event.h"
#include "core/hle/kernel/shared_memory.h"
#include "core/hle/service/mic_u.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Namespace MIC_U

namespace MIC_U {

static Kernel::SharedPtr<Kernel::Event> buffer_full_event;
static Kernel::SharedPtr<Kernel::SharedMemory> shared_memory;
static u8 gain = 0x28;
static bool mic_bias;
static bool is_sampling;
static bool clamp;
static u32 sampling_type;
static u32 sampling_rate;
static s32 offset;
static u32 size;
static bool loop;

static void MapMemoryBlock(Service::Interface* self) {
    u32* cmd_buff = Kernel::GetCommandBuffer();
    Handle mem_handle = cmd_buff[3];
    shared_memory = Kernel::g_handle_table.Get<Kernel::SharedMemory>(mem_handle);
    if (shared_memory) {
        shared_memory->name = "MIC_U:shared_memory";
        shared_memory->Map(Kernel::g_current_process.get(), 0, shared_memory->permissions,
                           shared_memory->other_permissions);
    }
    cmd_buff[1] = RESULT_SUCCESS.raw; // No error
    // LOG_WARNING(Service_MIC, "(STUBBED) called, size=%d", size);
}

static void UnmapMemoryBlock(Service::Interface* self) {
    u32* cmd_buff = Kernel::GetCommandBuffer();

    shared_memory->Unmap(Kernel::g_current_process.get(), shared_memory->base_address);

    cmd_buff[1] = RESULT_SUCCESS.raw; // No error
    // LOG_WARNING(Service_MIC, "(STUBBED) called");
}

static void GetGain(Service::Interface* self) {
    u32* cmd_buff = Kernel::GetCommandBuffer();
    cmd_buff[1] = RESULT_SUCCESS.raw; // No error
    cmd_buff[2] = gain;
    // LOG_WARNING(Service_MIC, "(STUBBED) called");
}

static void StartSampling(Service::Interface* self) {
    u32* cmd_buff = Kernel::GetCommandBuffer();

    sampling_type = cmd_buff[1] & 0xFF;
    sampling_rate = cmd_buff[2] & 0xFF;
    offset = cmd_buff[3];
    size = cmd_buff[4];
    loop = cmd_buff[5] & 0xFF;

    cmd_buff[1] = RESULT_SUCCESS.raw; // No error
    is_sampling = true;
    // LOG_WARNING(Service_MIC, "(STUBBED) called");
}

static void StopSampling(Service::Interface* self) {
    u32* cmd_buff = Kernel::GetCommandBuffer();
    cmd_buff[1] = RESULT_SUCCESS.raw; // No error
    is_sampling = false;
    // LOG_WARNING(Service_MIC, "(STUBBED) called");
}

static void SetGain(Service::Interface* self) {
    u32* cmd_buff = Kernel::GetCommandBuffer();
    gain = cmd_buff[1] & 0xFF;
    cmd_buff[1] = RESULT_SUCCESS.raw; // No error
    // LOG_WARNING(Service_MIC, "(STUBBED) called, gain = %d", gain);
}

static void GetMicBias(Service::Interface* self) {
    u32* cmd_buff = Kernel::GetCommandBuffer();
    cmd_buff[1] = RESULT_SUCCESS.raw; // No error
    cmd_buff[2] = mic_bias;
    // LOG_WARNING(Service_MIC, "(STUBBED) called");
}

static void SetMicBias(Service::Interface* self) {
    u32* cmd_buff = Kernel::GetCommandBuffer();
    mic_bias = cmd_buff[1] & 0xFF;
    cmd_buff[1] = RESULT_SUCCESS.raw; // No error
    // LOG_WARNING(Service_MIC, "(STUBBED) called, power = %u", mic_bias);
}

static void GetBufferFullEvent(Service::Interface* self) {
    u32* cmd_buff = Kernel::GetCommandBuffer();
    cmd_buff[1] = RESULT_SUCCESS.raw; // No error
    cmd_buff[3] = Kernel::g_handle_table.Create(buffer_full_event).MoveFrom();
    // LOG_WARNING(Service_MIC, "(STUBBED) called");
}

static void IsSampling(Service::Interface* self) {
    u32* cmd_buff = Kernel::GetCommandBuffer();
    cmd_buff[1] = RESULT_SUCCESS.raw; // No error
    cmd_buff[2] = is_sampling;
    // LOG_WARNING(Service_MIC, "(STUBBED) called");
}

static void AdjustSampling(Service::Interface* self) {
    u32* cmd_buff = Kernel::GetCommandBuffer();
    sampling_rate = cmd_buff[1] & 0xFF;
    cmd_buff[1] = RESULT_SUCCESS.raw; // No error
    // LOG_WARNING(Service_MIC, "(STUBBED) called");
}

static void SetClamp(Service::Interface* self) {
    u32* cmd_buff = Kernel::GetCommandBuffer();
    clamp = cmd_buff[1] & 0xFF;
    cmd_buff[1] = RESULT_SUCCESS.raw; // No error
    // LOG_WARNING(Service_MIC, "(STUBBED) called");
}

static void GetClamp(Service::Interface* self) {
    u32* cmd_buff = Kernel::GetCommandBuffer();
    cmd_buff[1] = RESULT_SUCCESS.raw; // No error
    cmd_buff[2] = clamp;
    // LOG_WARNING(Service_MIC, "(STUBBED) called");
}

const Interface::FunctionInfo FunctionTable[] = {
    {0x00010042, MapMemoryBlock, "MapMemoryBlock"},
    {0x00020000, UnmapMemoryBlock, "UnmapMemoryBlock"},
    {0x00030140, StartSampling, "StartSampling"},
    {0x00040040, AdjustSampling, "AdjustSampling"},
    {0x00050000, StopSampling, "StopSampling"},
    {0x00060000, IsSampling, "IsSampling"},
    {0x00070000, GetBufferFullEvent, "GetBufferFullEvent"},
    {0x00080040, SetGain, "SetGain"},
    {0x00090000, GetGain, "GetGain"},
    {0x000A0040, SetMicBias, "SetPower"},
    {0x000B0000, GetMicBias, "GetPower"},
    {0x000C0042, nullptr, "SetIirFilterMic"},
    {0x000D0040, SetClamp, "SetClamp"},
    {0x000E0000, GetClamp, "GetClamp"},
    {0x00100040, nullptr, "SetClientSDKVersion"},
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// Interface class

Interface::Interface() {
    Register(FunctionTable);
    shared_memory = nullptr;
    buffer_full_event =
        Kernel::Event::Create(Kernel::ResetType::OneShot, "MIC_U::buffer_full_event");
    is_sampling = false;
}

Interface::~Interface() {
    shared_memory = nullptr;
    buffer_full_event = nullptr;
}

} // namespace
