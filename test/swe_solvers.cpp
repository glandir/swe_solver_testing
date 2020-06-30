#include "swe.hpp"
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <gtest/gtest.h>
#include <iomanip>

// Test Helpers
namespace {

struct RiemannSolver
{
	using F = decltype(&swe::riemannSolver);

	F f;
	std::string_view name;
};
constexpr RiemannSolver solvers[] = {
	{swe::originalRiemannSolver, "origRS"},
	{swe::riemannSolver, "myRS"},
	{swe::samoaRiemannSolver, "samRS"},
};

constexpr double grav = 9.81;
constexpr double dryTolerance = 1e-3;

struct TestResult
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
auto operator<<(std::ostream& os, TestResult r) -> std::ostream&
{
	return os << "Result{\t" << r.cells[0] << ",\t" << r.cells[1] << ",\t"
			  << r.maxSpeed << "}";
}
auto operator<<(std::ostream& os, TestInput i) -> std::ostream&
{
	return os << "Input{\t" << i.cells[0] << ",\t" << i.cells[1] << "}";
}

auto callSolver(TestInput input, int direction, RiemannSolver::F f)
	-> TestResult
{
	TestResult result;
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

} // namespace

struct RiemannSolversF: ::testing::Test
{
	std::vector<TestInput> allCases;
	RiemannSolversF()
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
			allCases.push_back(i);
		}}}}
		}}}}
		// clang-format on
	}

	static constexpr TestInput simpleCases[] = {
		{{{1, 0, 0, 0}, {1, 0, 0, 0}}},
		{{{1, 0, 0, 0}, {0.5, 0, 0, 0}}},
		{{{1, 1, 1, 0}, {1, 1, 1, 0}}},
		{{{0, 1, 1, 1}, {0, 1, 1, 1}}},
		{{{1, 1, 1, 1}, {1, 1, 1, 1}}},
	};
};

TEST_F(RiemannSolversF, Simple)
{
	for(auto i: simpleCases) {
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
	for(auto input: allCases) {
		std::cout << std::setprecision(6) << std::fixed << "\nInput:\t" << input
				  << "\n";
		TestResult results[3];
		for(auto i = 0U; i < 3; ++i) {
			results[i] = callSolver(input, 0, solvers[i].f);

			std::cout << solvers[i].name << ":\t" << results[i] << "\n";

			ASSERT_DOUBLE_EQ(0.0, results[i].cells[0].w);
			ASSERT_DOUBLE_EQ(0.0, results[i].cells[1].w);
		}

		for(auto j: {0, 1}) {
			// These happen most of the time, not always though.
			// ASSERT_DOUBLE_EQ(results[1].cells[j].x, results[2].cells[j].x);
			// ASSERT_DOUBLE_EQ(results[1].cells[j].y, results[2].cells[j].y);

			ASSERT_DOUBLE_EQ(results[0].cells[j].z, results[1].cells[j].z);
		}
	}

	FAIL();
}
