#pragma once

#include <array>

template <typename TOut, typename TIn, size_t N>
constexpr std::array<TOut, N> make_array(const std::array<TIn, N>& input)
{
	std::array<TOut, N> output;
	for (size_t i = 0; i < output.size(); i++)
		output[i] = input[i]

	return output;
}
