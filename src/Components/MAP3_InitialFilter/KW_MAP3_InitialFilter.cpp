/*!
 * \file KW_MAP3_InitialFilter.cpp
 * \brief Filtr, który odrzuci piksele, któ¶e na pewno nie sa skora
 * \author kwasak
 * \date 2011-03-29
 */

#include <memory>
#include <string>

#include "KW_MAP3_InitialFilter.hpp"
#include "Logger.hpp"

namespace Processors {
namespace KW_MAP3_Filter {

// OpenCV writes hue in range 0..180 instead of 0..360
#define H(x) (x>>1)

KW_MAP3_InitialFilter::KW_MAP3_InitialFilter(const std::string & name) : Base::Component(name),
/* dla filmiku działa, tylko wszystko jest zakomentowane oprócz 2 pierwszych linijek
		blue_B("grupa1|  down_blue_B", 255, "range"),
		blue_R("grupa1|  up_blue_R", 150, "range"),
		blue_G("grupa1|  up_blue_G", 0, "range"),

		green_G("grupa2|  down_green_G", 0, "range"),
		green_R("grupa2|  up_green_R", 117, "range"),
		green_B("grupa2|  up_green_B", 255, "range"),

		dark_R("grupa3|  up_dark_R", 158, "range"),
		dark_G("grupa3|  up_dark_G", 163, "range"),
		dark_B("grupa3|  up_dark_B", 156, "range"),

		green("grupa4|  down_green", 156, "range"),
		red_green("grupa4|  down_red_green", 145, "range"),

		yellow_G("grupa5|  down_yellow_G", 140, "range"),
		yellow_B("grupa5|  up_yellow_B", 100, "range"),

		blue_in_contast("grupa6|  down_blue_in_contast", 40, "range"),
		green_in_contast("grupa6|  down_green_in_contast", 45, "range")

*/

		blue_B("grupa1|  down_blue_B", 255, "range"),
		blue_R("grupa1|  up_blue_R", 10, "range"),
		blue_G("grupa1|  up_blue_G", 230, "range"),

		green_G("grupa2|  down_green_G", 0, "range"),
		green_R("grupa2|  up_green_R", 0, "range"),
		green_B("grupa2|  up_green_B", 0, "range"),

		dark_R("grupa3|  up_dark_R", 99, "range"),
		dark_G("grupa3|  up_dark_G", 82, "range"),
		dark_B("grupa3|  up_dark_B", 61, "range"),

		green("grupa4|  down_green", 196, "range"),
		red_green("grupa4|  down_red_green", 54, "range"),

		yellow_G("grupa5|  down_yellow_G", 140, "range"),
		yellow_B("grupa5|  up_yellow_B", 100, "range"),

		blue_in_contast("grupa6|  down_blue_in_contast", 40, "range"),
		green_in_contast("grupa6|  down_green_in_contast", 54, "range")

{
	LOG(LTRACE) << "Hello KW_MAP3_InitialFilter\n";
	k = 0;

	blue_R.addConstraint("0");
	blue_R.addConstraint("255");

	blue_G.addConstraint("0");
	blue_G.addConstraint("255");

	blue_B.addConstraint("0");
	blue_B.addConstraint("255");

	registerProperty(blue_R);
	registerProperty(blue_G);
	registerProperty(blue_B);

	green_R.addConstraint("0");
	green_R.addConstraint("255");

	green_G.addConstraint("0");
	green_G.addConstraint("255");

	green_B.addConstraint("0");
	green_B.addConstraint("255");

	registerProperty(green_R);
	registerProperty(green_G);
	registerProperty(green_B);

	green.addConstraint("0");
	green.addConstraint("255");

	red_green.addConstraint("0");
	red_green.addConstraint("255");

	registerProperty(green);
	registerProperty(red_green);


	dark_R.addConstraint("0");
	dark_R.addConstraint("255");

	dark_G.addConstraint("0");
	dark_G.addConstraint("255");

	dark_B.addConstraint("0");
	dark_B.addConstraint("255");

	registerProperty(dark_R);
	registerProperty(dark_G);
	registerProperty(dark_B);

	yellow_G.addConstraint("0");
	yellow_G.addConstraint("255");

	yellow_B.addConstraint("0");
	yellow_B.addConstraint("255");

	registerProperty(yellow_G);
	registerProperty(yellow_B);

	blue_in_contast.addConstraint("0");
	blue_in_contast.addConstraint("100");

	green_in_contast.addConstraint("0");
	green_in_contast.addConstraint("100");

	registerProperty(blue_in_contast);
	registerProperty(green_in_contast);
}

KW_MAP3_InitialFilter::~KW_MAP3_InitialFilter()
{
	LOG(LTRACE) << "Good bye KW_MAP3_InitialFilter\n";
}

bool KW_MAP3_InitialFilter::onInit()
{
	LOG(LTRACE) << "KW_MAP3_InitialFilter::initialize\n";

	h_onNewImage.setup(this, &KW_MAP3_InitialFilter::onNewImage);
	registerHandler("onNewImage", &h_onNewImage);

	registerStream("in_img", &in_img);

	newImage = registerEvent("newImage");

	registerStream("out_img", &out_img);



	return true;
}

bool KW_MAP3_InitialFilter::onFinish()
{
	LOG(LTRACE) << "KW_MAP3_InitialFilter::finish\n";

	return true;
}

bool KW_MAP3_InitialFilter::onStep()
{
	LOG(LTRACE) << "KW_MAP3_InitialFilter";
	return true;
}

bool KW_MAP3_InitialFilter::onStop()
{
	return true;
}

bool KW_MAP3_InitialFilter::onStart()
{
	return true;
}

void KW_MAP3_InitialFilter::onNewImage()
{
	LOG(LTRACE) << "KW_MAP3_InitialFilter::onNewImage\n";
	try {
		cv::Mat RGB_img = in_img.read();	//czytam obrazem w wejścia

		cv::Size size = RGB_img.size();		//rozmiar obrazka
		
		Filtered_img.create(size, CV_8UC3);

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
			uchar* Filtered_img_p = Filtered_img.ptr <uchar> (i);
			// get pointer to beggining of i-th row of output hue image

			uchar R, G, B;


			int j;
			for (j = 0; j < size.width; j += 3)
			{
				B = RGB_p[j];
				G = RGB_p[j + 1];
				R = RGB_p[j + 2];

				//wszystko było zakomentowane oprócz dwóch pierwszych linijek

				if((B > blue_B && R < blue_R && G < blue_G)  // too much blue
					||(G > green_G && R < green_R && B < green_B) // too much green
					||(B < dark_B && R < dark_R && G < dark_G)  // too dark
					||(G > green) //Green
					//||(R+G > red_green)  // too much red and green (yellow like color)
					//||(G > yellow_G && B < yellow_B) // too Yellow like also
					||(1.0*B/(R+G+B) > 0.01 * blue_in_contast) // too much blue in contrast to others
					||(1.0*G/(R+G+B) > 0.01 * green_in_contast)// // too much green in contrast to others
				//	(R < 102 && G > 100 && B > 110 && G < 140 && B <160)
					)
					{
						Filtered_img_p[j] = 0;
						Filtered_img_p[j + 1] = 0;
						Filtered_img_p[j + 2] = 0;
					}
				else
				{
					Filtered_img_p[j] = RGB_p[j];
					Filtered_img_p[j + 1] = RGB_p[j + 1];
					Filtered_img_p[j + 2] = RGB_p[j + 2] ;
				}
				++k;
			}
		}

		out_img.write(Filtered_img);


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
		LOG(LERROR) << "KW_MAP3_InitialFilter::onNewImage failed\n";
	}
}

}//: namespace KW_MAP3_InitialFilter
}//: namespace Processors
