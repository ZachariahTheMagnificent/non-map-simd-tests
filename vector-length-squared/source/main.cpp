#include <iostream>
#include <vector>

constexpr auto cache_line_num_bytes = 64;

class GenericAllocator
{
public:
	static constexpr auto maximum_alignment = cache_line_num_bytes;

	GenericAllocator() = default;

	static char* allocate(const std::size_t size)
	{
		const auto num_blocks = size / sizeof(Block) * sizeof(Block);
		return reinterpret_cast<char*>(new Block[num_blocks]);
	}
	static void deallocate(char*const allocation, const std::size_t size) noexcept
	{
		delete[] reinterpret_cast<Block*>(allocation);
	}

private:
	struct Block
	{
		alignas(cache_line_num_bytes) char bytes[cache_line_num_bytes];
	};
};

template<typename Type>
class Allocator
{
public:
	static_assert(alignof(Type) < GenericAllocator::maximum_alignment, "The allocator type has too high of an alignment requirement!");

	using value_type = Type;

	Allocator() = default;
	template<typename OtherType>
	constexpr Allocator(const Allocator<OtherType>&) noexcept
	{

	}

	template<typename OtherType>
	constexpr bool operator==(const Allocator<OtherType>&) const noexcept
	{
		return true;
	}
	template<typename OtherType>
	constexpr bool operator!=(const Allocator<OtherType>&) const noexcept
	{
		return false;
	}

	Type* allocate(const std::size_t size)
	{
		return reinterpret_cast<Type*>(GenericAllocator::allocate(size));
	}
	void deallocate(Type*const allocation, const std::size_t size) noexcept
	{
		GenericAllocator::deallocate(reinterpret_cast<char*>(allocation), size);
	}
};

template<typename Type>
using DynamicArray = std::vector<Type, Allocator<Type>>;

int main(const int num_arguments, char** arguments)
{
	DynamicArray<float> vectors;
	std::cout << "Hello world!\n";
	return EXIT_SUCCESS;
}
