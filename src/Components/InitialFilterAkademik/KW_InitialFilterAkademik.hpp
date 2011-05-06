/*!
 * \file KW_InitialFilterAkademik.hpp
 * \brief
 * Wstępna selekcja kolorów w akademiku
 * \author kwasak
 * \date 2010-03-05
 */

#ifndef KW_INILIAL_FILTER_AKADEMIK_HPP_
#define KW_INITIAL_FILTER2_AKADEMIK_HPP_

#include "Component_Aux.hpp"
#include "Component.hpp"
#include "Panel_Empty.hpp"
#include "DataStream.hpp"
#include "Props.hpp"
#include "Property.hpp"


#include <cv.h>
#include <highgui.h>

namespace Processors {
namespace KW_FilterAkademik {

using namespace cv;

/*!
 * \brief KW_mean_skin properties
 */
struct Props: public Base::Props
{
	/*!
	 * \copydoc Base::Props::load
	 */
	void load(const ptree & pt)
	{
	}

	/*!
	 * \copydoc Base::Props::save
	 */
	void save(ptree & pt)
	{
	}
};

/*!
 * \class KW_InitialFilterAkademik
 * \brief Example processor class.
 */
class  KW_InitialFilterAkademik: public Base::Component
{
public:
	/*!
	 * Constructor.
	 */
	 KW_InitialFilterAkademik(const std::string & name = "");

	/*!
	 * Destructor
	 */
	virtual ~ KW_InitialFilterAkademik();

	/*!
	 * Return window properties
	 */
	Base::Props * getProperties()
	{
		return &props;
	}

protected:

	/*!
	 * Connects source to given device.
	 */
	bool onInit();

	/*!
	 * Disconnect source from device, closes streams, etc.
	 */
	bool onFinish();

	/*!
	 * Retrieves data from device.
	 */
	bool onStep();

	/*!
	 * Start component
	 */
	bool onStart();

	/*!
	 * Stop component
	 */
	bool onStop();


	/*!
	 * Event handler function.
	 */
	void onNewImage();

	/// Event handler.
	Base::EventHandler < KW_InitialFilterAkademik> h_onNewImage;

	/// Input image
	Base::DataStreamIn <Mat> in_img;

	/// Event raised, when image is processed
	Base::Event * newImage;

	/// Output data stream - hue part with continous red
	Base::DataStreamOut <Mat> out_img;


	/// Properties
	Props props;

private:
	Base::Property<int> blue_R;
	Base::Property<int> blue_G;
	Base::Property<int> blue_B;

	Base::Property<int> green_R;
	Base::Property<int> green_G;
	Base::Property<int> green_B;

	Base::Property<int> green;
	Base::Property<int> red_green;

	Base::Property<int> dark_R;
	Base::Property<int> dark_G;
	Base::Property<int> dark_B;

	Base::Property<int> yellow_G;
	Base::Property<int> yellow_B;

	Base::Property<int> blue_in_contast;
	Base::Property<int> green_in_contast;

	cv::Mat RGB_img;
	cv::Mat Filtered_img;

	int k;
};

}//: namespace KW_FilterAkademik
}//: namespace Processors


/*
 * Register processor component.
 */
REGISTER_PROCESSOR_COMPONENT("KW_InitialFilterAkademik", Processors::KW_FilterAkademik::KW_InitialFilterAkademik, Common::Panel_Empty)

#endif /* KW_INITIAL_FILTER_AKADEMIK_HPP_ */

