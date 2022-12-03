#pragma once

#include "ConfigDllExportMotionHubUtil.h"

#include "MMHmath.h"

#include <vector>
#include <numeric>

class MotionHubUtil_DLL_import_export Evaluation {

public:

	Evaluation() {
		values = new std::vector<float>();
	}

	~Evaluation() {
		delete values;
	}

	void clearValues() {
		values->clear();
	}

	void addValue(float value) {
		values->push_back(value);
	}

	float getAverage() {
		return std::reduce(values->begin(), values->end()) / static_cast<float>(values->size());
	}

	float getMedian() {
		size_t n = values->size() / 2;
		std::nth_element(values->begin(), values->begin() + n, values->end());
		return values->at(n);
	}

	float getMin() {
		return *std::min_element(values->begin(), values->end());
	}

	float getMax() {
		return *std::max_element(values->begin(), values->end());
	}

private:
	std::vector<float>* values;

};