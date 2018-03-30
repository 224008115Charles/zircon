// Copyright 2018 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <ddk/protocol/usb.h>
#include <ddktl/device.h>
#include <fbl/intrusive_double_list.h>
#include <fbl/mutex.h>
#include <fbl/ref_counted.h>
#include <fbl/ref_ptr.h>
#include <zircon/thread_annotations.h>

#include "usb-audio-control-interface.h"
#include "usb-audio-descriptors.h"
#include "debug-logging.h"

namespace audio {
namespace usb {

class UsbAudioStream;

class UsbAudioDevice;
using UsbAudioDeviceBase = ddk::Device<UsbAudioDevice, ddk::Unbindable>;

class UsbAudioDevice : public UsbAudioDeviceBase,
                       public fbl::RefCounted<UsbAudioDevice> {
public:
    static zx_status_t DriverBind(zx_device_t* parent);
    void DdkUnbind();
    void DdkRelease();

    void RemoveAudioStream(const fbl::RefPtr<UsbAudioStream>& stream);

    const char* log_prefix() const { return log_prefix_; }
    const usb_device_descriptor_t& desc() const { return usb_dev_desc_; }
    const usb_protocol_t& usb_proto() const { return usb_proto_; }
    uint16_t vid() const { return usb_dev_desc_.idVendor; }
    uint16_t pid() const { return usb_dev_desc_.idProduct; }

private:
    explicit UsbAudioDevice(zx_device_t* parent);

    zx_status_t Bind();
    void Probe();

    char log_prefix_[LOG_PREFIX_STORAGE] = { 0 };

    usb_protocol_t usb_proto_;
    fbl::Mutex lock_;
    usb_device_descriptor_t usb_dev_desc_;
    fbl::RefPtr<DescriptorListMemory> desc_list_;
    fbl::DoublyLinkedList<fbl::RefPtr<UsbAudioStream>> streams_ TA_GUARDED(lock_);
};

}  // namespace usb
}  // namespace audio