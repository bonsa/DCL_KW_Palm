/*!
 * \file KW_InitialFilter_Movie.hpp
 * \brief
 * \author kwasak
 * \date 2010-03-29
 */

#ifndef KW_INILIAL_FILTER_MOVIE_HPP_
#define KW_INITIAL_FILTER_MOVIE_HPP_

#include "Component_Aux.hpp"
#include "Component.hpp"
#include "Panel_Empty.hpp"
#include "DataStream.hpp"
#include "Props.hpp"
#include "Property.hpp"


#include <cv.h>
#include <highgui.h>

namespace Processors {
namespace KW_Filter_Movie {

using namespace cv;

/*!
 * \brief KW_InitialFilter_Movie properties
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
 * \class KW_InitialFilter_Movie
 * \brief Example processor class.
 */
class KW_InitialFilter_Movie: public Base::Component
{
public:
	/*!
	 * Constructor.
	 */
	KW_InitialFilter_Movie(const std::string & name = "");

	/*!
	 * Destructor
	 */
	virtual ~KW_InitialFilter_Movie();

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
	Base::EventHandler <KW_InitialFilter_Movie> h_onNewImage;

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

}//: namespace KW_Filter_Movie
}//: namespace Processors


/*
 * Register processor component.
 */
REGISTER_PROCESSOR_COMPONENT("KW_InitialFilter_Movie", Processors::KW_Filter_Movie::KW_InitialFilter_Movie, Common::Panel_Empty)

#endif /* KW_INITIAL_FILTER_MOVIE_HPP_ */

