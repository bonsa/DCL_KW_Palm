/*!
 * \file MAP_PalmDetection.cpp
 * \brief Identyfikacja największego bloba przekraczajacego minimalny rozmiar jako dłon
 * \author kwasak
 * \date 2011-05-12
 */

#include <memory>
#include <string>
#include <math.h> 

#include "MAP_PalmDetection.hpp"
#include "Logger.hpp"
#include "Types/Ellipse.hpp"
#include "Types/Line.hpp"
#include "Types/Rectangle.hpp"
#include <vector>
#include <iomanip>

namespace Processors {
namespace MAP_PalmDetection {

using namespace cv;


MAP_PalmDetection::MAP_PalmDetection(const std::string & name) :
	Base::Component(name) {
	LOG(LTRACE) << "Hello MAP_PalmDetectionL\n";
}

MAP_PalmDetection::~MAP_PalmDetection() {
	LOG(LTRACE) << "Good bye MAP_PalmDetection\n";
}

bool MAP_PalmDetection::onInit() {
	LOG(LTRACE) << "MAP_PalmDetection::initialize\n";

	h_onNewImage.setup(this, &MAP_PalmDetection::onNewImage);
	registerHandler("onNewImage", &h_onNewImage);

	h_onNewBlobs.setup(this, &MAP_PalmDetection::onNewBlobs);
	registerHandler("onNewBlobs", &h_onNewBlobs);

	registerStream("in_blobs", &in_blobs);
	registerStream("in_img", &in_img);

	//!!!!!!!!!!!!!!!!!
	newImage = registerEvent("newImage");

	registerStream("out_blobs1", &out_blobs1);
	registerStream("out_blobs2", &out_blobs2);

	return true;
}

bool MAP_PalmDetection::onFinish() {
	LOG(LTRACE) << "MAP_PalmDetection::finish\n";

	return true;
}



bool MAP_PalmDetection::onStep()
{
	LOG(LTRACE) << "MAP_PalmDetection::step\n";

	blobs_ready = img_ready = false;

	try {

		getObservation();

		return true;
	} catch (...) {
		LOG(LERROR) << "MAP_PalmDetection failed\n";
		return false;
	}


}


bool MAP_PalmDetection::onStop() {
	return true;
}

bool MAP_PalmDetection::onStart() {
	return true;
}

void MAP_PalmDetection::onNewImage() {
	LOG(LTRACE) << "KW_MAP::onNewImage\n";

	img_ready = true;
	tsl_img = in_img.read();
	//co robi tak linijka?
	tsl_img = tsl_img.clone();
	if (blobs_ready && img_ready)
		onStep();
}

void MAP_PalmDetection::onNewBlobs() {
	LOG(LTRACE) << "KW_MAP::onNewBlobs\n";

	blobs_ready = true;
	blobs = in_blobs.read();
	//LOG(LNOTICE) << "Got blobs: " << blobs.GetNumBlobs();
	if (blobs_ready && img_ready)
		onStep();
}


void MAP_PalmDetection::getObservation(){

	LOG(LNOTICE) << "MAP_PalmDetection::getObservationn\n";

	try {

		// id największego bloba, czyli dłoni
		int id = 0;
		// aktualnie pobrany blob
		Types::Blobs::Blob *currentBlob;
		// wynikowy blob
		Types::Blobs::BlobResult result;
		// punkty znajdujace sie na konturze
		CvSeq * contour;
		// czyta elementy na konturu
		CvSeqReader reader;
		// punkt, na którym aktualnie jest wykonywana operacja
		cv::Point actualPoint;
		// obrocony actualPoint o kat nachylania dłoni wzgledem układu wspolrzędnych w punkcie środa masy
		cv::Point tempPoint;
		// wektor zawierający punkty konturu
		vector<cv::Point> contourPoints;
		//zmienna pomocnicza
		float TempDist;
		//zapamietuje poprzedni znak różnicy miedzy punktami,
		//1- funkcja jest rosnoca, -1 - funkcja malejąca
		int lastSign;
		int lastMinDist;
		//idenksy punktów charakterystycznych;
		//powyzej tej odległości od środa cieżkosci moga znajdować sie ekstrema
		int MINDIST;
		//id ostatenio wyznaczonego ekstremum
		int idLastExtreme;

		Types::DrawableContainer signs;

		//momenty goemetryczne potrzebne do obliczenia środka ciężkości
		double m00, m10, m01;
		//powierzchnia bloba, powiedzchnia największego bloba, współrzędne środka ciężkości, maksymalna wartośc współrzędnej Y
		double Area, CenterOfGravity_x, CenterOfGravity_y;

		double height, width;
		MaxArea = 0;

		id = -1;

		if (blobs.GetNumBlobs() < 1) {
		//	LOG(LNOTICE) << "Blobs: " << blobs.GetNumBlobs();
			return;
		}

		//największy blob to dłoń
		for (int i = 0; i < blobs.GetNumBlobs(); i++) {
			currentBlob = blobs.GetBlob(i);

			Area = currentBlob->Area();
			//szukanie bloba o największej powierzchni
			if (Area > MaxArea)
			{
				MaxArea = Area;
				// id największego bloba, czyli dłoni
				id = i;
			}

			if(MaxArea > 41000)
			{
				PALM = true;
			}
			else if (MaxArea <= 41000)
			{
				PALM = false;
			}
		}
		//current Blob przychowuje największego bloba, czyli dłoni
		currentBlob = blobs.GetBlob(id);

		if(PALM == true)
		{
			result.AddBlob(blobs.GetBlob(id));
			out_blobs1.write(result);
		}
		else
		{
			result.AddBlob(blobs.GetBlob(id));
			out_blobs2.write(result);
		}

		newImage->raise();
	} catch (...) {
		//LOG(LERROR) << "KW_MAP::getObservation failed\n";

	}
}



}//: namespace KW_MAP2
}//: namespace Processors
