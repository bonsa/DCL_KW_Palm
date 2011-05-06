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

KW_RGB_TSL::KW_RGB_TSL(const std::string & name) : Base::Component(name)
{
	LOG(LTRACE) << "Hello KW_RGB_TSL\n";
	k = 0;
}

KW_RGB_TSL::~KW_RGB_TSL()
{
	LOG(LTRACE) << "Good bye KW_RGB_TSL\n";
}

bool KW_RGB_TSL::onInit()
{
	LOG(LTRACE) << "KW_RGB_TSL::initialize\n";

	h_onNewImage.setup(this, &KW_RGB_TSL::onNewImage);
	registerHandler("onNewImage", &h_onNewImage);

	registerStream("in_img", &in_img);

	newImage = registerEvent("newImage");

	registerStream("out_img", &out_img);
	registerStream("out_img2", &out_img2);



	return true;
}

bool KW_RGB_TSL::onFinish()
{
	LOG(LTRACE) << "KW_RGB_TSL::finish\n";

	return true;
}

bool KW_RGB_TSL::onStep()
{
	LOG(LTRACE) << "KW_RGB_TSL::step\n";
	return true;
}

bool KW_RGB_TSL::onStop()
{
	return true;
}

bool KW_RGB_TSL::onStart()
{
	return true;
}

void KW_RGB_TSL::onNewImage()
{
	LOG(LTRACE) << "KW_RGB_TSL::onNewImage\n";
	try {
		cv::Mat RGB_img = in_img.read();	//czytam obrazem w zejścia
		std::ofstream plik("/home/kasia/Test.txt");
		cv::Size size = RGB_img.size();		//rozmiar obrazka
		
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
			const uchar* RGB_p = RGB_img.ptr <uchar> (i);
			// get pointer to beggining of i-th row of output hue image
			uchar* TSL_img_p = TSL_img.ptr <uchar> (i);
			// get pointer to beggining of i-th row of output hue image



			float R, G, B, r_, g_, T,S,L;

			//MaxT
			MinMax.maxT = 0;
			//MinS
			MinMax.minS = 100000;
			//MaxS
			MinMax.maxT = 0;


			int j;
			for (j = 0; j < size.width; j += 3)
			{

				B = RGB_p[j];
				G = RGB_p[j + 1];
				R = RGB_p[j + 2];

				if (B == 0 && G == 0 && R == 0)
				{
					TSL_img_p[j] = 0;
					TSL_img_p[j + 1] = 0;
					TSL_img_p[j + 2] = 0;

				}
				else
				{
					r_ = 1.0*R/(R+G+B+1)-1./3;
					g_ = 1.0*G/(R+G+B+1)-1./3;

					if (g_!=0)
					{
						T = atan(r_/g_)/3.141592 + 0.5;
					}
					else
					{
						T = 0;
					}
					S = sqrt(9.0/5.0*(r_*r_+g_*g_));
					L = 0.299*R + 0.587*G + 0.114*B;


					TSL_img_p[j] = T*255;
					TSL_img_p[j + 1] = S*255;
					TSL_img_p[j + 2] = L;


					if (MinMax.maxT < T)
					{
						MinMax.maxT = T;
					}
					if (MinMax.minS > S)
					{
						MinMax.minS = S;
					}
					if (MinMax.maxS < S)
					{
						MinMax.maxS = S;
					}
					plik<<L;

				}

			}


		}

		out_img.write(TSL_img);
		out_img2.write(MinMax);
		plik.close();

		newImage->raise();
		

	}
	catch (Common::DisCODeException& ex) {
		LOG(LERROR) << ex.what() << "\n";
		ex.printStackTrace();
		exit(EXIT_FAILURE);
	}
	catch (const char * ex) {
		LOG(LERROR) << ex;
	}
	catch (...) {
		LOG(LERROR) << "KW_RGB_TSL::onNewImage failed\n";
	}
}

}//: namespace KW_TSL
}//: namespace Processors
