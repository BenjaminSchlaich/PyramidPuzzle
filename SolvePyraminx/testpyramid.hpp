
#pragma once

#include "pyramid.hpp"

#include <string>
#include <list>

void runAllTests();

/// given n+1 pyramids and n operations, check that they form a valid sequence for our implementation,
/// i.e. operation(i) on pyramid(i) = pyramid(i+1)
/// return value: status of the test. -1 if failed, 0 if skipped, 1 if succeeded.
int runTestCase(const std::list<pyramid> &pyramids, const std::list<Operation> &operations);
