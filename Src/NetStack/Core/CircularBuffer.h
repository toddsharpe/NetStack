#pragma once

#include <cstddef>
#include <array>

template <typename T, size_t N>
class CircularBuffer
{
public:
	bool push(T push)
	{
		if (is_full())
			return false;

		entries[tail] = push;
		tail = (tail + 1) % entries.size();
		count++;
		return true;
	}

	void pop()
	{
		_ASSERT(!is_empty());
		head = (head + 1) % entries.size();
		count--;
	}

	T& peek()
	{
		_ASSERT(!is_empty());
		return entries[head];
	}

	bool is_full() const
	{
		return count == entries.size();
	}

	bool is_empty() const
	{
		return count == 0;
	}

private:
	std::array<T, N> entries;
	size_t count;
	size_t head;
	size_t tail;
};
