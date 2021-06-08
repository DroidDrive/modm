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
#include "erpc_fifo_transport.h"

// #include <iostream>
#include <string>
#include <chrono>
using namespace std::chrono_literals;

using namespace erpc;


////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

FifoTransport::FifoTransport(Buffer* receiveBuffer, Buffer* sendBuffer)
: receiveBuffer_(receiveBuffer), sendBuffer_(sendBuffer)
{
}

FifoTransport::~FifoTransport(void) {}

erpc_status_t FifoTransport::connectClient(void)
{
    erpc_status_t status = kErpcStatus_Success;
    return status;
}

bool FifoTransport::hasMessage(void){
    return receiveBuffer_->size() > 0;
}


#define TIMEOUT_DURATION 100ms
erpc_status_t FifoTransport::underlyingReceive(uint8_t *data, uint32_t size)
{
    erpc_status_t status = kErpcStatus_ReceiveFailed;
    bool timeout = false;

    /// kikass13:
    /// apparently, this has to block until we receive something, or the cient will just read nothing
    /// the server hasnt even received the request and the client will not wait for an answer inside the erpc framework.
    /// so the response has to be given directly via the streaming process
    // if(receiveBuffer_->size() >= size)
    auto now = std::chrono::high_resolution_clock::now();
    // we could wait for the first byte to be send ... buz that would mean we would have to implement read/write
    // mutexes, so that there is no race condition (mainly calling front/pop before the data was sent)
    //while(receiveBuffer_->size() == 0){
    // but i guess we will wait for all data to be ready instead (fifo size == expected size)
    // this is slower, but probably safer :D
    while(receiveBuffer_->size() < size){
        /// ....
        auto t = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(t - now);
        // std::cout << "elapsed ... " << elapsed.count() << std::endl;
        if(elapsed >= TIMEOUT_DURATION){
            timeout = true;
            break;
        }
    }
    
    
    if(timeout){
        status = kErpcStatus_Timeout;
    }
    else{
        // std::cout << "receiveBuffer to data ["<< size <<"]: " << (unsigned long) receiveBuffer_ << "\n";
        for(unsigned int i = 0; i < size; i++){
            uint8_t val = receiveBuffer_->front();
            data[i] = val;
            receiveBuffer_->pop();
            // std::cout << std::hex << (unsigned int) val << std::dec << "  ";
            // if(i > 0 && i % 20 == 0){
            //     std::cout << "\n";
            // }
        }
        // std::cout << "\n";
        status = kErpcStatus_Success;
    }
    return status;
}

erpc_status_t FifoTransport::underlyingSend(const uint8_t *data, uint32_t size)
{
    erpc_status_t status = kErpcStatus_SendFailed;
    // std::cout << "data to sendBuffer ["<< size <<"]: " << (unsigned long) sendBuffer_ << "\n";
    for(unsigned int i = 0; i < size; i++){
        uint8_t val = data[i];
        sendBuffer_->push(val);
        // std::cout << std::hex << (unsigned int) val << std::dec << "  ";
        // if(i > 0 && i % 20 == 0){}
        //     std::cout << "\n";
        // }
    }
    // std::cout << "\n";
    status = kErpcStatus_Success;
    return status;
}
