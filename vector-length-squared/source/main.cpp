#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <immintrin.h>

constexpr auto cache_line_num_bytes = 64;

class GenericAllocator
{
public:
	static constexpr auto maximum_alignment = cache_line_num_bytes;

	GenericAllocator() = default;

	static char* allocate(const std::size_t size)
	{
		const auto num_blocks = (size + sizeof(Block) - 1) / sizeof(Block);
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
		return reinterpret_cast<Type*>(GenericAllocator::allocate(size*sizeof(Type)));
	}
	void deallocate(Type*const allocation, const std::size_t size) noexcept
	{
		GenericAllocator::deallocate(reinterpret_cast<char*>(allocation), size*sizeof(Type));
	}
};

template<typename Type>
using DynamicArray = std::vector<Type, Allocator<Type>>;

int main(const int num_arguments, char** arguments)
{
	constexpr auto seed = std::size_t{860374};
	constexpr auto num_elements = std::size_t{1'000'000};
	constexpr auto num_iterations = std::size_t{100};
	constexpr auto min_value = -1000.0f;
	constexpr auto max_value = 1000.0f;
	constexpr auto num_vector_components = 3;
	
	auto input = DynamicArray<float>(num_elements*num_vector_components);
	auto rng_engine = std::mt19937_64{seed};
	const auto random_float = std::uniform_real_distribution{min_value, max_value};
	for(auto& element : input)
	{
		element = random_float(rng_engine);
	}

	std::cout << "vector-length-squared test!\n";

	auto output = DynamicArray<float>(input.size()/num_vector_components);

	const auto start_time = std::chrono::steady_clock::now();

#if defined SIMD
	using Vector = __m256;
	constexpr auto vector_size = sizeof(Vector)/sizeof(float);
	for(auto iteration_index = std::size_t{}; iteration_index < num_iterations; ++iteration_index)
	{
		for(auto element_index = std::size_t{}; element_index < output.size(); element_index += vector_size)
		{
			auto length_squared = _mm256_setzero_ps();
			for(auto component_index = std::size_t{}; component_index < num_vector_components; ++component_index)
			{
				Vector component;
				for(auto vector_index = std::size_t{}; vector_index < vector_size; ++vector_index)
				{
					reinterpret_cast<float(&)[vector_size]>(component)[vector_index] = input[(element_index + vector_index) * 3 + component_index];
				}
				length_squared = _mm256_fmadd_ps(component, component, length_squared);
			}
			_mm256_stream_ps(output.data() + element_index, length_squared);
		}
	}
#else
	for(auto iteration_index = std::size_t{}; iteration_index < num_iterations; ++iteration_index)
	{
		for(auto element_index = std::size_t{}; element_index < output.size(); ++element_index)
		{
			auto length_squared = float{};
			for(auto component_index = std::size_t{}; component_index < num_vector_components; ++component_index)
			{
				const auto component = input[element_index*num_vector_components + component_index];
				length_squared += component * component;
			}
			output[element_index] = length_squared;
		}
	}
#endif

	const auto end_time = std::chrono::steady_clock::now();
	
	const auto duration = end_time - start_time;
	const auto duration_in_seconds = std::chrono::duration<double>{duration}.count();

	std::cout << "Done!\n";
	std::cout << "First element: " << output.front() << '\n';
	std::cout << "Last element: " << output.back() << '\n';
	std::cout << "Time taken: " << duration_in_seconds << "s\n";
	return EXIT_SUCCESS;
}
