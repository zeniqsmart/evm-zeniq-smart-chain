// evmone: Fast Ethereum Virtual Machine implementation
// Copyright 2018 The evmone Authors.
// SPDX-License-Identifier: Apache-2.0

/// @file
/// EVMC instance (class VM) and entry point of evmone is defined here.

#include "vm.hpp"
#include "advanced_execution.hpp"
#include "baseline.hpp"
#include <evmone_v2.h>
#include <cassert>
#include <iostream>

namespace evmone_v2
{
namespace
{
void destroy(evmc_vm* vm) noexcept
{
    assert(vm != nullptr);
    delete static_cast<VM*>(vm);
}

constexpr evmc_capabilities_flagset get_capabilities(evmc_vm* /*vm*/) noexcept
{
    return EVMC_CAPABILITY_EVM1;
}

evmc_set_option_result set_option(evmc_vm* c_vm, char const* c_name, char const* c_value) noexcept
{
    const auto name = (c_name != nullptr) ? std::string_view{c_name} : std::string_view{};
    const auto value = (c_value != nullptr) ? std::string_view{c_value} : std::string_view{};
    auto& vm = *static_cast<VM*>(c_vm);

    if (name == "advanced")
    {
        c_vm->execute = evmone_v2::advanced::execute;
            return EVMC_SET_OPTION_SUCCESS;
        }
    else if (name == "cgoto")
        {
#if EVMONE_CGOTO_SUPPORTED
        if (value == "no")
        {
            vm.cgoto = false;
            return EVMC_SET_OPTION_SUCCESS;
        }
        return EVMC_SET_OPTION_INVALID_VALUE;
#else
        return EVMC_SET_OPTION_INVALID_NAME;
#endif
    }
    else if (name == "trace")
    {
        vm.add_tracer(create_instruction_tracer(std::clog));
        return EVMC_SET_OPTION_SUCCESS;
    }
    else if (name == "histogram")
    {
        vm.add_tracer(create_histogram_tracer(std::clog));
        return EVMC_SET_OPTION_SUCCESS;
    }
    else if (name == "validate_eof")
    {
        vm.validate_eof = true;
        return EVMC_SET_OPTION_SUCCESS;
    }
    return EVMC_SET_OPTION_INVALID_NAME;
}

}  // namespace


inline constexpr VM::VM() noexcept
  : evmc_vm{
        EVMC_ABI_VERSION,
        "evmone_v2",
        PROJECT_VERSION,
        evmone_v2::destroy,
        evmone_v2::advanced::execute,
        evmone_v2::get_capabilities,
        evmone_v2::set_option,
   } {}

}  // namespace evmone_v2

extern "C" {
EVMC_EXPORT struct evmc_vm* evmc_create_evmone_v2() noexcept
{
    return new evmone_v2::VM{};
}
}
