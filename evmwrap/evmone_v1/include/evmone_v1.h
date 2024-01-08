// evmone: Fast Ethereum Virtual Machine implementation
// Copyright 2018-2019 The evmone Authors.
// SPDX-License-Identifier: Apache-2.0

#ifndef EVMONE_V1_H
#define EVMONE_V1_H

#include <evmc/evmc.h>
#include <evmc/utils.h>

#if __cplusplus
extern "C" {
#endif

EVMC_EXPORT struct evmc_vm* evmc_create_evmone_v1(void) EVMC_NOEXCEPT;

#if __cplusplus
}
#endif

#endif  // EVMONE_V1_H
