/*!
 * \file KW_RGB_TSL.cpp
 * \brief Konwersja barw z przestrzeni RGB do TSL
 * \author kwasak
 * \date 2010-12-17
 */

#include <memory>
#include <string>
#include <math.h>

#include "KW_RGB_TSL.hpp"
#include "Logger.hpp"

namespace Processors {
namespace KW_TSL {

// OpenCV writes hue in range 0..180 instead of 0..360
#define H(x) (x>>1)

KW_RGB_TSL::KW_RGB_TSL(const std::string & name) :
	Base::Component(name) {
	LOG(LTRACE) << "Hello KW_RGB_TSL\n";
	k = 0;

	total = 0.0f;
	loops = 0;

	int r, g;
	float r_, g_;

	int ss = 128;

	for (r = 0; r < ss; ++r) {
		r_ = 1.0 * r / ss - 1.0 / 3;
		for (g = 0; g < ss; ++g) {
			g_ = 1.0 * g / ss - 1.0 / 3;
			atan_arr[ss * r + g] = atan(r_ / g_) / 3.141592 + 0.5;
		}
	}

	for (int i = 0; i < 4000; ++i) {
		sqrt_arr[i] = sqrt(1.0 * i / 2000);
	}
}

KW_RGB_TSL::~KW_RGB_TSL() {
	LOG(LTRACE) << "Good bye KW_RGB_TSL\n";
}

bool KW_RGB_TSL::onInit() {
	LOG(LTRACE) << "KW_RGB_TSL::initialize\n";

	h_onNewImage.setup(this, &KW_RGB_TSL::onNewImage);
	registerHandler("onNewImage", &h_onNewImage);

	registerStream("in_img", &in_img);

	newImage = registerEvent("newImage");

	registerStream("out_img", &out_img);
	registerStream("out_img2", &out_img2);

	return true;
}

bool KW_RGB_TSL::onFinish() {
	LOG(LTRACE) << "KW_RGB_TSL::finish\n";

	return true;
}

bool KW_RGB_TSL::onStep() {
	LOG(LTRACE) << "KW_RGB_TSL::step\n";
	return true;
}

bool KW_RGB_TSL::onStop() {
	return true;
}

bool KW_RGB_TSL::onStart() {
	return true;
}

inline float fastSqrt_2(const float x) {
	union {
		int i;
		float x;
	} u;
	u.x = x;
	u.i = (1 << 29) + (u.i >> 1) - (1 << 22);
	return u.x;
}

void KW_RGB_TSL::onNewImage() {
	LOG(LTRACE) << "KW_RGB_TSL::onNewImage\n";
	try {
		timer.restart();

		cv::Mat RGB_img = in_img.read(); //czytam obrazem w zejścia
		cv::Size size = RGB_img.size(); //rozmiar obrazka

		TSL_img.create(size, CV_8UC3);

		// Check the arrays for continuity and, if this is the case,
		// treat the arrays as 1D vectors
		if (RGB_img.isContinuous()) {
			size.width *= size.height;
			size.height = 1;
		}
		size.width *= 3;

		for (int i = 0; i < size.height; i++) {
			// when the arrays are continuous,
			// the outer loop is executed only once
			// if not - it's executed for each row

			// get pointer to beggining of i-th row of input hsv image
			const uchar* RGB_p = RGB_img.ptr<uchar> (i);
			// get pointer to beggining of i-th row of output hue image
			uchar* TSL_img_p = TSL_img.ptr<uchar> (i);
			// get pointer to beggining of i-th row of output hue image


			//MaxT
			MinMax.maxT = 0;
			//MinS
			MinMax.minS = 100000;
			//MaxS
			MinMax.maxS = 0;

			int j;
#pragma omp parallel for
			for (j = 0; j < size.width; j += 6) {
				float R, G, B, r_, g_, T, S, L;

				B = RGB_p[j];
				G = RGB_p[j + 1];
				R = RGB_p[j + 2];

				if (B == 0 && G == 0 && R == 0) {
					TSL_img_p[j] = 0;
					TSL_img_p[j + 1] = 0;
					TSL_img_p[j + 2] = 0;

				} else {
					r_ = R / (R + G + B + 1) - 0.333333333f;
					g_ = G / (R + G + B + 1) - 0.333333333f;

					if (g_ != 0) {
						// atan lib
						//T = atan(r_/g_)/3.141592 + 0.5;

						// atan cv 1/180 = 0,005555
						T = cv::fastAtan2(r_ / g_, 1.0f);
						T = T > 180 ? 0.005555f * (T - 270) : 0.005555f * (T
								+ 90);

						// atan 128
						//T = atan_arr[ 128 * (int)(r_*127) + (int)(g_*127) ];
					} else {
						T = 0;
					}

					// sqrt lib
					float arg = 1.8f * (r_ * r_ + g_ * g_);
					S = sqrt(arg);

					// sqrt arr
					//float arg = r_*r_ + g_*g_;
					//S = sqrt_arr[(int)(arg * 2000)];


					L = 0.299f * R + 0.587f * G + 0.114f * B;

					TSL_img_p[j] = T * 255;
					TSL_img_p[j + 1] = S * 255;
					TSL_img_p[j + 2] = L;

					TSL_img_p[j + 3] = T * 255;
					TSL_img_p[j + 4] = S * 255;
					TSL_img_p[j + 5] = L;

					if (MinMax.maxT < T) {
						MinMax.maxT = T;
					}
					if (MinMax.minS > S) {
						MinMax.minS = S;
					}
					if (MinMax.maxS < S) {
						MinMax.maxS = S;
					}

				}

			}

		}

		out_img.write(TSL_img);
		out_img2.write(MinMax);

		newImage->raise();

		total += timer.elapsed();
		loops++;
		if (loops > 100) {
			LOG(LNOTICE) << "TSL: " << total / loops;
			loops = 0;
			total = 0;
		}

	} catch (Common::DisCODeException& ex) {
		LOG(LERROR) << ex.what() << "\n";
		ex.printStackTrace();
		exit( EXIT_FAILURE);
	} catch (const char * ex) {
		LOG(LERROR) << ex;
	} catch (...) {
		LOG(LERROR) << "KW_RGB_TSL::onNewImage failed\n";
	}
}

}//: namespace KW_TSL
}//: namespace Processors
