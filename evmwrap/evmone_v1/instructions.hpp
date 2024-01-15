// evmone: Fast Ethereum Virtual Machine implementation
// Copyright 2019-2020 The evmone Authors.
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "execution_state.hpp"
#include "instructions_traits.hpp"
#include <ethash/keccak.hpp>

#define RET(x) x

namespace evmone_v1
{
#define StackTop Stack&
/* v2:
class
StackTop {
    uint256* m_top;

public:
    StackTop(uint256* top) noexcept : m_top{top} {}

    /// Returns the reference to the stack item by index, where 0 means the top item
    /// and positive index values the items further down the stack.
    /// Using [-1] is also valid, but .push() should be used instead.
    [[nodiscard]] uint256& operator[](int index) noexcept { return m_top[-index]; }

    /// Returns the reference to the stack top item.
    [[nodiscard]] uint256& top() noexcept { return *m_top; }

    /// Returns the current top item and move the stack top pointer down.
    /// The value is returned by reference because the stack slot remains valid.
    [[nodiscard]] uint256& pop() noexcept { return *m_top--; }

    /// Assigns the value to the stack top and moves the stack top pointer up.
    void push(const uint256& value) noexcept { *++m_top = value; }
};
*/

#define Result evmc_status_code
#define TermResult evmc_status_code
/* v2:
struct
Result {
    evmc_status_code status;
    int64_t gas_left;
};

/// Instruction result indicating that execution terminates unconditionally.
struct TermResult : Result
{};
*/

constexpr auto max_buffer_size = std::numeric_limits<uint32_t>::max();

/// The size of the EVM 256-bit word.
constexpr auto word_size = 32;

/// Returns number of words what would fit to provided number of bytes,
/// i.e. it rounds up the number bytes to number of words.
inline constexpr int64_t num_words(uint64_t size_in_bytes) noexcept
{
    return (static_cast<int64_t>(size_in_bytes) + (word_size - 1)) / word_size;
}

inline bool check_memory(
    ExecutionState& state, const uint256& offset, uint64_t size) noexcept
{
    if (offset > max_buffer_size)
        return false;

    const auto new_size = static_cast<uint64_t>(offset) + size;
    if (new_size > state.memory.size()) {
        const auto new_words = num_words(new_size);
        const auto current_words = static_cast<int64_t>(state.memory.size() / word_size);
        const auto new_cost = 3 * new_words + new_words * new_words / 512;
        const auto current_cost = 3 * current_words + current_words * current_words / 512;
        const auto cost = new_cost - current_cost;
        if ((state.gas_left -= cost) < 0)
            return false;

        state.memory.resize(static_cast<size_t>(new_words * word_size));
    }

    return true;
}

inline bool check_memory(
    ExecutionState& state, const uint256& offset, const uint256& size) noexcept
{
    if (size == 0)
        return true;

    if (size > max_buffer_size)
        return false;

    return check_memory(state, offset, static_cast<uint64_t>(size));
}

inline void add(StackTop stack) noexcept
{
    stack.top() += stack.pop();
}

inline void mul(StackTop stack) noexcept
{
    stack.top() *= stack.pop();
}

inline void sub(StackTop stack) noexcept
{
    stack[1] = stack[0] - stack[1];
    stack.pop();
}

inline void div(StackTop stack) noexcept
{
    auto& v = stack[1];
    v = v != 0 ? stack[0] / v : 0;
    stack.pop();
}

inline void sdiv(StackTop stack) noexcept
{
    auto& v = stack[1];
    v = v != 0 ? intx::sdivrem(stack[0], v).quot : 0;
    stack.pop();
}

inline void mod(StackTop stack) noexcept
{
    auto& v = stack[1];
    v = v != 0 ? stack[0] % v : 0;
    stack.pop();
}

inline void smod(StackTop stack) noexcept
{
    auto& v = stack[1];
    v = v != 0 ? intx::sdivrem(stack[0], v).rem : 0;
    stack.pop();
}

inline void addmod(StackTop stack) noexcept
{
    const auto x = stack.pop();
    const auto y = stack.pop();
    auto& m = stack.top();
    m = m != 0 ? intx::addmod(x, y, m) : 0;
}

inline void mulmod(StackTop stack) noexcept
{
    const auto x = stack.pop();
    const auto y = stack.pop();
    auto& m = stack.top();
    m = m != 0 ? intx::mulmod(x, y, m) : 0;
}

inline Result exp(
        ExecutionState& state) noexcept
{
    const auto base = state.stack.pop();
    auto& exponent = state.stack.top();

    const auto exponent_significant_bytes =
        static_cast<int>(intx::count_significant_bytes(exponent));
    const auto exponent_cost = state.rev >= EVMC_SPURIOUS_DRAGON ? 50 : 10;
    const auto additional_cost = exponent_significant_bytes * exponent_cost;
    if ((state.gas_left -= additional_cost) < 0)
        return RET(EVMC_OUT_OF_GAS);

    exponent = intx::exp(base, exponent);
    return RET(EVMC_SUCCESS);
}

inline void signextend(StackTop stack) noexcept
{
    const auto ext = stack.pop();
    auto& x = stack.top();

    if (ext < 31)
    {
        auto sign_bit = static_cast<int>(ext) * 8 + 7;
        auto sign_mask = uint256{1} << sign_bit;
        auto value_mask = sign_mask - 1;
        auto is_neg = (x & sign_mask) != 0;
        x = is_neg ? x | ~value_mask : x & value_mask;
    }
}

inline void lt(StackTop stack) noexcept
{
    const auto x = stack.pop();
    stack[0] = x < stack[0];
}

inline void gt(StackTop stack) noexcept
{
    const auto x = stack.pop();
    stack[0] = stack[0] < x;  // Arguments are swapped and < is used.
}

inline void slt(StackTop stack) noexcept
{
    const auto x = stack.pop();
    stack[0] = slt(x, stack[0]);
}

inline void sgt(StackTop stack) noexcept
{
    const auto x = stack.pop();
    stack[0] = slt(stack[0], x);  // Arguments are swapped and SLT is used.
}

inline void eq(StackTop stack) noexcept
{
    stack[1] = stack[0] == stack[1];
    stack.pop();
}

inline void iszero(StackTop stack) noexcept
{
    stack.top() = stack.top() == 0;
}

inline void and_(StackTop stack) noexcept
{
    stack.top() &= stack.pop();
}

inline void or_(StackTop stack) noexcept
{
    stack.top() |= stack.pop();
}

inline void xor_(StackTop stack) noexcept
{
    stack.top() ^= stack.pop();
}

inline void not_(StackTop stack) noexcept
{
    stack.top() = ~stack.top();
}

inline void byte(StackTop stack) noexcept
{
    const auto n = stack.pop();
    auto& x = stack.top();

    if (n > 31)
        x = 0;
    else
    {
        auto sh = (31 - static_cast<unsigned>(n)) * 8;
        auto y = x >> sh;
        x = y & 0xff;
    }
}

inline void shl(StackTop stack) noexcept
{
    stack.top() <<= stack.pop();
}

inline void shr(StackTop stack) noexcept
{
    stack.top() >>= stack.pop();
}

inline void sar(StackTop stack) noexcept
{
    if ((stack[1] & (uint256{1} << 255)) == 0)
        return shr(stack);

    constexpr auto allones = ~uint256{};

    if (stack[0] >= 256)
        stack[1] = allones;
    else
    {
        const auto shift = static_cast<unsigned>(stack[0]);
        stack[1] = (stack[1] >> shift) | (allones << (256 - shift));
    }

    stack.pop();
}

inline Result keccak256(
        ExecutionState& state) noexcept
{
    const auto index = state.stack.pop();
    auto& size = state.stack.top();

    if (!check_memory(state, index, size))
        return RET(EVMC_OUT_OF_GAS);

    const auto i = static_cast<size_t>(index);
    const auto s = static_cast<size_t>(size);
    const auto w = num_words(s);
    const auto cost = w * 6;
    if ((state.gas_left -= cost) < 0)
        return RET(EVMC_OUT_OF_GAS);

    auto data = s != 0 ? &state.memory[i] : nullptr;
    size = intx::be::load<uint256>(ethash::keccak256(data, s));
    return RET(EVMC_SUCCESS);
}


inline void address(
        ExecutionState& state) noexcept
{
    state.stack.push(intx::be::load<uint256>(state.msg->recipient));
}

inline Result balance(
        ExecutionState& state) noexcept
{
    auto& x = state.stack.top();
    const auto addr = intx::be::trunc<evmc::address>(x);

    if (state.rev >= EVMC_BERLIN && state.host.access_account(addr) == EVMC_ACCESS_COLD)
    {
        if ((state.gas_left -= instr::additional_cold_account_access_cost) < 0)
            return RET(EVMC_OUT_OF_GAS);
    }

    x = intx::be::load<uint256>(state.host.get_balance(addr));
    return RET(EVMC_SUCCESS);
}

inline void origin(
        ExecutionState& state) noexcept
{
    state.stack.push(intx::be::load<uint256>(state.host.get_tx_context().tx_origin));
}

inline void caller(
        ExecutionState& state) noexcept
{
    state.stack.push(intx::be::load<uint256>(state.msg->sender));
}

inline void callvalue(
        ExecutionState& state) noexcept
{
    state.stack.push(intx::be::load<uint256>(state.msg->value));
}

inline void calldataload(
        ExecutionState& state) noexcept
{
    auto& index = state.stack.top();

    if (state.msg->input_size < index)
        index = 0;
    else
    {
        const auto begin = static_cast<size_t>(index);
        const auto end = std::min(begin + 32, state.msg->input_size);

        uint8_t data[32] = {};
        for (size_t i = 0; i < (end - begin); ++i)
            data[i] = state.msg->input_data[begin + i];

        index = intx::be::load<uint256>(data);
    }
}

inline void calldatasize(
        ExecutionState& state) noexcept
{
    state.stack.push(state.msg->input_size);
}


inline Result calldatacopy(
        ExecutionState& state) noexcept
{
    const auto mem_index = state.stack.pop();
    const auto input_index = state.stack.pop();
    const auto size = state.stack.pop();

    if (!check_memory(state, mem_index, size))
        return RET(EVMC_OUT_OF_GAS);

    auto dst = static_cast<size_t>(mem_index);
    auto src = state.msg->input_size < input_index ? state.msg->input_size :
                                                     static_cast<size_t>(input_index);
    auto s = static_cast<size_t>(size);
    auto copy_size = std::min(s, state.msg->input_size - src);

    const auto copy_cost = num_words(s) * 3;
    if ((state.gas_left -= copy_cost) < 0)
        return RET(EVMC_OUT_OF_GAS);

    if (copy_size > 0)
        std::memcpy(&state.memory[dst], &state.msg->input_data[src], copy_size);

    if (s - copy_size > 0)
        std::memset(&state.memory[dst + copy_size], 0, s - copy_size);

    return RET(EVMC_SUCCESS);
}

inline void codesize(
        ExecutionState& state) noexcept
{
    state.stack.push(state.code.size());
}

inline Result codecopy(
        ExecutionState& state) noexcept
{
    // TODO: Similar to calldatacopy().

    const auto mem_index = state.stack.pop();
    const auto input_index = state.stack.pop();
    const auto size = state.stack.pop();

    if (!check_memory(state, mem_index, size))
        return RET(EVMC_OUT_OF_GAS);

    const auto code_size = state.code.size();
    const auto dst = static_cast<size_t>(mem_index);
    const auto src = code_size < input_index ? code_size : static_cast<size_t>(input_index);
    const auto s = static_cast<size_t>(size);
    const auto copy_size = std::min(s, code_size - src);

    const auto copy_cost = num_words(s) * 3;
    if ((state.gas_left -= copy_cost) < 0)
        return RET(EVMC_OUT_OF_GAS);

    // TODO: Add unit tests for each combination of conditions.
    if (copy_size > 0)
        std::memcpy(&state.memory[dst], &state.code[src], copy_size);

    if (s - copy_size > 0)
        std::memset(&state.memory[dst + copy_size], 0, s - copy_size);

    return RET(EVMC_SUCCESS);
}



inline void gasprice(
        ExecutionState& state) noexcept
{
    state.stack.push(intx::be::load<uint256>(state.host.get_tx_context().tx_gas_price));
}


inline void basefee(
        ExecutionState& state) noexcept
{
    state.stack.push(intx::be::load<uint256>(state.host.get_tx_context().block_base_fee));
}


inline Result extcodesize(
        ExecutionState& state) noexcept
{
    auto& x = state.stack.top();
    const auto addr = intx::be::trunc<evmc::address>(x);

    if (state.rev >= EVMC_BERLIN && state.host.access_account(addr) == EVMC_ACCESS_COLD)
    {
        if ((state.gas_left -= instr::additional_cold_account_access_cost) < 0)
            return RET(EVMC_OUT_OF_GAS);
    }

    x = state.host.get_code_size(addr);
    return RET(EVMC_SUCCESS);
}

inline Result extcodecopy(
        ExecutionState& state) noexcept
{
    const auto addr = intx::be::trunc<evmc::address>(state.stack.pop());
    const auto mem_index = state.stack.pop();
    const auto input_index = state.stack.pop();
    const auto size = state.stack.pop();

    if (!check_memory(state, mem_index, size))
        return RET(EVMC_OUT_OF_GAS);

    auto dst = static_cast<size_t>(mem_index);
    auto src = max_buffer_size < input_index ? max_buffer_size : static_cast<size_t>(input_index);
    auto s = static_cast<size_t>(size);

    const auto copy_cost = num_words(s) * 3;
    if ((state.gas_left -= copy_cost) < 0)
        return RET(EVMC_OUT_OF_GAS);

    if (state.rev >= EVMC_BERLIN && state.host.access_account(addr) == EVMC_ACCESS_COLD)
    {
        if ((state.gas_left -= instr::additional_cold_account_access_cost) < 0)
            return RET(EVMC_OUT_OF_GAS);
    }

    auto data = s != 0 ? &state.memory[dst] : nullptr;
    auto num_bytes_copied = state.host.copy_code(addr, src, data, s);
    if (s - num_bytes_copied > 0)
        std::memset(&state.memory[dst + num_bytes_copied], 0, s - num_bytes_copied);

    return RET(EVMC_SUCCESS);
}

inline void returndatasize(
        ExecutionState& state) noexcept
{
    state.stack.push(state.return_data.size());
}

inline Result returndatacopy(
        ExecutionState& state) noexcept
{
    const auto mem_index = state.stack.pop();
    const auto input_index = state.stack.pop();
    const auto size = state.stack.pop();

    if (!check_memory(state, mem_index, size))
        return RET(EVMC_OUT_OF_GAS);

    auto dst = static_cast<size_t>(mem_index);
    auto s = static_cast<size_t>(size);

    if (state.return_data.size() < input_index)
        return RET(EVMC_INVALID_MEMORY_ACCESS);
    auto src = static_cast<size_t>(input_index);

    if (src + s > state.return_data.size())
        return RET(EVMC_INVALID_MEMORY_ACCESS);

    const auto copy_cost = num_words(s) * 3;
    if ((state.gas_left -= copy_cost) < 0)
        return RET(EVMC_OUT_OF_GAS);

    if (s > 0)
        std::memcpy(&state.memory[dst], &state.return_data[src], s);

    return RET(EVMC_SUCCESS);
}

inline Result extcodehash(
        ExecutionState& state) noexcept
{
    auto& x = state.stack.top();
    const auto addr = intx::be::trunc<evmc::address>(x);

    if (state.rev >= EVMC_BERLIN && state.host.access_account(addr) == EVMC_ACCESS_COLD)
    {
        if ((state.gas_left -= instr::additional_cold_account_access_cost) < 0)
            return RET(EVMC_OUT_OF_GAS);
    }

    x = intx::be::load<uint256>(state.host.get_code_hash(addr));
    return RET(EVMC_SUCCESS);
}


inline void blockhash(
        ExecutionState& state) noexcept
{
    auto& number = state.stack.top();

    const auto upper_bound = state.host.get_tx_context().block_number;
    const auto lower_bound = std::max(upper_bound - 256, decltype(upper_bound){0});
    const auto n = static_cast<int64_t>(number);
    const auto header =
        (number < upper_bound && n >= lower_bound) ? state.host.get_block_hash(n) : evmc::bytes32{};
    number = intx::be::load<uint256>(header);
}

inline void coinbase(
        ExecutionState& state) noexcept
{
    state.stack.push(intx::be::load<uint256>(state.host.get_tx_context().block_coinbase));
}

inline void timestamp(
        ExecutionState& state) noexcept
{
    // TODO: Add tests for negative timestamp?
    const auto timestamp = static_cast<uint64_t>(state.host.get_tx_context().block_timestamp);
    state.stack.push(timestamp);
}

inline void number(
        ExecutionState& state) noexcept
{
    // TODO: Add tests for negative block number?
    const auto block_number = static_cast<uint64_t>(state.host.get_tx_context().block_number);
    state.stack.push(block_number);
}

inline void difficulty(
        ExecutionState& state) noexcept
{
    state.stack.push(intx::be::load<uint256>(state.host.get_tx_context().block_prev_randao));
}

inline void gaslimit(
        ExecutionState& state) noexcept
{
    const auto block_gas_limit = static_cast<uint64_t>(state.host.get_tx_context().block_gas_limit);
    state.stack.push(block_gas_limit);
}

inline void chainid(
        ExecutionState& state) noexcept
{
    state.stack.push(intx::be::load<uint256>(state.host.get_tx_context().chain_id));
}

inline void selfbalance(
        ExecutionState& state) noexcept
{
    // TODO: introduce selfbalance in EVMC?
    state.stack.push(intx::be::load<uint256>(state.host.get_balance(state.msg->recipient)));
}

inline Result mload(
        ExecutionState& state) noexcept
{
    auto& index = state.stack.top();

    if (!check_memory(state, index, 32))
        return RET(EVMC_OUT_OF_GAS);

    index = intx::be::unsafe::load<uint256>(&state.memory[static_cast<size_t>(index)]);
    return RET(EVMC_SUCCESS);
}

inline Result mstore(
        ExecutionState& state) noexcept
{
    const auto index = state.stack.pop();
    const auto value = state.stack.pop();

    if (!check_memory(state, index, 32))
        return RET(EVMC_OUT_OF_GAS);

    intx::be::unsafe::store(&state.memory[static_cast<size_t>(index)], value);
    return RET(EVMC_SUCCESS);
}

inline Result mstore8(
        ExecutionState& state) noexcept
{
    const auto index = state.stack.pop();
    const auto value = state.stack.pop();

    if (!check_memory(state, index, 1))
        return RET(EVMC_OUT_OF_GAS);

    state.memory[static_cast<size_t>(index)] = static_cast<uint8_t>(value);
    return RET(EVMC_SUCCESS);
}

inline void msize(
        ExecutionState& state) noexcept
{
    state.stack.push(state.memory.size());
}



/// DUP instruction implementation.
/// @tparam N  The number as in the instruction definition, e.g. DUP3 is dup<3>.
template <size_t N>
inline void dup(StackTop stack) noexcept
{
    static_assert(N >= 1 && N <= 16);
    stack.push(stack[N - 1]);
}

/// SWAP instruction implementation.
/// @tparam N  The number as in the instruction definition, e.g. SWAP3 is swap<3>.
template <size_t N>
inline void swap(StackTop stack) noexcept
{
    static_assert(N >= 1 && N <= 16);
    std::swap(stack.top(), stack[N]);
}



template <size_t NumTopics>
inline Result log(
        ExecutionState& state) noexcept
{
    static_assert(NumTopics >= 0 && NumTopics <= 4);

    if (state.msg->flags & EVMC_STATIC)
        return RET(EVMC_STATIC_MODE_VIOLATION);

    const auto offset = state.stack.pop();
    const auto size = state.stack.pop();

    if (!check_memory(state, offset, size))
        return RET(EVMC_OUT_OF_GAS);

    const auto o = static_cast<size_t>(offset);
    const auto s = static_cast<size_t>(size);

    const auto cost = int64_t(s) * 8;
    if ((state.gas_left -= cost) < 0)
        return RET(EVMC_OUT_OF_GAS);

    std::array<evmc::bytes32, NumTopics> topics;
    for (auto& topic : topics)
        topic = intx::be::store<evmc::bytes32>(state.stack.pop());

    const auto data = s != 0 ? &state.memory[o] : nullptr;
    state.host.emit_log(state.msg->recipient, data, s, topics.data(), NumTopics);
    return RET(EVMC_SUCCESS);
}

inline TermResult selfdestruct(
        ExecutionState& state) noexcept
{
    if (state.msg->flags & EVMC_STATIC)
        return RET(EVMC_STATIC_MODE_VIOLATION);

    const auto beneficiary = intx::be::trunc<evmc::address>(state.stack[0]);

    if (state.rev >= EVMC_BERLIN && state.host.access_account(beneficiary) == EVMC_ACCESS_COLD)
    {
        if ((state.gas_left -= instr::cold_account_access_cost) < 0)
            return RET(EVMC_OUT_OF_GAS);
    }

    if (state.rev >= EVMC_TANGERINE_WHISTLE)
    {
        if (state.rev == EVMC_TANGERINE_WHISTLE || state.host.get_balance(state.msg->recipient))
        {
            // After TANGERINE_WHISTLE apply additional cost of
            // sending value to a non-existing account.
            if (!state.host.account_exists(beneficiary))
            {
                if ((state.gas_left -= 25000) < 0)
                    return RET(EVMC_OUT_OF_GAS);
            }
        }
    }

    state.host.selfdestruct(state.msg->recipient, beneficiary);
    return RET(EVMC_SUCCESS);
}

Result
sload(
    ExecutionState& state) noexcept;

Result 
sstore(
        ExecutionState& state) noexcept;

template <evmc_call_kind Kind, bool Static = false>
Result call (ExecutionState& state) noexcept;
// new_in_evmone_v2
// using code_iterator = const uint8_t*;
// template <Opcode Op> Result call_impl( StackTop stack, int64_t gas_left, ExecutionState& state) noexcept;
// inline constexpr auto call = call_impl<OP_CALL>;
// inline constexpr auto callcode = call_impl<OP_CALLCODE>;
// inline constexpr auto delegatecall = call_impl<OP_DELEGATECALL>;
// inline constexpr auto staticcall = call_impl<OP_STATICCALL>;
// new_in_evmone_v2 end

template <evmc_call_kind Kind>
Result create(
        ExecutionState& state) noexcept;
// new_in_evmone_v2
// template <Opcode Op>
// Result create_impl(
//         StackTop stack, int64_t gas_left, ExecutionState& state) noexcept;
// inline constexpr auto create = create_impl<OP_CREATE>;
// inline constexpr auto create2 = create_impl<OP_CREATE2>;
// new_in_evmone_v2 end

template <evmc_status_code StatusCode>
inline void return_(
        ExecutionState& state) noexcept
{
    const auto offset = state.stack[0];
    const auto size = state.stack[1];

    if (!check_memory(state, offset, size))
    {
        state.status = EVMC_OUT_OF_GAS;
        return;
    }

    state.output_offset = static_cast<size_t>(offset);  // Can be garbage if size is 0.
    state.output_size = static_cast<size_t>(size);
    state.status = StatusCode;
}
// new_in_evmone_v2
// inline constexpr auto return_ = return_impl<EVMC_SUCCESS>;
// inline constexpr auto revert = return_impl<EVMC_REVERT>;
// new_in_evmone_v2

inline void pop(StackTop stack) noexcept
{
    stack.pop();
}
// new_in_evmone_v2
// inline void noop(StackTop /*stack*/) noexcept {}
// inline constexpr auto pop = noop;
// inline constexpr auto jumpdest = noop;
// new_in_evmone_v2 end

// new_in_evmone_v2
// inline code_iterator callf(
//         StackTop stack, ExecutionState& state, code_iterator pos) noexcept
// {
//     const auto index = read_uint16_be(&pos[1]);
//     const auto& header = state.analysis.baseline->eof_header;
//     const auto stack_size = &stack.top() - state.stack_space.bottom();
// 
//     const auto callee_required_stack_size =
//         header.types[index].max_stack_height - header.types[index].inputs;
//     if (stack_size + callee_required_stack_size > StackSpace::limit)
//     {
//         state.status = EVMC_STACK_OVERFLOW;
//         return nullptr;
//     }
// 
//     if (state.call_stack.size() >= StackSpace::limit)
//     {
//         // TODO: Add different error code.
//         state.status = EVMC_STACK_OVERFLOW;
//         return nullptr;
//     }
//     state.call_stack.push_back(pos + 3);
// 
//     const auto offset = header.code_offsets[index] - header.code_offsets[0];
//     auto code = state.analysis.baseline->executable_code;
//     return code.data() + offset;
// }
// 
// inline code_iterator retf(StackTop /*stack*/, ExecutionState& state, code_iterator /*pos*/) noexcept
// {
//     const auto p = state.call_stack.back();
//     state.call_stack.pop_back();
//     return p;
// }
// 
// inline code_iterator jumpf(
//         StackTop stack, ExecutionState& state, code_iterator pos) noexcept
// {
//     const auto index = read_uint16_be(&pos[1]);
//     const auto& header = state.analysis.baseline->eof_header;
//     const auto stack_size = &stack.top() - state.stack_space.bottom();
// 
//     const auto callee_required_stack_size =
//         header.types[index].max_stack_height - header.types[index].inputs;
//     if (stack_size + callee_required_stack_size > StackSpace::limit)
//     {
//         state.status = EVMC_STACK_OVERFLOW;
//         return nullptr;
//     }
// 
//     const auto offset = header.code_offsets[index] - header.code_offsets[0];
//     const auto code = state.analysis.baseline->executable_code;
//     return code.data() + offset;
// }
// 
// /// Internal jump implementation for JUMP/JUMPI instructions.
// inline code_iterator jump_impl(ExecutionState& state, const uint256& dst) noexcept
// {
//     const auto& jumpdest_map = state.analysis.baseline->jumpdest_map;
//     if (dst >= jumpdest_map.size() || !jumpdest_map[static_cast<size_t>(dst)])
//     {
//         state.status = EVMC_BAD_JUMP_DESTINATION;
//         return nullptr;
//     }
// 
//     return &state.analysis.baseline->executable_code[static_cast<size_t>(dst)];
// }
// 
// /// JUMP instruction implementation using baseline::CodeAnalysis.
// inline code_iterator jump(
//         StackTop stack, ExecutionState& state, code_iterator /*pos*/) noexcept
// {
//     return jump_impl(state, stack.pop());
// }
// 
// /// JUMPI instruction implementation using baseline::CodeAnalysis.
// inline code_iterator jumpi(
//         StackTop stack, ExecutionState& state, code_iterator pos) noexcept
// {
//     const auto& dst = stack.pop();
//     const auto& cond = stack.pop();
//     return cond ? jump_impl(state, dst) : pos + 1;
// }
// 
// inline code_iterator rjump(StackTop /*stack*/, ExecutionState& /*state*/, code_iterator pc) noexcept
// {
//     // Reading next 2 bytes is guaranteed to be safe by deploy-time validation.
//     const auto offset = read_int16_be(&pc[1]);
//     return pc + 3 + offset;  // PC_post_rjump + offset
// }
// 
// inline code_iterator rjumpi(
//         StackTop stack, ExecutionState& state, code_iterator pc) noexcept
// {
//     const auto cond = stack.pop();
//     return cond ? rjump(stack, state, pc) : pc + 3;
// }
// 
// inline code_iterator rjumpv(
//         StackTop stack, ExecutionState& /*state*/, code_iterator pc) noexcept
// {
//     constexpr auto REL_OFFSET_SIZE = sizeof(int16_t);
//     const auto case_ = stack.pop();
// 
//     const auto max_index = pc[1];
//     const auto pc_post = pc + 1 + 1 /* max_index */ + (max_index + 1) * REL_OFFSET_SIZE /* tbl */;
// 
//     if (case_ > max_index)
//     {
//         return pc_post;
//     }
//     else
//     {
//         const auto rel_offset =
//             read_int16_be(&pc[2 + static_cast<uint16_t>(case_) * REL_OFFSET_SIZE]);
// 
//         return pc_post + rel_offset;
//     }
// }
// 
// inline code_iterator pc(
//         StackTop stack, ExecutionState& state, code_iterator pos) noexcept
// {
//     stack.push(static_cast<uint64_t>(pos - state.analysis.baseline->executable_code.data()));
//     return pos + 1;
// }
// 
// inline Result gas(
//         StackTop stack, int64_t gas_left, ExecutionState& /*state*/) noexcept
// {
//     stack.push(gas_left);
//     return {EVMC_SUCCESS, gas_left};
// }
// 
// inline void tload(
//         StackTop stack, ExecutionState& state) noexcept
// {
//     auto& x = stack.top();
//     const auto key = intx::be::store<evmc::bytes32>(x);
//     const auto value = state.host.get_transient_storage(state.msg->recipient, key);
//     x = intx::be::load<uint256>(value);
// }
// 
// inline Result tstore(
//         StackTop stack, int64_t gas_left, ExecutionState& state) noexcept
// {
//     if (state.in_static_mode())
//         return {EVMC_STATIC_MODE_VIOLATION, 0};
// 
//     const auto key = intx::be::store<evmc::bytes32>(stack.pop());
//     const auto value = intx::be::store<evmc::bytes32>(stack.pop());
//     state.host.set_transient_storage(state.msg->recipient, key, value);
//     return {EVMC_SUCCESS, gas_left};
// }
// 
// inline void push0(
//         StackTop stack) noexcept
// {
//     stack.push({});
// }
// 
// 
// template <size_t Len>
// inline uint64_t load_partial_push_data(code_iterator pos) noexcept
// {
//     static_assert(Len > 4 && Len < 8);
// 
//     // It loads up to 3 additional bytes.
//     return intx::be::unsafe::load<uint64_t>(pos) >> (8 * (sizeof(uint64_t) - Len));
// }
// 
// template <>
// inline uint64_t load_partial_push_data<1>(code_iterator pos) noexcept
// {
//     return pos[0];
// }
// 
// template <>
// inline uint64_t load_partial_push_data<2>(code_iterator pos) noexcept
// {
//     return intx::be::unsafe::load<uint16_t>(pos);
// }
// 
// template <>
// inline uint64_t load_partial_push_data<3>(code_iterator pos) noexcept
// {
//     // It loads 1 additional byte.
//     return intx::be::unsafe::load<uint32_t>(pos) >> 8;
// }
// 
// template <>
// inline uint64_t load_partial_push_data<4>(code_iterator pos) noexcept
// {
//     return intx::be::unsafe::load<uint32_t>(pos);
// }
// 
// /// PUSH instruction implementation.
// /// @tparam Len The number of push data bytes, e.g. PUSH3 is push<3>.
// ///
// /// It assumes that at lest 32 bytes of data are available so code padding is required.
// template <size_t Len>
// inline code_iterator push(
//         StackTop stack, ExecutionState& /*state*/, code_iterator pos) noexcept
// {
//     constexpr auto num_full_words = Len / sizeof(uint64_t);
//     constexpr auto num_partial_bytes = Len % sizeof(uint64_t);
//     auto data = pos + 1;
// 
//     stack.push(0);
//     auto& r = stack.top();
// 
//     // Load top partial word.
//     if constexpr (num_partial_bytes != 0)
//     {
//         r[num_full_words] = load_partial_push_data<num_partial_bytes>(data);
//         data += num_partial_bytes;
//     }
// 
//     // Load full words.
//     for (size_t i = 0; i < num_full_words; ++i)
//     {
//         r[num_full_words - 1 - i] = intx::be::unsafe::load<uint64_t>(data);
//         data += sizeof(uint64_t);
//     }
// 
//     return pos + (Len + 1);
// }
// 
// inline code_iterator dupn(
//         StackTop stack, ExecutionState& state, code_iterator pos) noexcept
// {
//     const auto n = pos[1] + 1;
// 
//     const auto stack_size = &stack.top() - state.stack_space.bottom();
// 
//     if (stack_size < n)
//     {
//         state.status = EVMC_STACK_UNDERFLOW;
//         return nullptr;
//     }
// 
//     stack.push(stack[n - 1]);
// 
//     return pos + 2;
// }
// 
// inline code_iterator swapn(
//         StackTop stack, ExecutionState& state, code_iterator pos) noexcept
// {
//     const auto n = pos[1] + 1;
// 
//     const auto stack_size = &stack.top() - state.stack_space.bottom();
// 
//     if (stack_size <= n)
//     {
//         state.status = EVMC_STACK_UNDERFLOW;
//         return nullptr;
//     }
// 
//     // TODO: This may not be optimal, see instr::core::swap().
//     std::swap(stack.top(), stack[n]);
// 
//     return pos + 2;
// }
// 
// inline Result mcopy(
//         StackTop stack, int64_t gas_left, ExecutionState& state) noexcept
// {
//     const auto& dst_u256 = stack.pop();
//     const auto& src_u256 = stack.pop();
//     const auto& size_u256 = stack.pop();
// 
//     if (!check_memory(gas_left, state.memory, std::max(dst_u256, src_u256), size_u256))
//         return {EVMC_OUT_OF_GAS, gas_left};
// 
//     const auto dst = static_cast<size_t>(dst_u256);
//     const auto src = static_cast<size_t>(src_u256);
//     const auto size = static_cast<size_t>(size_u256);
// 
//     if (const auto cost = copy_cost(size); (gas_left -= cost) < 0)
//         return {EVMC_OUT_OF_GAS, gas_left};
// 
//     if (size > 0)
//         std::memmove(&state.memory[dst], &state.memory[src], size);
// 
//     return {EVMC_SUCCESS, gas_left};
// }
// 
// inline void dataload(
//         StackTop stack, ExecutionState& state) noexcept
// {
//     auto& index = stack.top();
// 
//     if (state.data.size() < index)
//         index = 0;
//     else
//     {
//         const auto begin = static_cast<size_t>(index);
//         const auto end = std::min(begin + 32, state.data.size());
// 
//         uint8_t data[32] = {};
//         for (size_t i = 0; i < (end - begin); ++i)
//             data[i] = state.data[begin + i];
// 
//         index = intx::be::unsafe::load<uint256>(data);
//     }
// }
// 
// inline void datasize(
//         StackTop stack, ExecutionState& state) noexcept
// {
//     stack.push(state.data.size());
// }
// 
// inline code_iterator dataloadn(
//         StackTop stack, ExecutionState& state, code_iterator pos) noexcept
// {
//     const auto index = read_uint16_be(&pos[1]);
// 
//     stack.push(intx::be::unsafe::load<uint256>(&state.data[index]));
//     return pos + 3;
// }
// 
// inline Result datacopy(
//         StackTop stack, int64_t gas_left, ExecutionState& state) noexcept
// {
//     const auto& mem_index = stack.pop();
//     const auto& data_index = stack.pop();
//     const auto& size = stack.pop();
// 
//     if (!check_memory(gas_left, state.memory, mem_index, size))
//         return {EVMC_OUT_OF_GAS, gas_left};
// 
//     const auto dst = static_cast<size_t>(mem_index);
//     // TODO why?
//     const auto src =
//         state.data.size() < data_index ? state.data.size() : static_cast<size_t>(data_index);
//     const auto s = static_cast<size_t>(size);
//     const auto copy_size = std::min(s, state.data.size() - src);
// 
//     if (const auto cost = copy_cost(s); (gas_left -= cost) < 0)
//         return {EVMC_OUT_OF_GAS, gas_left};
// 
//     if (copy_size > 0)
//         std::memcpy(&state.memory[dst], &state.data[src], copy_size);
// 
//     if (s - copy_size > 0)
//         std::memset(&state.memory[dst + copy_size], 0, s - copy_size);
// 
//     return {EVMC_SUCCESS, gas_left};
// }
// 
// 
// inline void blobhash(
//         StackTop stack, ExecutionState& state) noexcept
// {
//     auto& index = stack.top();
//     const auto& tx = state.get_tx_context();
// 
//     index = (index < tx.blob_hashes_count) ?
//                 intx::be::load<uint256>(tx.blob_hashes[static_cast<size_t>(index)]) :
//                 0;
// }
// 
// inline void blobbasefee(
//         StackTop stack, ExecutionState& state) noexcept
// {
//     stack.push(intx::be::load<uint256>(state.get_tx_context().blob_base_fee));
// }
// 
// template <evmc_status_code Status>
// inline TermResult stop_impl(
//     StackTop /*stack*/, int64_t gas_left, ExecutionState& /*state*/) noexcept
// {
//     return {Status, gas_left};
// }
// inline constexpr auto stop = stop_impl<EVMC_SUCCESS>;
// inline constexpr auto invalid = stop_impl<EVMC_INVALID_INSTRUCTION>;
// template <Opcode Op>
// inline constexpr auto impl = nullptr;
// #undef ON_OPCODE_IDENTIFIER
// #define ON_OPCODE_IDENTIFIER(OPCODE, IDENTIFIER) \
//     template <>                                  \
//     inline constexpr auto impl<OPCODE> = IDENTIFIER;  // opcode -> implementation
// MAP_OPCODES
// #undef ON_OPCODE_IDENTIFIER
// #define ON_OPCODE_IDENTIFIER ON_OPCODE_IDENTIFIER_DEFAULT
// new_in_evmone_v2 end

}  // namespace
