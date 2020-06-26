#include "swe.hpp"
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <gtest/gtest.h>

using RiemannSolverFunction = auto (*)(
								  double* fL,
								  double* fR,
								  const double* qL,
								  const double* qR,
								  int direction,
								  double grav,
								  double epsilon) -> double;

TEST(RiemannSolvers, Linking)
{
	glm::dvec4 l = {};
	glm::dvec4 r = {};
	glm::dvec4 dl = {};
	glm::dvec4 dr = {};
	swe::samoaRiemannSolver(
		glm::value_ptr(dl),
		glm::value_ptr(dr),
		glm::value_ptr(l),
		glm::value_ptr(r),
		0,
		9.81,
		0.1);
}
