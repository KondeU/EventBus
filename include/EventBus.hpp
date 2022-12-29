#pragma once

#define BUS_REF(Bus) \
static Bus& Bus##Reference = Bus::GetReference()

#include "BusTrait.hpp"
#include "BusEvent.hpp"
#include "BusActor.hpp"

#include "RpcStandardRequest.hpp"
#include "RpcStandardResponse.hpp"
#include "RpcAsyncBroadcast.hpp"

#include "ApplicationFramework.hpp"
