#include "swe.hpp"
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <gtest/gtest.h>
#include <iomanip>

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
constexpr double dryTolerance = 1e-5;

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

auto floatW = std::setw(14);

namespace glm {
auto operator<<(std::ostream& os, glm::dvec4 v) -> std::ostream&
{
	return os << "(" << floatW << v.x << ",\t" << floatW << v.y << ",\t"
			  << floatW << v.z << ",\t" << floatW << v.w << ")";
}
} // namespace glm
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

auto swe::operator<<(std::ostream& os, const swe::PrintVec4& p) -> std::ostream&
{
	return os << glm::dvec4{p.v[0], p.v[1], p.v[2], p.v[3]};
}

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
		{{{0.005, 0, 0, 0}, {0.001, 0, 0, 0}}},
		{{{8899.326826472694, 122.0337839252433, 0, 0},
		  {8899.326826472694, -122.0337839252433, 0, 0}}},
		{{{1, 0, 0, 0}, {1, 0, 0, 0}}},
		{{{1, 0, 0, 0}, {0.5, 0, 0, 0}}},
		{{{1, 1, 1, 0}, {1, 1, 1, 0}}},
		{{{0, 1, 1, 1}, {0, 1, 1, 1}}},
		{{{1, 1, 1, 1}, {1, 1, 1, 1}}},
	};
	//  8899.739847378269
};

TEST_F(RiemannSolversF, Simple)
{
	for(auto i: simpleCases) {
		std::cout << std::setprecision(6) << std::fixed
				  << "------------------\nInput:\t" << i << "\n";

		for(auto s: solvers) {
			auto r = callSolver(i, 0, s.f);

			std::cout << s.name << ":\t" << r << "\n\n";
		}
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
		}
	}

	FAIL();
}
