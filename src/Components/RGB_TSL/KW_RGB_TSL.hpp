/*!
 * \file KW_RGB_TSL.hpp
 * \brief
 * \author kwasak
 * \date 2010-12-17
 */

#ifndef KW_RGB_TSL_HPP_
#define KW_RGB_TSL_HPP_

#include "Component_Aux.hpp"
#include "Component.hpp"
#include "Panel_Empty.hpp"
#include "DataStream.hpp"
#include "Props.hpp"

#include <iostream>
#include <fstream>
#include <cv.h>
#include <highgui.h>

namespace Processors {
namespace KW_TSL {

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
 * \class KW_Initial_Filter
 * \brief Example processor class.
 */


struct min_max {
        float maxT;
        float minS;
        float maxS;
};

class KW_RGB_TSL: public Base::Component
{
public:
	/*!
	 * Constructor.
	 */
	KW_RGB_TSL(const std::string & name = "");

	/*!
	 * Destructor
	 */
	virtual ~KW_RGB_TSL();

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
	Base::EventHandler <KW_RGB_TSL> h_onNewImage;

	/// Input image
	Base::DataStreamIn <Mat> in_img;

	/// Event raised, when image is processed
	Base::Event * newImage;

	/// Output data stream - hue part with continous red
	Base::DataStreamOut <Mat> out_img;
	
	Base::DataStreamOut <min_max> out_img2;


	/// Properties
	Props props;

private:
	cv::Mat RGB_img;
	cv::Mat TSL_img;


	min_max MinMax;
	int k;
};

}//: namespace KW_TSL
}//: namespace Processors


/*
 * Register processor component.
 */
REGISTER_PROCESSOR_COMPONENT("KW_RGB_TSL", Processors::KW_TSL::KW_RGB_TSL, Common::Panel_Empty)

#endif /* KW_RBG_TSL_HPP_ */

