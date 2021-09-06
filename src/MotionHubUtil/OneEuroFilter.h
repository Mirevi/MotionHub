/* -*- coding: utf-8 -*-
 *
 * OneEuroFilter.cc -
 *
 * Author: Nicolas Roussel (nicolas.roussel@inria.fr)
 *
 * Copyright 2019 Inria
 *
 * BSD License https://opensource.org/licenses/BSD-3-Clause
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice, this list of conditions
 * and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions
 * and the following disclaimer in the documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or
 * promote products derived from this software without specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "ConfigDllExportMotionHubUtil.h"
#include "MMHmath.h"

#include <iostream>
#include <stdexcept>
#include <cmath>
#include <ctime>

 // -----------------------------------------------------------------
 // Utilities

typedef double TimeStamp; // in seconds

static const TimeStamp UndefinedTime = -1.0;

// -----------------------------------------------------------------

class LowPassFilter {

private:

	float y, a, s;
	bool initialized;

	void setAlpha(float alpha) {
		if (alpha <= 0.0f || alpha > 1.0f)
			throw std::range_error("alpha should be in (0.0., 1.0]");
		a = alpha;
	}

public:

	LowPassFilter(float alpha, float initval = 0.0f) {
		y = s = initval;
		setAlpha(alpha);
		initialized = false;
	}

	float filter(float value) {
		float result;
		if (initialized)
			result = a * value + (1.0f - a) * s;
		else {
			result = value;
			initialized = true;
		}
		y = value;
		s = result;
		return result;
	}

	float filterWithAlpha(float value, float alpha) {
		setAlpha(alpha);
		return filter(value);
	}

	bool hasLastRawValue(void) {
		return initialized;
	}

	float lastRawValue(void) {
		return y;
	}

};

// -----------------------------------------------------------------

class OneEuroFilter {

private:

	float freq;
	float mincutoff;
	float beta_;
	float dcutoff;
	LowPassFilter* x;
	LowPassFilter* dx;
	TimeStamp lasttime;

	float currentValue;

	float alpha(float cutoff) {
		float te = 1.0f / freq;
		float tau = 1.0f / (2.0f * M_PI * cutoff);
		return 1.0f / (1.0f + tau / te);
	}

	void setFrequency(float f) {
		if (f <= 0.0f) throw std::range_error("freq should be >0");
		freq = f;
	}

	void setMinCutoff(float mc) {
		if (mc <= 0.0f) throw std::range_error("mincutoff should be >0");
		mincutoff = mc;
	}

	void setBeta(float b) {
		beta_ = b;
	}

	void setDerivateCutoff(float dc) {
		if (dc <= 0.0f) throw std::range_error("dcutoff should be >0");
		dcutoff = dc;
	}

public:

	OneEuroFilter() {
		x = nullptr;
		dx = nullptr;
	}

	OneEuroFilter(float freq, float mincutoff = 1.0f, float beta_ = 0.0f, float dcutoff = 1.0f) {
		setFrequency(freq);
		setMinCutoff(mincutoff);
		setBeta(beta_);
		setDerivateCutoff(dcutoff);
		x = new LowPassFilter(alpha(mincutoff));
		dx = new LowPassFilter(alpha(dcutoff));
		lasttime = UndefinedTime;
	}

	float filter(float value, TimeStamp timestamp = UndefinedTime) {
		// update the sampling frequency based on timestamps
		if (lasttime != UndefinedTime && timestamp != UndefinedTime)
			freq = 1.0f / (timestamp - lasttime);
		lasttime = timestamp;
		// estimate the current variation per second 
		float dvalue = x->hasLastRawValue() ? (value - x->lastRawValue()) * freq : 0.0f; // FIXME: 0.0 or value?
		float edvalue = dx->filterWithAlpha(dvalue, alpha(dcutoff));
		// use it to update the cutoff frequency
		float cutoff = mincutoff + beta_ * fabs(edvalue);
		// filter the given value
		currentValue = x->filterWithAlpha(value, alpha(cutoff));
		return currentValue;
	}

	float value() {
		return currentValue;
	}

	~OneEuroFilter() {
		//if (x != nullptr) delete x;
		//if (dx != nullptr) delete dx;
	}

};

// -----------------------------------------------------------------

/*
int
main(int argc, char** argv) {
	randSeed();

	double duration = 10.0; // seconds

	double frequency = 120; // Hz
	double mincutoff = 1.0; // FIXME
	double beta = 1.0;      // FIXME
	double dcutoff = 1.0;   // this one should be ok

	std::cout << "#SRC OneEuroFilter.cc" << std::endl
		<< "#CFG {'beta': " << beta << ", 'freq': " << frequency << ", 'dcutoff': " << dcutoff << ", 'mincutoff': " << mincutoff << "}" << std::endl
		<< "#LOG timestamp, signal, noisy, filtered" << std::endl;

	OneEuroFilter f(frequency, mincutoff, beta, dcutoff);
	for (TimeStamp timestamp = 0.0; timestamp < duration; timestamp += 1.0 / frequency) {
		double signal = sin(timestamp);
		double noisy = signal + (unifRand() - 0.5) / 5.0;
		double filtered = f.filter(noisy, timestamp);
		std::cout << timestamp << ", "
			<< signal << ", "
			<< noisy << ", "
			<< filtered
			<< std::endl;
	}

	return 0;
}
*/

// -----------------------------------------------------------------

class BaseOneEuroFilter {

protected:
	std::vector<OneEuroFilter> oneEuroFilters;

public:
	BaseOneEuroFilter() {}

	~BaseOneEuroFilter() {}
};

class Vector3OneEuroFilter : public BaseOneEuroFilter {

public:
	Vector3OneEuroFilter() {}

	~Vector3OneEuroFilter() {}

	Vector3OneEuroFilter(float freq, float mincutoff = 1.0f, float beta = 0.0f, float dcutoff = 1.0f) {

		for (int i = 0; i < 3; i++) {
			oneEuroFilters.push_back(OneEuroFilter(freq, mincutoff, beta, dcutoff));
		}
	}

	Vector3f filter(Vector3f value, TimeStamp timestamp = UndefinedTime) {

		Vector3f output;

		for (int i = 0; i < 3; i++) {
			output[i] = oneEuroFilters[i].filter(value[i], timestamp);
		}

		return output;
	}
};

class QuaternionOneEuroFilter : public BaseOneEuroFilter {

public:
	QuaternionOneEuroFilter() {}

	~QuaternionOneEuroFilter() {}

	QuaternionOneEuroFilter(float freq, float mincutoff = 1.0f, float beta = 0.0f, float dcutoff = 1.0f) {

		for (int i = 0; i < 4; i++) {
			oneEuroFilters.push_back(OneEuroFilter(freq, mincutoff, beta, dcutoff));
		}
	}

	Quaternionf filter(Quaternionf value, TimeStamp timestamp = UndefinedTime) {

		Quaternionf output;

		/*
		if ((Vector4f(oneEuroFilters[0].value(), oneEuroFilters[1].value(), oneEuroFilters[2].value(), oneEuroFilters[3].value()).normalized()
			- Vector4f(value.x(), value.y(), value.z(), value.w()).normalized()).squaredNorm() > 2.0f)
		{
			output.x() = oneEuroFilters[0].filter(-value.x(), timestamp);
			output.y() = oneEuroFilters[1].filter(-value.y(), timestamp);
			output.z() = oneEuroFilters[2].filter(-value.z(), timestamp);
			output.w() = oneEuroFilters[2].filter(-value.w(), timestamp);
		}
		else {
		*/
			output.x() = oneEuroFilters[0].filter(value.x(), timestamp);
			output.y() = oneEuroFilters[1].filter(value.y(), timestamp);
			output.z() = oneEuroFilters[2].filter(value.z(), timestamp);
			output.w() = oneEuroFilters[3].filter(value.w(), timestamp);
		//}

		return output;
	}
};