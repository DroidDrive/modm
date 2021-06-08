/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * Copyright 2021 ACRIOS Systems s.r.o.
 * Copyright 2021 DroidDrive GmbH
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "erpc_modm_device_transport.h"


////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////
namespace erpc{

ModmDeviceTransport::ModmDeviceTransport(Device* device)
: device_(device)
{
    (void) device;
}

ModmDeviceTransport::~ModmDeviceTransport(void) {}

erpc_status_t ModmDeviceTransport::connectClient(void){
    erpc_status_t status = kErpcStatus_ReceiveFailed;
    if(device_){
        status = device_->connect();
    }
    return status;
}

bool ModmDeviceTransport::hasMessage(void){
    bool hasReceived = false;
    if(device_){
        hasReceived = device_->hasReceived();
    }
    return hasReceived;
}

erpc_status_t ModmDeviceTransport::underlyingReceive(uint8_t *data, uint32_t size)
{
    erpc_status_t status = kErpcStatus_ReceiveFailed;
    if(device_){
        status = device_->read(data, size).getResult();
    }
    return status;
}

erpc_status_t ModmDeviceTransport::underlyingSend(const uint8_t *data, uint32_t size)
{
    erpc_status_t status = kErpcStatus_ReceiveFailed;
    if(device_){
        status = device_->write(data, size).getResult();
    }
    return status;
}

} // namespace erpc