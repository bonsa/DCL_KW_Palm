/*!
 * \file KW_MAP3_SkinDetection.cpp
 * \brief Detekcja skory w przestrzni barw TSL
 * \author kwasak
 * \date 2010-04-29
 */

#include <memory>
#include <string>

#include "KW_MAP3_SkinDetection.hpp"
#include "Logger.hpp"

namespace Processors {
namespace KW_MAP3_Skin {

// OpenCV writes hue in range 0..180 instead of 0..360
#define H(x) (x>>1)

KW_MAP3_SkinDetection::KW_MAP3_SkinDetection(const std::string & name) : Base::Component(name),

	MIN_T("grupa1|  MIN_T", 0, "range"),
	MAX_T("grupa1|  MAX_T", 33, "range"),

	MIN_S("grupa2|  MIN_S", 28, "range"),
	MAX_S("grupa2|  MAX_S", 80, "range"),

	MIN_L("grupa3|  MIN_L", 49, "range"),
	MAX_L("grupa3|  MAX_L", 255, "range")

{
	LOG(LTRACE) << "Hello KW_MAP3_SkinDetection\n";

	MIN_T.addConstraint("0");
	MIN_T.addConstraint("255");
	MAX_T.addConstraint("0");
	MAX_T.addConstraint("255");

	MIN_S.addConstraint("0");
	MIN_S.addConstraint("255");
	MAX_S.addConstraint("0");
	MAX_S.addConstraint("255");

	MIN_L.addConstraint("0");
	MIN_L.addConstraint("255");
	MAX_L.addConstraint("0");
	MAX_L.addConstraint("255");

	registerProperty(MIN_T);
	registerProperty(MAX_T);

	registerProperty(MIN_S);
	registerProperty(MAX_S);

	registerProperty(MIN_L);
	registerProperty(MAX_L);

}

KW_MAP3_SkinDetection::~KW_MAP3_SkinDetection()
{
	LOG(LTRACE) << "Good bye KW_MAP3_SkinDetection\n";
}

bool KW_MAP3_SkinDetection::onInit()
{
	LOG(LTRACE) << "KW_MAP3_SkinDetection::initialize\n";

	h_onNewImage.setup(this, &KW_MAP3_SkinDetection::onNewImage);
	registerHandler("onNewImage", &h_onNewImage);

	registerStream("in_img", &in_img);

	newImage = registerEvent("newImage");

	registerStream("out_img", &out_img);


	return true;
}

bool KW_MAP3_SkinDetection::onFinish()
{
	LOG(LTRACE) << "KW_MAP3_SkinDetection::finish\n";

	return true;
}

bool KW_MAP3_SkinDetection::onStep()
{
	LOG(LTRACE) << "KW_MAP3_SkinDetection::step\n";
	return true;
}

bool KW_MAP3_SkinDetection::onStop()
{
	return true;
}

bool KW_MAP3_SkinDetection::onStart()
{
	return true;
}

void KW_MAP3_SkinDetection::onNewImage()
{
	LOG(LTRACE) << "KW_MAP3_SkinDetection::onNewImage\n";
	try {
		cv::Mat TSL_img = in_img.read();	//czytam obrazu w wejścia

		cv::Size size = TSL_img.size();		//rozmiar obrazka

		skin_img.create(size, CV_8UC3);		//8bitów, 0-255, 1 kanał




		// Check the arrays for continuity and, if this is the case,
		// treat the arrays as 1D vectors
		if (TSL_img.isContinuous() && skin_img.isContinuous())  {
			size.width *= size.height;
			size.height = 1;
		}
		size.width *= 3;

		for (int i = 0; i < size.height; i++) {

			// when the arrays are continuous,
			// the outer loop is executed only once
			// if not - it's executed for each row

			// get pointer to beggining of i-th row of input image
			const uchar* c_p = TSL_img.ptr <uchar> (i);
			// get pointer to beggining of i-th row of output hue image
			uchar* skin_p = skin_img.ptr <uchar> (i);


			int j,k = 0;
			for (j = 0; j < size.width; j += 3) {

				if ((c_p[j] >= MIN_T) && (c_p[j] <=MAX_T) &&
					(c_p[j+1] >= MIN_S) && (c_p[j+1] <= MAX_S)&&
					(c_p[j+2] >= MIN_L) && (c_p[j+2] <= MAX_L))
				{
							skin_p[j] = 255;
							skin_p[j+1] = 255;
							skin_p[j+2] = 255;
				}
				else
				{
					skin_p[j] = 0;//c_p[j];
					skin_p[j+1] = 0;//c_p[j+1];
					skin_p[j+2] = 0;//c_p[j+2];
				}

				++k;
			}
		}

		out_img.write(skin_img);


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
		LOG(LERROR) << "KW_MAP3_SkinDetection::onNewImage failed\n";
	}
}

}//: namespace KW_MAP3_Skin
}//: namespace Processors
