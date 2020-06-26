#include "swe.hpp"
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <gtest/gtest.h>
#include <iomanip>

using RiemannSolverFunction = auto (*)(
								  double* fL,
								  double* fR,
								  const double* qL,
								  const double* qR,
								  int direction,
								  double grav,
								  double epsilon) -> double;

struct NamedRSFunction
{
	RiemannSolverFunction f;
	std::string_view name;
};
constexpr NamedRSFunction solvers[] = {
	{swe::originalRiemannSolver, "origRS"},
	{swe::riemannSolver, "myRS"},
	{swe::samoaRiemannSolver, "samRS"},
};

constexpr double grav = 9.81;
constexpr double dryTolerance = 1e-3;

constexpr int floatWidth = 11;

struct Result
{
	/// Fluxes in cells.
	glm::dvec4 cells[2];
	double maxSpeed;
};

struct TestInput
{
	/// Initial conserved values in cells.
	glm::dvec4 cells[2];
};

auto floatW = std::setw(11);

auto operator<<(std::ostream& os, glm::vec4 v) -> std::ostream&
{
	return os << "(" << floatW << v.x << ",\t" << floatW << v.y << ",\t"
			  << floatW << v.z << ",\t" << floatW << v.w << ")";
}
auto operator<<(std::ostream& os, Result r) -> std::ostream&
{
	return os << "Result{\t" << r.cells[0] << ",\t" << r.cells[1] << ",\t"
			  << r.maxSpeed << "}";
}
auto operator<<(std::ostream& os, TestInput i) -> std::ostream&
{
	return os << "Input{\t" << i.cells[0] << ",\t" << i.cells[1] << "}";
}

auto callSolver(TestInput input, int direction, RiemannSolverFunction f)
	-> Result
{
	Result result;
	result.maxSpeed
		= f(glm::value_ptr(result.cells[0]),
			glm::value_ptr(result.cells[1]),
			glm::value_ptr(input.cells[0]),
			glm::value_ptr(input.cells[1]),
			direction,
			grav,
			dryTolerance);
	return result;
}

template<class T>
auto mirrored(T t) -> T
{
	auto negUV = [](glm::dvec4 v) { return glm::dvec4{v.x, -v.y, -v.z, v.w}; };
	return {{negUV(t.cells[1]), negUV(t.cells[0])}};
}

auto swappedUV(TestInput r) -> TestInput
{
	std::swap(r.cells[0].y, r.cells[0].z);
	std::swap(r.cells[1].y, r.cells[1].z);
	return r;
}

struct RiemannSolversF: ::testing::Test
{
	std::vector<TestInput> dynInputs;
	template<class F>
	auto forEach(F&& f)
	{
		constexpr double values[] = {-1, 0, 1};
		// clang-format off
		// For all ${{v, w} : v,w \in \mathbb{Z}_3^4}$
		for(auto b1:  {0, 1, 2}) {
		for(auto h1:  {0, 1, 2}) {
		for(auto hu1: {0, 1, 2}) {
		for(auto hv1: {0, 1, 2}) {
		for(auto b2  = b1;  b2  < 3; b2++) {
		for(auto h2  = h1;  h2  < 3; h2++) {
		for(auto hu2 = hu1; hu2 < 3; hu2++) {
		for(auto hv2 = hv1; hv2 < 3; hv2++) {
			TestInput i = {{
				{values[h1], values[hu1], values[hv1], values[b1]},
				{values[h2], values[hu2], values[hv2], values[b2]},
			}};
			
			f(i);

		}}}}
		}}}}
		// clang-format on
	}

	std::vector<TestInput> moreInputs;
	static constexpr TestInput inputs[] = {
		{{{0, 0, 0, 0}, {1, 0, 0, 0}}},
		{{{0, 0, 0, 0}, {1, 1, 0, 0}}},
		{{{0, 0, 0, 0}, {1, -1, 0, 0}}},
		{{{0, 0, 0, 0}, {1, 0, 1, 0}}},
		{{{0, 0, 0, 0}, {1, 0, -1, 0}}},
		{{{0, 0, 0, 0}, {1, 1, 1, 0}}},
		{{{0, 0, 0, 0}, {1, -1, -1, 0}}},
	};
};

TEST(RiemannSolvers, Linking)
{
	Result r = callSolver({}, 0, swe::samoaRiemannSolver);
}

/// riemann(l, r) should produce equivalent results to
/// riemann(r, l).
TEST_F(RiemannSolversF, SymmetryAroundWall)
{
	for(auto i: inputs) {
		auto mi = mirrored(i);
		std::cout << std::setprecision(6) << std::fixed << "\nInput:\t" << i
				  << "\n"
					 "\t"
				  << mi << '\n';

		for(auto s: solvers) {
			auto r1 = callSolver(i, 0, s.f);
			auto r2 = callSolver(mi, 0, s.f);

			std::cout << s.name << ":\t" << r1
					  << "\n"
						 "\t"
					  << r2 << "\n";
		}
		std::cout << '\n';
	}
	FAIL();
}

TEST_F(RiemannSolversF, SymmetryUV)
{
	for(auto i: inputs) {
		auto si = swappedUV(i);
		std::cout << std::setprecision(6) << std::fixed << "\nInput:\t" << i
				  << "\n"
					 "\t"
				  << si << '\n';
		for(auto s: solvers) {
			auto r1 = callSolver(i, 0, s.f);
			auto r2 = callSolver(si, 1, s.f);

			std::cout << s.name << ":\t" << r1
					  << "\n"
						 "\t"
					  << r2 << "\n";
		}
		std::cout << '\n';
	}
}

TEST_F(RiemannSolversF, Comparisons)
{
	for(auto i: moreInputs) {
		std::cout << std::setprecision(6) << std::fixed << "\nInput:\t" << i
				  << "\n";

		for(auto s: solvers) {
			auto r = callSolver(i, 0, s.f);

			std::cout << s.name << ":\t" << r << "\n";
		}
		std::cout << '\n';
	}
	FAIL();
}

TEST_F(RiemannSolversF, Extensive)
{
	auto f = [](auto i) {
		std::cout << std::setprecision(6) << std::fixed << "\nInput:\t" << i
				  << "\n";

		for(auto s: solvers) {
			auto r = callSolver(i, 0, s.f);

			std::cout << s.name << ":\t" << r << "\n";
		}
	};
	forEach(f);

	FAIL();
}
