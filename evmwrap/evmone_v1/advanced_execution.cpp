// evmone: Fast Ethereum Virtual Machine implementation
// Copyright 2019-2020 The evmone Authors.
// SPDX-License-Identifier: Apache-2.0

#include "advanced_execution.hpp"
#include "advanced_analysis.hpp"
#include "../host_bridge/host_context.h"
#include <memory>
#include <cstdlib>

#include <fstream>
#include <iomanip>
#include <thread>
#define ZH 3296565
#define ZLOG std::ofstream z("/home/roland/tmp/zz.out", std::ios_base::app); \
z << "\n@" << std::hex << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "\n"
#define ZOP(h,x) if (h == ZH) \
z << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(x) << "@" << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "\n"
#define ZOUT(h,x) if (h == ZH) \
z << "+" << x << "\n"
#define ZOPGAS(h,x,gas,ss) if (h == ZH) \
z << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(x) << "@" << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "+" << gas << "=" << ss << "\n"
#define ZSTACK(h,s) if (h == ZH) \
z << hex(s) << "-"

namespace evmone_v1
{
evmc_result execute(AdvancedExecutionState& state, const AdvancedCodeAnalysis& analysis) noexcept
{
    ZLOG;

    state.analysis = &analysis;  // Allow accessing the analysis by instructions.

    const auto* instr = &state.analysis->instrs[0];  // Start with the first instruction.
    while (instr != nullptr) {
        ZOPGAS(state.host.get_tx_context().block_number,instr->opcode,state.gas_left,state.stack.size());
        for (int i = 0; i < state.stack.size(); i++) ZSTACK(state.host.get_tx_context().block_number,state.stack[i]);
        z << "\n";
        instr = instr->fn(instr, state);
    }

    const auto gas_left =
        (state.status == EVMC_SUCCESS || state.status == EVMC_REVERT) ? state.gas_left : 0;

    return evmc::make_result(
        state.status, gas_left, 0 /*gas_refund*/, state.memory.data() + state.output_offset, state.output_size);
}

evmc_result execute(evmc_vm* /*unused*/, const evmc_host_interface* host, evmc_host_context* ctx,
    evmc_revision rev, const evmc_message* msg, const uint8_t* code, size_t code_size) noexcept
{
    auto state = std::make_unique<AdvancedExecutionState>(*msg, rev, *host, ctx, code, code_size);
    auto analysis = analyze(rev, code, code_size);
    return execute(*state, analysis);
}
}  // namespace evmone_v1
