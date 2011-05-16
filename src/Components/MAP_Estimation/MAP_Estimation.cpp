/*!
 * \file MAP_Estimation.cpp
 * \brief Estymacja MAP 
 * \author kwasak
 * \date 2011-05-12
 */

#include <memory>
#include <string>
#include <math.h> 

#include "MAP_Estimation.hpp"
#include "Logger.hpp"
#include "Types/Ellipse.hpp"
#include "Types/Line.hpp"
#include "Types/Rectangle.hpp"
#include <vector>
#include <iomanip>

namespace Processors {
namespace MAP_Estimation {

using namespace cv;

MAP_Estimation::~MAP_Estimation() {
	LOG(LTRACE) << "Good bye KW_MAP2\n";
}

bool MAP_Estimation::onInit() {
	LOG(LTRACE) << "MAP_Estimation::initialize\n";

	h_onNewImage.setup(this, &MAP_Estimation::onNewImage);
	registerHandler("onNewImage", &h_onNewImage);

	h_onNewBlobs.setup(this, &MAP_Estimation::onNewBlobs);
	registerHandler("onNewBlobs", &h_onNewBlobs);

	h_map.setup(this, &MAP_Estimation::map);
	registerHandler("map", &h_map);

	h_onTrigger.setup(this, &MAP_Estimation::onTrigger);
	registerHandler("onTrigger", &h_onTrigger);

	registerStream("in_blobs", &in_blobs);
	registerStream("in_img", &in_img);

	newImage = registerEvent("newImage");

	registerStream("out_draw", &out_draw);

	return true;
}

bool MAP_Estimation::onFinish() {
	LOG(LTRACE) << "MAP_Estimation::finish\n";

	return true;
}

void MAP_Estimation::onTrigger() {

		if(MAP == true)
			MAP = false;
		else
		{
			MAP = true;

			error = 100;
			error2 = 100;

			s.clear();
			RFinger.s.clear();
			MFinger.s.clear();
			FFinger.s.clear();
			TFinger.s.clear();
			SFinger.s.clear();

			statesInit();
			matrixInit();

		}
}

void MAP_Estimation::map()
{
//	if(MAP == true)
	//	MAP = false;
//	else
//	//	MAP = true;
}

bool MAP_Estimation::onStep()
{
	LOG(LTRACE) << "KW_MAP2::step\n";

	blobs_ready = img_ready = false;


	try {

		drawcont.clear();

		//czubki palców
		fingertips.clear();
		idFingertips.clear();

		//srodkowy palec
		MFinger.z.clear();
		MFinger.h_z.clear();
		MFinger.diff.clear();
		MFinger.sTest.clear();
	//	s_MFinger.clear();

		getObservation();
		projectionFingertips();

		if((MAP == true) && (idFingertips.size() == 5))
		{

			//palec wskazujacy
			FFinger.z.clear();
		//	s_FFinger.clear();
			FFinger.h_z.clear();
			FFinger.diff.clear();
			FFinger.sTest.clear();

			//kciuk
			TFinger.z.clear();
		//	s_FFinger.clear();
			TFinger.h_z.clear();
			TFinger.diff.clear();
			TFinger.sTest.clear();

			//mały palec
			SFinger.z.clear();
		//	s_SFinger.clear();
			SFinger.h_z.clear();
			SFinger.diff.clear();
			SFinger.sTest.clear();

			//mały palec
			RFinger.z.clear();
		//	s_RFinger.clear();
			RFinger.h_z.clear();
			RFinger.diff.clear();
			RFinger.sTest.clear();

		//	cout<<"STOP "<<STOP<<"\n";
			if(error > 1.0)
			{
				cout<<"Error "<<error<<"\n";
				error = 0.0;
				error2 = 0.0;

				z.clear();
				h_z.clear();
				diff.clear();
				sTest.clear();


			//	projectionObservation(z, 255, 255, 255);
				observationToState();
				projectionState(sTest, 0, 255, 255);
				stateToObservation();
			//	projectionObservation(h_z, 255, 0, 255);
				calculateDiff();
				updateState();


				//palec środkowy
				sTest.clear();
				MFinger.z = getFingerObservation(2);
				projectionFingerObservation(MFinger.z, 200, 200, 200);
				MFinger.sTest = observationFingerToState(MFinger.z, 0.7, 0.6);
				projectionFingerState(MFinger.sTest, 0, 255, 255);
				MFinger.h_z = stateFingerToObservation(MFinger.s, 7.0/6.0);
			//	projectionFingerObservation(h_z_MFinger, 255, 255, 0);

				calculateFingerH(MFinger.s, MFinger.H, 7.0/6.0);

				MFinger.diff = calculateFingerDiff(MFinger.h_z, MFinger.z, MFinger.invR, MFinger.H, MFinger.P, factorFinger);
				MFinger.s = updateFingerState(MFinger.diff,MFinger.s, MFinger.P);



				//palec wskazujacy
				FFinger.z = getFingerObservation(3);
				projectionFingerObservation(FFinger.z, 200, 200, 200);
				FFinger.sTest = observationFingerToState(FFinger.z, 0.72, 0.56);
				projectionFingerState(FFinger.sTest, 0, 255, 255);
				FFinger.h_z = stateFingerToObservation(FFinger.s, 9.0/7.0);
			//	projectionFingerObservation(h_z_FFinger, 255, 255, 0);

				calculateFingerH(FFinger.s, FFinger.H, 9.0/7.0);
				FFinger.diff = calculateFingerDiff(FFinger.h_z, FFinger.z, FFinger.invR, FFinger.H, FFinger.P, 0.03);//factorFinger);
				FFinger.s = updateFingerState(FFinger.diff, FFinger.s, FFinger.P);


				//kciuk
				TFinger.z = getFingerObservation(4);
				projectionFingerObservation(TFinger.z, 200, 200, 200);
				TFinger.sTest = observationFingerToState(TFinger.z, 0.72, 0.56);
				projectionFingerState(TFinger.sTest, 0, 255, 255);
				TFinger.h_z = stateFingerToObservation(TFinger.s, 9.0/7.0);
			//	projectionFingerObservation(h_z_TFinger, 255, 255, 0);
				calculateFingerH(TFinger.s, TFinger.H, 9.0/7.0);
				TFinger.diff = calculateFingerDiff(TFinger.h_z, TFinger.z, TFinger.invR, TFinger.H, TFinger.P, factorFinger);
				TFinger.s = updateFingerState(TFinger.diff ,TFinger.s, TFinger.P);

				//mały palec
				SFinger.z = getFingerObservation(0);
				projectionFingerObservation(SFinger.z, 200, 200, 200);
				SFinger.sTest = observationFingerToState(SFinger.z, 0.82, 0.36);
				projectionFingerState(SFinger.sTest, 0, 255, 255);
				SFinger.h_z = stateFingerToObservation(SFinger.s, 41.0/18.0);
			//	projectionFingerObservation(h_z_SFinger, 255, 255, 0);
				calculateFingerH(SFinger.s, SFinger.H, 41.0/18.0);
				SFinger.diff = calculateFingerDiff(SFinger.h_z, SFinger.z, SFinger.invR, SFinger.H, SFinger.P, factorFinger);
				SFinger.s = updateFingerState(SFinger.diff,SFinger.s, SFinger.P);

				//palec serdeczny
				//ring-finger
				RFinger.z = getFingerObservation(1);
				projectionFingerObservation(RFinger.z, 200, 200, 200);
				RFinger.sTest = observationFingerToState(RFinger.z, 0.73, 0.54);
				projectionFingerState(RFinger.sTest, 0, 255, 255);
				RFinger.h_z = stateFingerToObservation(RFinger.s,73.0/54.0);
			//	projectionFingerObservation(h_z_RFinger, 255, 255, 0);
				calculateFingerH(RFinger.s, RFinger.H, 73.0/54.0);
				RFinger.diff = calculateFingerDiff(RFinger.h_z, RFinger.z, RFinger.invR, RFinger.H, RFinger.P, factorFinger);
				RFinger.s = updateFingerState(RFinger.diff, RFinger.s, RFinger.P);

				projectionState(s, 255, 255, 255);
				projectionFingerState(MFinger.s, 255, 255, 255);
				projectionFingerState(FFinger.s, 255, 255, 255);
				projectionFingerState(TFinger.s, 255, 255, 255);
				projectionFingerState(SFinger.s, 255, 255, 255);
				projectionFingerState(RFinger.s, 255, 255, 255);

			}
			else
			{
				projectionState(s, 255, 255, 255);
				projectionFingerState(MFinger.s, 255, 255, 255);
				projectionFingerState(FFinger.s, 255, 255, 255);
				projectionFingerState(TFinger.s, 255, 255, 255);
				projectionFingerState(SFinger.s, 255, 255, 255);
				projectionFingerState(RFinger.s, 255, 255, 255);
			}

		}

		out_draw.write(drawcont);
		newImage->raise();
		return true;
	} catch (...) {
		//LOG(LERROR) << "KW_MAP::MAP_Estimation failed\n";
		return false;
	}


}


bool MAP_Estimation::onStop() {
	return true;
}

bool MAP_Estimation::onStart() {
	return true;
}

void MAP_Estimation::onNewImage() {
	LOG(LTRACE) << "MAP_Estimation::onNewImage\n";

	img_ready = true;
	tsl_img = in_img.read();
	//co robi tak linijka?
	tsl_img = tsl_img.clone();
	if (blobs_ready && img_ready)
		onStep();
}

void MAP_Estimation::onNewBlobs() {
	LOG(LTRACE) << "KW_MAP::onNewBlobs\n";

	blobs_ready = true;
	blobs = in_blobs.read();
	//LOG(LNOTICE) << "Got blobs: " << blobs.GetNumBlobs();
	if (blobs_ready && img_ready)
		onStep();
}



void MAP_Estimation::getObservation(){

	//LOG(LNOTICE) << "KW_MAP::getObservation\n";

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
		double CenterOfGravity_x, CenterOfGravity_y;

		double height, width;

		currentBlob = blobs.GetBlob(0);

		// calculate moments
		m00 = currentBlob->Moment(0, 0);
		m01 = currentBlob->Moment(0, 1);
		m10 = currentBlob->Moment(1, 0);

		MinX = currentBlob->MinX();
		MaxX = currentBlob->MaxX();
		MinY = currentBlob->MinY();
		MaxY = currentBlob->MaxY();

		//obliczenie środka cięzkości
		CenterOfGravity_x = m10 / m00;
		CenterOfGravity_y = m01 / m00;

		z.push_back(CenterOfGravity_x);
		z.push_back(CenterOfGravity_y);

		Types::Ellipse * elE;
		elE = new Types::Ellipse(Point2f(CenterOfGravity_x, CenterOfGravity_y), Size2f(10, 10));
		elE->setCol(CV_RGB(0,255,0));
		drawcont.add(elE);

		//kontur największego bloba
		contour = currentBlob->GetExternalContour()->GetContourPoints();
		cvStartReadSeq(contour, &reader);
		CV_READ_SEQ_ELEM( actualPoint, reader);
		topPoint = actualPoint;
		idFingertips.push_back(0);

		for (int j = 0; j < contour->total; j = j + 1) {

			CV_READ_SEQ_ELEM( actualPoint, reader);

			if (j % 20 == 1)
			{
				//wpisanie punktów z konturu do wektora
				contourPoints.push_back(cv::Point(actualPoint.x, actualPoint.y));
			}
		}

		MINDIST = (MaxY - CenterOfGravity_y) * (MaxY - CenterOfGravity_y) * 4/ 9;

		//środek cieżkości przesuwał troche w dół ekranu, aby ułatwić wyznaczanie punktów charakterystycznych
		CenterOfGravity_y += (MaxY - CenterOfGravity_y) * 2 / 3;


		//******************************************************************
		//obliczenie roznicy miedzy punktami konturu a przesuniętym środkiem ciężkosci
		vector<double> dist;
		//1 -oznacza, że ostatni element z konturu należał do dolnej czesci dłoni
		lastMinDist = 0;
		idLastExtreme = 0;
		lastSign = 0; //ta linijka nic nie znaczy, nie chce miec warninga
		for (unsigned int i = 0; i < contourPoints.size(); i++)
		{
			TempDist = (contourPoints[i].x - CenterOfGravity_x)	* (contourPoints[i].x - CenterOfGravity_x) + (contourPoints[i].y - CenterOfGravity_y) * (contourPoints[i].y - CenterOfGravity_y);

			if (TempDist > MINDIST)
				dist.push_back(TempDist);
			else
				//jeśli odległość jest mniejsza niż MINDIST oznacza to, że jest to dolna cześć dłoni i nie znajdują się tam żadnego punkty charakterystyczne poza przesuniętym środkiem ciężkości, dlatego te punkty można ominąć
				dist.push_back(MINDIST);

			if (i == 1)
			{
				if ((dist[1] - dist[0]) > 0)
					lastSign = 1;
				else
					lastSign = -1;
			}
			else if (i > 1)
			{
				if (dist[i] > MINDIST && dist[i - 1] > MINDIST)
				{

					//jeżeli ostatnio była wykryta dolna cześci dłoni, następnym charakterystycznych punktem powinien być czubek palca, dlatego lastSign = 1;
					if (lastMinDist == 1)
					{
						lastSign = 1;
						lastMinDist = 0;
					}
					//maksiumum - czubek palca, funkcja rosła i zaczeła maleć
					if ((dist[i] - dist[i-1]) < 0 && lastSign == 1)
					{
						if (((contourPoints[i-1].x - contourPoints[idLastExtreme].x) * (contourPoints[i-1].x - contourPoints[idLastExtreme].x) + (contourPoints[i-1].y - contourPoints[idLastExtreme].y) * (contourPoints[i-1].y - contourPoints[idLastExtreme].y)) > 3000)
						{
							if((dist[i-1]>20000) && (idFingertips.size()<5))
							{
								newFingertip = true;

								for(unsigned int j = 0; j < idFingertips.size(); j++)
								{
									if(((contourPoints[i-1].x - contourPoints[idFingertips[j]].x) * (contourPoints[i-1].x - contourPoints[idFingertips[j]].x) + (contourPoints[i-1].y - contourPoints[idFingertips[j]].y) * (contourPoints[i-1].y - contourPoints[idFingertips[j]].y)) < 500)
									{
									//	newFingertip = false;
									}
								}
									if(newFingertip == true)
									{
										idFingertips.push_back(i-1);
										lastSign = -1;
										idLastExtreme = i-1;
									}
							}
						}
					}
					//minimum - punkt między palcami
					else if ((dist[i] - dist[i-1]) > 0 && lastSign == -1)
					{
						if (((contourPoints[i-1].x - contourPoints[idLastExtreme].x) * (contourPoints[i-1].x - contourPoints[idLastExtreme].x) + (contourPoints[i-1].y - contourPoints[idLastExtreme].y) * (contourPoints[i-1].y - contourPoints[idLastExtreme].y)) > 5000)
						{
							lastSign = 1;
							idLastExtreme = i-1;
						}
					}
				}
				else
				{
					// element należący do dołu dłoni
					lastMinDist = 1;
				}
			}

		}

		int idLeftPoint = 0;
		int xLeftPoint = 1000000;
		for (unsigned int i = 0; i < idFingertips.size(); i++) {
			//znajdujemy punkt najbardziej wysynięty na lewo, czyli wierzchołek małego palca
			if (xLeftPoint > contourPoints[idFingertips[i]].x)
			{
				xLeftPoint = contourPoints[idFingertips[i]].x;
				idLeftPoint = i;
			}
		}

		for (int i = idLeftPoint; i >= 0; i--) {
			//wpisanie do tablicy punktów charakterystycznych punktów opisujących trzy lewe palce
			fingertips.push_back(cv::Point( contourPoints[idFingertips[i]].x, contourPoints[idFingertips[i]].y));
		}

		for (int i = idFingertips.size() - 1; i > idLeftPoint; i--) {
			//wpisanie do tablicy punktów charakterystycznych punktów opisujących dwa prawe palce
			fingertips.push_back(cv::Point( contourPoints[idFingertips[i]].x,  contourPoints[idFingertips[i]].y));
		}

		double dx = - z[0] + topPoint.x;
		double dy = - z[1] + topPoint.y;

		//argument kąta nachylenia
		double angle = abs(atan2(dy, dx));

		//kat w radianach
		z.push_back(angle);

		height = MaxY - MinY;
		width = MaxX - MinX;

		z.push_back(height);
		z.push_back(width);

	//	LOG(LNOTICE) << "Fingertips: " << idFingertips.size();

	} catch (...) {
		LOG(LERROR) << "MAP_Estimation::getObservation failed\n";

	}
}

//projekcja na obraz punktów charakterystycznych z aktualnego obrazka
void MAP_Estimation::projectionFingertips() {
	Types::Ellipse * el;
	for (unsigned int i = 0; i < fingertips.size(); i++) {
		el = new Types::Ellipse(cv::Point(fingertips[i].x, fingertips[i].y),Size2f(10, 10));
		el->setCol(CV_RGB(0,0,255));
		drawcont.add(el);
	}

}
//punkcja obracająca punkt p o kąt angle według układu współrzędnych znajdującym się w punkcie p0
cv::Point MAP_Estimation::rot(cv::Point p, double angle, cv::Point p0) {
	cv::Point t;
	t.x = p0.x + (int) ((double) (p.x - p0.x) * cos(angle) - (double) (p.y - p0.y) * sin(angle));
	t.y = p0.y + (int) ((double) (p.x - p0.x) * sin(angle) + (double) (p.y - p0.y) * cos(angle));
	return t;
}

void MAP_Estimation::projectionObservation(vector<double> z, int R, int G, int B)
{
	cv::Point obsPointA;
	cv::Point obsPointB;
	cv::Point obsPointC;
	cv::Point obsPointD;

	double rotAngle = 0;

	if(z[2]> M_PI_2)
	{
		rotAngle = (z[2] - M_PI_2);
	}
	else if (z[2]< M_PI_2)
	{
		rotAngle = - (M_PI_2 - z[2]);
	}

	obsPointA.x = z[0] - 0.5 * z[4];
	obsPointA.y = z[1] - 4/7.0 *z[3];

	obsPointB.x = z[0] + 0.5 * z[4];
	obsPointB.y = z[1] - 4/7.0 *z[3];

	obsPointC.x = z[0] + 0.5 * z[4];
	obsPointC.y = z[1] + 3/7.0 *z[3];

	obsPointD.x = z[0] - 0.5 * z[4];
	obsPointD.y = z[1] + 3/7.0 *z[3];

	Types::Line * elL;


	obsPointA = rot(obsPointA, - rotAngle, cv::Point(z[0], z[1]));
	obsPointB = rot(obsPointB, - rotAngle, cv::Point(z[0], z[1]));
	obsPointC = rot(obsPointC, - rotAngle, cv::Point(z[0], z[1]));
	obsPointD = rot(obsPointD, - rotAngle, cv::Point(z[0], z[1]));


	elL = new Types::Line(cv::Point(obsPointA.x, obsPointA.y), cv::Point(obsPointB.x, obsPointB.y));
	elL->setCol(CV_RGB(R,G,B));
	drawcont.add(elL);

	elL = new Types::Line(cv::Point(obsPointB.x, obsPointB.y), cv::Point(obsPointC.x, obsPointC.y));
	elL->setCol(CV_RGB(R,G,B));
	drawcont.add(elL);

	elL = new Types::Line(cv::Point(obsPointC.x, obsPointC.y), cv::Point(obsPointD.x, obsPointD.y));
	elL->setCol(CV_RGB(R,G,B));

	drawcont.add(elL);
	elL = new Types::Line(cv::Point(obsPointD.x, obsPointD.y), cv::Point(obsPointA.x, obsPointA.y));
	elL->setCol(CV_RGB(R,G,B));
	drawcont.add(elL);


}


// Funkcja wyliczajaca wartosci parametru stanu na podstawie wartosci obserwacji
void MAP_Estimation::observationToState()
{
	float s_mx, s_my, s_angle, s_heigth, s_width;

	s_mx = z[0] - 0.05 * z[4];
	s_my = z[1] + 1.0/7.0 * z[3];

	Types::Ellipse * el;

	el = new Types::Ellipse(cv::Point(s_mx, s_my), Size2f(10, 10));
	el->setCol(CV_RGB(255,0,0));
	drawcont.add(el);

	s_angle = z[2];
	s_heigth = 0.4 * z[3];
	s_width = 0.5 * z[4];

	sTest.push_back(s_mx);
	sTest.push_back(s_my);
	sTest.push_back(s_angle);
	sTest.push_back(s_heigth);
	sTest.push_back(s_width);

}

void MAP_Estimation::projectionState(vector<double> s, int R, int G, int B)
{
	cv::Point obsPointA;
	cv::Point obsPointB;
	cv::Point obsPointC;
	cv::Point obsPointD;

	double rotAngle = 0;
	if(s[2]> M_PI_2)
	{
		rotAngle = (s[2] - M_PI_2);
	}
	else if (s[2]< M_PI_2)
	{
		rotAngle = - (M_PI_2 - s[2]);
	}

	obsPointA.x = s[0] - 0.5 * s[4];
	obsPointA.y = s[1] - 0.5 * s[3];

	obsPointB.x = s[0] + 0.5 * s[4];
	obsPointB.y = s[1] - 0.5 *s[3];

	obsPointC.x = s[0] + 0.5 * s[4];
	obsPointC.y = s[1] + 0.5 *s[3];

	obsPointD.x = s[0] - 0.5 * s[4];
	obsPointD.y = s[1] + 0.5 *s[3];

	Types::Line * elL;

	obsPointA = rot(obsPointA, - rotAngle, cv::Point(s[0], s[1]));
	obsPointB = rot(obsPointB, - rotAngle, cv::Point(s[0], s[1]));
	obsPointC = rot(obsPointC, - rotAngle, cv::Point(s[0], s[1]));
	obsPointD = rot(obsPointD, - rotAngle, cv::Point(s[0], s[1]));

	elL = new Types::Line(cv::Point(obsPointA.x, obsPointA.y), cv::Point(obsPointB.x, obsPointB.y));
	elL->setCol(CV_RGB(R,G,B));
	drawcont.add(elL);

	elL = new Types::Line(cv::Point(obsPointB.x, obsPointB.y), cv::Point(obsPointC.x, obsPointC.y));
	elL->setCol(CV_RGB(R,G,B));
	drawcont.add(elL);

	elL = new Types::Line(cv::Point(obsPointC.x, obsPointC.y), cv::Point(obsPointD.x, obsPointD.y));
	elL->setCol(CV_RGB(R,G,B));

	drawcont.add(elL);
	elL = new Types::Line(cv::Point(obsPointD.x, obsPointD.y), cv::Point(obsPointA.x, obsPointA.y));
	elL->setCol(CV_RGB(R,G,B));
	drawcont.add(elL);


}

void MAP_Estimation:: stateToObservation()
{
	float hz_mx, hz_my, hz_angle, hz_heigth, hz_width;

	hz_mx = s[0] +  0.1 * s[4];
	hz_my = s[1] - 5.0/14.0 * s[3];
	hz_angle = s[2];
	hz_heigth = 5.0/2.0 * s[3];
	hz_width = 2 * s[4];

	h_z.push_back(hz_mx);
	h_z.push_back(hz_my);
	h_z.push_back(hz_angle);
	h_z.push_back(hz_heigth);
	h_z.push_back(hz_width);

}



// Funkcja obliczająca o jaki wektor nalezy zaktualizowac wektor stan
void MAP_Estimation::calculateDiff()
{
	//różnicaiedzy wektorami h(s) i z
	double D[5];


	for (unsigned int i = 0; i < 5; i ++)
	{
		//różnica miedzy punktami charakterystycznymi aktualnego obraz
		D[i] =  h_z[i] - z[i];
		error2 += abs(D[i]);
	}

	double t1[5];
	for (unsigned int i = 0; i < 5; i++) {
		t1[i] = 0;
		for (unsigned int j = 0; j < 5; j++) {
			//t = iloraz odwrotnej macierzy R * roznica D
			t1[i] += invR[i][j] * D[j];
		}
	}

	double t2[5];
	for (unsigned int i = 0; i < 5; i++) {
		t2[i] = 0;
		for (unsigned int j = 0; j < 5; j++) {
			//t1 = iloraz macierzy H * t1
			t2[i] += H[i][j] * t1[j];
		}
	}

	double t3[5];
	for (unsigned int i = 0; i < 5; i++) {
		t3[i] = 0;
		for (unsigned int j = 0; j < 5; j++) {
			//mnożenie macierzy P * t2
			t3[i] +=   P[i][j] * t2[j];

		}
		t3[i] *= factorPalm;//*factor;
		diff.push_back(t3[i]);


	}
}

void MAP_Estimation::updateState()
{
	for (unsigned int i = 0; i < 5; i++) {
		s[i] = s[i] - diff[i];
		error += abs(diff[i]);
	}

	for (unsigned int i = 0; i < 5; i++) {
		for (unsigned int j = 0; j < 5; j++) {
			P[i][j] *= (1 - factor);
		}
	}
}


// Otrzymanie obserwacji środkowego palca
vector <double> MAP_Estimation::getFingerObservation(int i)
{

	double downX, downY, topX, topY, alfa, w;

	downX = z[0] - 3.0/7.0 * (topPoint.x - z[0]);
	downY = z[1] - 3.0/7.0 * (topPoint.y - z[1]);

	topX = fingertips[i].x;
	topY = fingertips[i].y;

	double dx, dy;
	dx = topX - downX;
	dy = topY - downY;

	double angle = abs(atan2(dy, dx));
	alfa = angle;  //kat w radianach
	w = z[4];

	vector <double> z_Finger;

	z_Finger.push_back(downX);
	z_Finger.push_back(downY);
	z_Finger.push_back(topX);
	z_Finger.push_back(topY);
	z_Finger.push_back(alfa);
	z_Finger.push_back(w);

	return z_Finger;
}

void MAP_Estimation::projectionFingerObservation(vector<double> z, int R, int G, int B)
{
	cv::Point obsPointDown = cv::Point(z[0], z[1]);
	cv::Point obsPointTop  = cv::Point(z[2], z[3]);

		Types::Ellipse * el;
		Types::Line * elL;

		el = new Types::Ellipse(cv::Point(obsPointDown.x, obsPointDown.y), Size2f(10, 10));
		el->setCol(CV_RGB(R,G,B));
		drawcont.add(el);

		el = new Types::Ellipse(cv::Point(obsPointTop.x, obsPointTop.y), Size2f(10, 10));
		el->setCol(CV_RGB(R,G,B));
		drawcont.add(el);

		elL = new Types::Line(cv::Point(obsPointDown.x, obsPointDown.y), cv::Point(obsPointTop.x, obsPointTop.y));
		elL->setCol(CV_RGB(R,G,B));
		drawcont.add(elL);

}



vector <double> MAP_Estimation:: observationFingerToState(vector <double> z_Finger, float a, float b)
{

	float s_mx, s_my, s_angle, s_heigth, s_width;

	s_mx = z_Finger[0] + a * (z_Finger[2] - z_Finger[0]);
	s_my = z_Finger[1] + a * (z_Finger[3] - z_Finger[1]);

	s_angle = z_Finger[4];
	s_heigth = b * (sqrt((z_Finger[0]-z_Finger[2])*(z_Finger[0]-z_Finger[2])+(z_Finger[1]-z_Finger[3])*(z_Finger[1]-z_Finger[3])));
	s_width = 0.12 * z_Finger[5];

	vector <double> sTest;
	sTest.push_back(s_mx);
	sTest.push_back(s_my);
	sTest.push_back(s_angle);
	sTest.push_back(s_heigth);
	sTest.push_back(s_width);

	return sTest;

}



void MAP_Estimation::projectionFingerState(vector<double> s, int R, int G, int B)
{
	cv::Point obsPointA;
	cv::Point obsPointB;
	cv::Point obsPointC;
	cv::Point obsPointD;

	double rotAngle = 0;

	//katy sa w radianach

	if((s[2])> M_PI_2)
	{
		rotAngle = ((s[2]) - M_PI_2);
	}
	else if ((s[2])< M_PI_2)
	{
		rotAngle = - (M_PI_2 - (s[2]));
	}

	obsPointA.x = s[0] - 0.5 * s[4];
	obsPointA.y = s[1] - 0.5 * s[3];

	obsPointB.x = s[0] + 0.5 * s[4];
	obsPointB.y = s[1] - 0.5 *s[3];

	obsPointC.x = s[0] + 0.5 * s[4];
	obsPointC.y = s[1] + 0.5 *s[3];

	obsPointD.x = s[0] - 0.5 * s[4];
	obsPointD.y = s[1] + 0.5 *s[3];


	Types::Line * elL;

	obsPointA = rot(obsPointA, - rotAngle, cv::Point(s[0], s[1]));
	obsPointB = rot(obsPointB, - rotAngle, cv::Point(s[0], s[1]));
	obsPointC = rot(obsPointC, - rotAngle, cv::Point(s[0], s[1]));
	obsPointD = rot(obsPointD, - rotAngle, cv::Point(s[0], s[1]));

	elL = new Types::Line(cv::Point(obsPointA.x, obsPointA.y), cv::Point(obsPointB.x, obsPointB.y));
	elL->setCol(CV_RGB(R,G,B));
	drawcont.add(elL);

	elL = new Types::Line(cv::Point(obsPointB.x, obsPointB.y), cv::Point(obsPointC.x, obsPointC.y));
	elL->setCol(CV_RGB(R,G,B));
	drawcont.add(elL);

	elL = new Types::Line(cv::Point(obsPointC.x, obsPointC.y), cv::Point(obsPointD.x, obsPointD.y));
	elL->setCol(CV_RGB(R,G,B));

	drawcont.add(elL);
	elL = new Types::Line(cv::Point(obsPointD.x, obsPointD.y), cv::Point(obsPointA.x, obsPointA.y));
	elL->setCol(CV_RGB(R,G,B));
	drawcont.add(elL);


}


vector <double> MAP_Estimation::calculateFingerDiff(vector <double> h_z, vector <double> z, double invR[6][6], double  H[5][6], double P[5][5], double factorFinger)
{
	//różnicaiedzy wektorami h(s) i z
	double D[6];

	vector <double> diff;

	for (unsigned int i = 0; i < 6; i ++)
	{
		//różnica miedzy punktami charakterystycznymi aktualnego obraz
		D[i] =  h_z[i] - z[i];
		error2 += abs(D[i]);
		//cout<<"DIFF     "<<D[i]<<"\n";
	}
	cout<<"\n";

	double t1[6];
	for (unsigned int i = 0; i < 6; i++) {
		t1[i] = 0;
		for (unsigned int j = 0; j < 6; j++) {
			//t = iloraz odwrotnej macierzy R * roznica D
			t1[i] += invR[i][j] * D[j];
		}
	}

	double t2[5];
	for (unsigned int i = 0; i < 5; i++) {
		t2[i] = 0;
		for (unsigned int j = 0; j < 6; j++) {
			t2[i] += H[i][j] * t1[j];
		}
	}

	double t3[5];
	for (unsigned int i = 0; i < 5; i++) {
		t3[i] = 0;
		for (unsigned int j = 0; j < 5; j++) {
			//mnożenie macierzy P * t2
			t3[i] +=  P[i][j] * t2[j];
			//cout<<P_Finger[i][j]<<"\t";
		}
		//cout<<"\n";
		t3[i] *= factorFinger;//*factor;
		diff.push_back(t3[i]);
	}

	//for (int i = 0; i < 5; ++i)
	//	diff_Finger[i] = fabs(diff_Finger[i]) > 8 ? 8 * diff_Finger[i]/fabs(diff_Finger[i]) : diff_Finger[i];

	return diff;
}


// Funckja aktualizująca wektor stanu i macierz kowariancji P
vector <double>MAP_Estimation::updateFingerState(vector <double> diff_Finger, vector <double> s_Finger, double P_Finger[5][5])
{
	vector <double> s_NFinger;

	for (unsigned int i = 0; i < 5; i++) {
		s_NFinger.push_back(s_Finger[i] - diff_Finger[i]);
		error += abs(diff_Finger[i]);
	}

	for (unsigned int i = 0; i < 5; i++) {
		for (unsigned int j = 0; j < 5; j++) {
			P_Finger[i][j] *= (1 - factor);
		}
	}
	return s_NFinger;
}




// Funkcja wyliczajaca wartosci parametrów obserwacji na podstawie wartosci obserwacji
vector <double> MAP_Estimation::stateFingerToObservation(vector <double> s_Finger, float a)
{
	float hz_downX, hz_downY, hz_topX, hz_topY, hz_angle, hz_width;

	hz_downX = s_Finger[0] - a * s_Finger[3]*cos(s_Finger[2]);
	hz_downY = s_Finger[1] + a * s_Finger[3]*sin(s_Finger[2]);

	hz_topX = s_Finger[0] + 1.0/2.0 * s_Finger[3]*cos(s_Finger[2]);
	hz_topY = s_Finger[1] - 1.0/2.0 * s_Finger[3]*sin(s_Finger[2]);

	hz_angle = s_Finger[2];
	hz_width = 25/3.0 * s_Finger[4];

	vector <double> h_z_Finger;

	h_z_Finger.push_back(hz_downX);
	h_z_Finger.push_back(hz_downY);
	h_z_Finger.push_back(hz_topX);
	h_z_Finger.push_back(hz_topY);
	h_z_Finger.push_back(hz_angle);
	h_z_Finger.push_back(hz_width);

	return h_z_Finger;
}


void MAP_Estimation::calculateFingerH(vector<double> s_Finger, double H_Finger[5][6], float a)
{

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 6; j++) {
			H_Finger[i][j] = 0;
		}
	}

	H_Finger[0][0] = 1;
	H_Finger[2][0] = a * s_Finger[3] * sin(s_Finger[2]);
	H_Finger[3][0] = - a * cos(s_Finger[2]);

	H_Finger[1][1] = 1;
	H_Finger[2][1] = a * s_Finger[3] * cos(s_Finger[2]);
	H_Finger[3][1] = a * sin(s_Finger[2]);

	H_Finger[0][2] = 1;
	H_Finger[2][2] = - 1.0/2.0 * s_Finger[3] * sin(s_Finger[2]);
	H_Finger[3][2] = 1.0/2.0 * cos(s_Finger[2]);

	H_Finger[1][3] = 1;
	H_Finger[2][3] = - 1.0/2.0 * s_Finger[3] * cos(s_Finger[2]);
	H_Finger[3][3] = - 1.0/2.0 * sin(s_Finger[2]);

	H_Finger[2][4] = 1.0;
	H_Finger[4][5] = 25/3.0;


}



/*************************************************************************/

//konstruktor
MAP_Estimation::MAP_Estimation(const std::string & name) :
	Base::Component(name)
	{
	LOG(LTRACE) << "Hello KW_MAP\n";

	error = 9999;
	MAP = true;
	STOP = false;

	
	//MAP3
	//wspołczynnik zapominania
	factor = 0.007;
	//współczynnik skalowania kroków
	factorFinger = 0.05;//0.03;

	factorPalm = 0.05;

	//dla MAP2_Sequence
	//factor2 = 0.04;


	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			H[i][j] = 0;
		}
	}

	H[0][0] = 1;
	H[4][0] = 0.05;
	H[1][1] = 1;
	H[3][1] = -5.0/14.0;
	H[2][2] = 1;
	H[3][3] = 5.0/3.0;
	H[4][4] = 2;


	statesInit();
	matrixInit();


}

void MAP_Estimation:: statesInit()
{
	//wew dlon

	s.push_back(352.18);
	s.push_back(331.46);
	s.push_back(1.6848);
	s.push_back(152.78);
	s.push_back(178.57);

	s0.push_back(352.18);
	s0.push_back(331.46);
	s0.push_back(1.6848);
	s0.push_back(152.78);
	s0.push_back(178.57);


	//srodkowy palec


	MFinger.s.push_back(347.62);
	MFinger.s.push_back(159.3);
	MFinger.s.push_back(1.6848);
	MFinger.s.push_back(177.87);
	MFinger.s.push_back(42.858);

	MFinger.s0.push_back(347.62);
	MFinger.s0.push_back(159.3);
	MFinger.s0.push_back(1.6848);
	MFinger.s0.push_back(177.87);
	MFinger.s0.push_back(42.858);



	//*******************************************************************************************//
	//palec wskazujacy

	FFinger.s.push_back(409.44);
	FFinger.s.push_back(162.31);
	FFinger.s.push_back(1.3846);
	FFinger.s.push_back(160.73);
	FFinger.s.push_back(42.858);

	FFinger.s0.push_back(409.44);
	FFinger.s0.push_back(162.31);
	FFinger.s0.push_back(1.3846);
	FFinger.s0.push_back(160.73);
	FFinger.s0.push_back(42.858);


	//***************************************************************************************//
	//kciuk

	TFinger.s.push_back(497.46);
	TFinger.s.push_back(299.07);
	TFinger.s.push_back(0.48356);
	TFinger.s.push_back(111.02);
	TFinger.s.push_back(42.858);

	TFinger.s0.push_back(497.46);
	TFinger.s0.push_back(299.07);
	TFinger.s0.push_back(0.48356);
	TFinger.s0.push_back(111.02);
	TFinger.s0.push_back(42.858);



	//***************************************************************************************//
	//mały palec

	SFinger.s.push_back(230.25);
	SFinger.s.push_back(229.59);
	SFinger.s.push_back(2.3748);
	SFinger.s.push_back(86.064);
	SFinger.s.push_back(42.858);

	SFinger.s0.push_back(230.25);
	SFinger.s0.push_back(229.59);
	SFinger.s0.push_back(2.3748);
	SFinger.s0.push_back(86.064);
	SFinger.s0.push_back(42.858);



	//*******************************************************************************//
	//palec serdeczny

	RFinger.s.push_back(299.59);
	RFinger.s.push_back(174.02);
	RFinger.s.push_back(1.929);
	RFinger.s.push_back(151.26);
	RFinger.s.push_back(42.858);

	RFinger.s0.push_back(299.59);
	RFinger.s0.push_back(174.02);
	RFinger.s0.push_back(1.929);
	RFinger.s0.push_back(151.26);
	RFinger.s0.push_back(42.858);
}

void MAP_Estimation:: matrixInit()
{

	//wew dlon
	P[0][0] = 7898.123517;
	P[0][1] = 37.669557;
	P[0][2] = 0.290219;
	P[0][3] = -354.473747;
	P[0][4] = -470.330553;

	P[1][0] = 37.669557;
	P[1][1] = 262.201038;
	P[1][2] = 0.471210;
	P[1][3] = 3.523747;
	P[1][4] = 71.038316;

	P[2][0] = 0.290219;
	P[2][1] = 0.471210;
	P[2][2] = 0.003764;
	P[2][3] = 0.014130;
	P[2][4] = 0.290246;

	P[3][0] = -354.473747;
	P[3][1] = 3.523747;
	P[3][2] = 0.014130;
	P[3][3] = 176.328000;
	P[3][4] = 274.791053;

	P[4][0] = -470.330553;
	P[4][1] = 71.038316;
	P[4][2] = 0.290246;
	P[4][3] = 274.791053;
	P[4][4] = 508.217763;


	invP[0][0] = 0.000141;
	invP[0][1] = 0.000020;
	invP[0][2] = -0.003878;
	invP[0][3] = 0.000514;
	invP[0][4] = -0.000148;

	invP[1][0] = 0.000020;
	invP[1][1] = 0.005368;
	invP[1][2] = -0.471001;
	invP[1][3] = 0.004396;
	invP[1][4] = -0.002840;

	invP[2][0] = -0.003878;
	invP[2][1] = -0.471001;
	invP[2][2] = 390.568673;
	invP[2][3] = 1.403795;
	invP[2][4] = -0.919834;

	invP[3][0] = 0.000514;
	invP[3][1] = 0.004396;
	invP[3][2] = 1.403795;
	invP[3][3] = 0.050642;
	invP[3][4] = -0.028323;

	invP[4][0] = -0.000148;
	invP[4][1] = -0.002840;
	invP[4][2] = -0.919834;
	invP[4][3] = -0.028323;
	invP[4][4] = 0.018067;


	R[0][0] = 7852.336300;
	R[0][1] = 162.862329;
	R[0][2] = 0.304703;
	R[0][3] = -851.867079;
	R[0][4] = -889.894553;

	R[1][0] = 162.862329;
	R[1][1] = 282.185173;
	R[1][2] = 0.466120;
	R[1][3] = -148.634526;
	R[1][4] = -54.218000;

	R[2][0] = 0.304703;
	R[2][1] = 0.466120;
	R[2][2] = 0.003764;
	R[2][3] = 0.035326;
	R[2][4] = 0.580491;

	R[3][0] = -851.867079;
	R[3][1] = -148.634526;
	R[3][2] = 0.035326;
	R[3][3] = 1102.050000;
	R[3][4] = 1373.955263;

	R[4][0] = -889.894553;
	R[4][1] = -54.218000;
	R[4][2] = 0.580491;
	R[4][3] = 1373.955263;
	R[4][4] = 2032.871053;


	invR[0][0] = 0.000141;
	invR[0][1] = 0.000020;
	invR[0][2] = -0.003881;
	invR[0][3] = 0.000208;
	invR[0][4] = -0.000077;

	invR[1][0] = 0.000020;
	invR[1][1] = 0.005367;
	invR[1][2] = -0.470892;
	invR[1][3] = 0.002525;
	invR[1][4] = -0.001420;

	invR[2][0] = -0.003881;
	invR[2][1] = -0.470892;
	invR[2][2] = 390.553502;
	invR[2][3] = 0.494273;
	invR[2][4] = -0.459845;

	invR[3][0] = 0.000208;
	invR[3][1] = 0.002525;
	invR[3][2] = 0.494273;
	invR[3][3] = 0.008715;
	invR[3][4] = -0.005873;

	invR[4][0] = -0.000077;
	invR[4][1] = -0.001420;
	invR[4][2] = -0.459845;
	invR[4][3] = -0.005873;
	invR[4][4] = 0.004521;

	//palec srodkowu

	 MFinger.P[0][0] = 7888.528068;
	 MFinger.P[0][1] = 406.435705;
	 MFinger.P[0][2] = -0.135018;
	 MFinger.P[0][3] = -435.621592;
	 MFinger.P[0][4] = -122.957084;

	 MFinger.P[1][0] = 406.435705;
	 MFinger.P[1][1] = 521.170785;
	 MFinger.P[1][2] = 0.491322;
	 MFinger.P[1][3] = -285.146180;
	 MFinger.P[1][4] = -70.328615;

	 MFinger.P[2][0] = -0.135018;
	 MFinger.P[2][1] = 0.491322;
	 MFinger.P[2][2] = 0.003764;
	 MFinger.P[2][3] = 0.034951;
	 MFinger.P[2][4] = 0.069659;

	 MFinger.P[3][0] = -435.621592;
	 MFinger.P[3][1] = -285.146180;
	 MFinger.P[3][2] = 0.034951;
	 MFinger.P[3][3] = 348.553026;
	 MFinger.P[3][4] = 97.536609;

	 MFinger.P[4][0] = -122.957084;
	 MFinger.P[4][1] = -70.328615;
	 MFinger.P[4][2] = 0.069659;
	 MFinger.P[4][3] = 97.536609;
	 MFinger.P[4][4] = 29.273343;


	MFinger.invP[0][0] = 0.000137;
	MFinger.invP[0][1] = -0.000040;
	MFinger.invP[0][2] = 0.008904;
	MFinger.invP[0][3] = 0.000139;
	MFinger.invP[0][4] = -0.000007;

	MFinger.invP[1][0] = -0.000040;
	MFinger.invP[1][1] = 0.004670;
	MFinger.invP[1][2] = -0.560854;
	MFinger.invP[1][3] = 0.005335;
	MFinger.invP[1][4] = -0.005386;

	MFinger.invP[2][0] = 0.008904;
	MFinger.invP[2][1] = -0.560854;
	MFinger.invP[2][2] = 580.463754;
	MFinger.invP[2][3] = 3.656078;
	MFinger.invP[2][4] = -14.873096;

	MFinger.invP[3][0] = 0.000139;
	MFinger.invP[3][1] = 0.005335;
	MFinger.invP[3][2] = 3.656078;
	MFinger.invP[3][3] = 0.084669;
	MFinger.invP[3][4] = -0.277407;

	MFinger.invP[4][0] = -0.000007;
	MFinger.invP[4][1] = -0.005386;
	MFinger.invP[4][2] = -14.873096;
	MFinger.invP[4][3] = -0.277407;
	MFinger.invP[4][4] = 0.980883;


	MFinger.R[0][0] = 7895.858947;
	MFinger.R[0][1] = 0.497900;
	MFinger.R[0][2] = 7772.402632;
	MFinger.R[0][3] = 668.163684;
	MFinger.R[0][4] = 0.634515;
	MFinger.R[0][5] = -788.814211;

	MFinger.R[1][0] = 0.497900;
	MFinger.R[1][1] = 303.854636;
	MFinger.R[1][2] = -154.575263;
	MFinger.R[1][3] = 124.265368;
	MFinger.R[1][4] = 0.447130;
	MFinger.R[1][5] = 344.682000;

	MFinger.R[2][0] = 7772.402632;
	MFinger.R[2][1] = -154.575263;
	MFinger.R[2][2] = 7986.368421;
	MFinger.R[2][3] = 609.263158;
	MFinger.R[2][4] = -0.464874;
	MFinger.R[2][5] = -1125.710526;

	MFinger.R[3][0] = 668.163684;
	MFinger.R[3][1] = 124.265368;
	MFinger.R[3][2] = 609.263158;
	MFinger.R[3][3] = 901.252632;
	MFinger.R[3][4] = 0.510406;
	MFinger.R[3][5] = -984.963158;

	MFinger.R[4][0] = 0.634515;
	MFinger.R[4][1] = 0.447130;
	MFinger.R[4][2] = -0.464874;
	MFinger.R[4][3] = 0.510406;
	MFinger.R[4][4] = 0.003764;
	MFinger.R[4][5] = 0.580491;

	MFinger.R[5][0] = -788.814211;
	MFinger.R[5][1] = 344.682000;
	MFinger.R[5][2] = -1125.710526;
	MFinger.R[5][3] = -984.963158;
	MFinger.R[5][4] = 0.580491;
	MFinger.R[5][5] = 2032.871053;


	MFinger.invR[0][0] = 0.529629;
	MFinger.invR[0][1] = 0.008552;
	MFinger.invR[0][2] = -0.530462;
	MFinger.invR[0][3] = -0.010065;
	MFinger.invR[0][4] = -146.296595;
	MFinger.invR[0][5] = -0.052785;

	MFinger.invR[1][0] = 0.008552;
	MFinger.invR[1][1] = 0.036847;
	MFinger.invR[1][2] = -0.008406;
	MFinger.invR[1][3] = -0.031746;
	MFinger.invR[1][4] = 1.035149;
	MFinger.invR[1][5] = -0.023261;

	MFinger.invR[2][0] = -0.530462;
	MFinger.invR[2][1] = -0.008406;
	MFinger.invR[2][2] = 0.531433;
	MFinger.invR[2][3] = 0.009892;
	MFinger.invR[2][4] = 146.558360;
	MFinger.invR[2][5] = 0.052817;

	MFinger.invR[3][0] = -0.010065;
	MFinger.invR[3][1] = -0.031746;
	MFinger.invR[3][2] = 0.009892;
	MFinger.invR[3][3] = 0.031387;
	MFinger.invR[3][4] = -1.029756;
	MFinger.invR[3][5] = 0.022456;

	MFinger.invR[4][0] = -146.296595;
	MFinger.invR[4][1] = 1.035149;
	MFinger.invR[4][2] = 146.558360;
	MFinger.invR[4][3] = -1.029756;
	MFinger.invR[4][4] = 41198.890066;
	MFinger.invR[4][5] = 11.950979;

	MFinger.invR[5][0] = -0.052785;
	MFinger.invR[5][1] = -0.023261;
	MFinger.invR[5][2] = 0.052817;
	MFinger.invR[5][3] = 0.022456;
	MFinger.invR[5][4] = 11.950979;
	MFinger.invR[5][5] = 0.020669;
	//palec wskazujacy


	 FFinger.P[0][0] = 7740.031841;
	 FFinger.P[0][1] = 414.422322;
	 FFinger.P[0][2] = -0.362811;
	 FFinger.P[0][3] = -339.554147;
	 FFinger.P[0][4] = -80.359658;

	 FFinger.P[1][0] = 414.422322;
	 FFinger.P[1][1] = 504.539142;
	 FFinger.P[1][2] = 0.232257;
	 FFinger.P[1][3] = -271.977598;
	 FFinger.P[1][4] = -72.707694;

	 FFinger.P[2][0] = -0.362811;
	 FFinger.P[2][1] = 0.232257;
	 FFinger.P[2][2] = 0.003018;
	 FFinger.P[2][3] = -0.023281;
	 FFinger.P[2][4] = 0.032337;

	 FFinger.P[3][0] = -339.554147;
	 FFinger.P[3][1] = -271.977598;
	 FFinger.P[3][2] = -0.023281;
	 FFinger.P[3][3] = 293.086467;
	 FFinger.P[3][4] = 89.050813;

	 FFinger.P[4][0] = -80.359658;
	 FFinger.P[4][1] = -72.707694;
	 FFinger.P[4][2] = 0.032337;
	 FFinger.P[4][3] = 89.050813;
	 FFinger.P[4][4] = 29.273343;


	FFinger.invP[0][0] = 0.000146;
	FFinger.invP[0][1] = -0.000006;
	FFinger.invP[0][2] = 0.050324;
	FFinger.invP[0][3] = 0.000888;
	FFinger.invP[0][4] = -0.002372;

	FFinger.invP[1][0] = -0.000006;
	FFinger.invP[1][1] = 0.004827;
	FFinger.invP[1][2] = -0.073208;
	FFinger.invP[1][3] = 0.010623;
	FFinger.invP[1][4] = -0.020261;

	FFinger.invP[2][0] = 0.050324;
	FFinger.invP[2][1] = -0.073208;
	FFinger.invP[2][2] = 450.229005;
	FFinger.invP[2][3] = 2.516361;
	FFinger.invP[2][4] = -8.195915;

	FFinger.invP[3][0] = 0.000888;
	FFinger.invP[3][1] = 0.010623;
	FFinger.invP[3][2] = 2.516361;
	FFinger.invP[3][3] = 0.086986;
	FFinger.invP[3][4] = -0.238571;

	FFinger.invP[4][0] = -0.002372;
	FFinger.invP[4][1] = -0.020261;
	FFinger.invP[4][2] = -8.195915;
	FFinger.invP[4][3] = -0.238571;
	FFinger.invP[4][4] = 0.712123;



	FFinger.R[0][0] = 7895.858947;
	FFinger.R[0][1] = 0.497900;
	FFinger.R[0][2] = 7689.533684;
	FFinger.R[0][3] = 728.568947;
	FFinger.R[0][4] = 0.248942;
	FFinger.R[0][5] = -788.814211;

	FFinger.R[1][0] = 0.497900;
	FFinger.R[1][1] = 303.854636;
	FFinger.R[1][2] = -55.510737;
	FFinger.R[1][3] = 116.235474;
	FFinger.R[1][4] = 0.319868;
	FFinger.R[1][5] = 344.682000;

	FFinger.R[2][0] = 7689.533684;
	FFinger.R[2][1] = -55.510737;
	FFinger.R[2][2] = 7755.694737;
	FFinger.R[2][3] = 537.700000;
	FFinger.R[2][4] = -0.600775;
	FFinger.R[2][5] = -623.257895;

	FFinger.R[3][0] = 728.568947;
	FFinger.R[3][1] = 116.235474;
	FFinger.R[3][2] = 537.700000;
	FFinger.R[3][3] = 836.892105;
	FFinger.R[3][4] = 0.198258;
	FFinger.R[3][5] = -975.544737;

	FFinger.R[4][0] = 0.248942;
	FFinger.R[4][1] = 0.319868;
	FFinger.R[4][2] = -0.600775;
	FFinger.R[4][3] = 0.198258;
	FFinger.R[4][4] = 0.003018;
	FFinger.R[4][5] = 0.269476;

	FFinger.R[5][0] = -788.814211;
	FFinger.R[5][1] = 344.682000;
	FFinger.R[5][2] = -623.257895;
	FFinger.R[5][3] = -975.544737;
	FFinger.R[5][4] = 0.269476;
	FFinger.R[5][5] = 2032.871053;


	FFinger.invR[0][0] = 0.709918;
	FFinger.invR[0][1] = 0.192100;
	FFinger.invR[0][2] = -0.708600;
	FFinger.invR[0][3] = -0.179833;
	FFinger.invR[0][4] = -205.161305;
	FFinger.invR[0][5] = -0.033455;

	FFinger.invR[1][0] = 0.192100;
	FFinger.invR[1][1] = 0.087830;
	FFinger.invR[1][2] = -0.191119;
	FFinger.invR[1][3] = -0.076314;
	FFinger.invR[1][4] = -55.665616;
	FFinger.invR[1][5] = -0.028189;

	FFinger.invR[2][0] = -0.708600;
	FFinger.invR[2][1] = -0.191119;
	FFinger.invR[2][2] = 0.707434;
	FFinger.invR[2][3] = 0.178894;
	FFinger.invR[2][4] = 204.815426;
	FFinger.invR[2][5] = 0.033038;

	FFinger.invR[3][0] = -0.179833;
	FFinger.invR[3][1] = -0.076314;
	FFinger.invR[3][2] = 0.178894;
	FFinger.invR[3][3] = 0.069998;
	FFinger.invR[3][4] = 51.722692;
	FFinger.invR[3][5] = 0.024741;

	FFinger.invR[4][0] = -205.161305;
	FFinger.invR[4][1] = -55.665616;
	FFinger.invR[4][2] = 204.815426;
	FFinger.invR[4][3] = 51.722692;
	FFinger.invR[4][4] = 59672.586988;
	FFinger.invR[4][5] = 9.534846;

	FFinger.invR[5][0] = -0.033455;
	FFinger.invR[5][1] = -0.028189;
	FFinger.invR[5][2] = 0.033038;
	FFinger.invR[5][3] = 0.024741;
	FFinger.invR[5][4] = 9.534846;
	FFinger.invR[5][5] = 0.013028;

	//kciuk

	 TFinger.P[0][0] = 7740.031841;
	 TFinger.P[0][1] = 414.422322;
	 TFinger.P[0][2] = -0.362811;
	 TFinger.P[0][3] = -339.554147;
	 TFinger.P[0][4] = -80.359658;

	 TFinger.P[1][0] = 414.422322;
	 TFinger.P[1][1] = 504.539142;
	 TFinger.P[1][2] = 0.232257;
	 TFinger.P[1][3] = -271.977598;
	 TFinger.P[1][4] = -72.707694;

	 TFinger.P[2][0] = -0.362811;
	 TFinger.P[2][1] = 0.232257;
	 TFinger.P[2][2] = 0.003018;
	 TFinger.P[2][3] = -0.023281;
	 TFinger.P[2][4] = 0.032337;

	 TFinger.P[3][0] = -339.554147;
	 TFinger.P[3][1] = -271.977598;
	 TFinger.P[3][2] = -0.023281;
	 TFinger.P[3][3] = 293.086467;
	 TFinger.P[3][4] = 89.050813;

	 TFinger.P[4][0] = -80.359658;
	 TFinger.P[4][1] = -72.707694;
	 TFinger.P[4][2] = 0.032337;
	 TFinger.P[4][3] = 89.050813;
	 TFinger.P[4][4] = 29.273343;


	TFinger.invP[0][0] = 0.000146;
	TFinger.invP[0][1] = -0.000006;
	TFinger.invP[0][2] = 0.050324;
	TFinger.invP[0][3] = 0.000888;
	TFinger.invP[0][4] = -0.002372;

	TFinger.invP[1][0] = -0.000006;
	TFinger.invP[1][1] = 0.004827;
	TFinger.invP[1][2] = -0.073208;
	TFinger.invP[1][3] = 0.010623;
	TFinger.invP[1][4] = -0.020261;

	TFinger.invP[2][0] = 0.050324;
	TFinger.invP[2][1] = -0.073208;
	TFinger.invP[2][2] = 450.229005;
	TFinger.invP[2][3] = 2.516361;
	TFinger.invP[2][4] = -8.195915;

	TFinger.invP[3][0] = 0.000888;
	TFinger.invP[3][1] = 0.010623;
	TFinger.invP[3][2] = 2.516361;
	TFinger.invP[3][3] = 0.086986;
	TFinger.invP[3][4] = -0.238571;

	TFinger.invP[4][0] = -0.002372;
	TFinger.invP[4][1] = -0.020261;
	TFinger.invP[4][2] = -8.195915;
	TFinger.invP[4][3] = -0.238571;
	TFinger.invP[4][4] = 0.712123;


	TFinger.R[0][0] = 7895.858947;
	TFinger.R[0][1] = 0.497900;
	TFinger.R[0][2] = 7543.161053;
	TFinger.R[0][3] = 201.359474;
	TFinger.R[0][4] = -0.069461;
	TFinger.R[0][5] = -788.814211;

	TFinger.R[1][0] = 0.497900;
	TFinger.R[1][1] = 303.854636;
	TFinger.R[1][2] = 121.068737;
	TFinger.R[1][3] = 247.388316;
	TFinger.R[1][4] = 0.011717;
	TFinger.R[1][5] = 344.682000;

	TFinger.R[2][0] = 7543.161053;
	TFinger.R[2][1] = 121.068737;
	TFinger.R[2][2] = 7655.418421;
	TFinger.R[2][3] = 232.400000;
	TFinger.R[2][4] = -0.762968;
	TFinger.R[2][5] = 116.334211;

	TFinger.R[3][0] = 201.359474;
	TFinger.R[3][1] = 247.388316;
	TFinger.R[3][2] = 232.400000;
	TFinger.R[3][3] = 356.568421;
	TFinger.R[3][4] = -0.547753;
	TFinger.R[3][5] = 7.147368;

	TFinger.R[4][0] = -0.069461;
	TFinger.R[4][1] = 0.011717;
	TFinger.R[4][2] = -0.762968;
	TFinger.R[4][3] = -0.547753;
	TFinger.R[4][4] = 0.004300;
	TFinger.R[4][5] = -0.572166;

	TFinger.R[5][0] = -788.814211;
	TFinger.R[5][1] = 344.682000;
	TFinger.R[5][2] = 116.334211;
	TFinger.R[5][3] = 7.147368;
	TFinger.R[5][4] = -0.572166;
	TFinger.R[5][5] = 2032.871053;


	TFinger.invR[0][0] = 0.317019;
	TFinger.invR[0][1] = 0.474137;
	TFinger.invR[0][2] = -0.316897;
	TFinger.invR[0][3] = -0.467519;
	TFinger.invR[0][4] = -107.693254;
	TFinger.invR[0][5] = 0.032089;

	TFinger.invR[1][0] = 0.474137;
	TFinger.invR[1][1] = 0.954881;
	TFinger.invR[1][2] = -0.473832;
	TFinger.invR[1][3] = -0.924260;
	TFinger.invR[1][4] = -197.176482;
	TFinger.invR[1][5] = -0.003056;

	TFinger.invR[2][0] = -0.316897;
	TFinger.invR[2][1] = -0.473832;
	TFinger.invR[2][2] = 0.316909;
	TFinger.invR[2][3] = 0.467163;
	TFinger.invR[2][4] = 107.649029;
	TFinger.invR[2][5] = -0.032105;

	TFinger.invR[3][0] = -0.467519;
	TFinger.invR[3][1] = -0.924260;
	TFinger.invR[3][2] = 0.467163;
	TFinger.invR[3][3] = 0.899069;
	TFinger.invR[3][4] = 192.339595;
	TFinger.invR[3][5] = -0.000459;

	TFinger.invR[4][0] = -107.693254;
	TFinger.invR[4][1] = -197.176482;
	TFinger.invR[4][2] = 107.649029;
	TFinger.invR[4][3] = 192.339595;
	TFinger.invR[4][4] = 42194.100515;
	TFinger.invR[4][5] = -3.316862;

	TFinger.invR[5][0] = 0.032089;
	TFinger.invR[5][1] = -0.003056;
	TFinger.invR[5][2] = -0.032105;
	TFinger.invR[5][3] = -0.000459;
	TFinger.invR[5][4] = -3.316862;
	TFinger.invR[5][5] = 0.014367;

	//mały palec


	 SFinger.P[0][0] = 8930.221039;
	 SFinger.P[0][1] = 104.922314;
	 SFinger.P[0][2] = -1.480602;
	 SFinger.P[0][3] = -300.765973;
	 SFinger.P[0][4] = -194.988840;

	 SFinger.P[1][0] = 104.922314;
	 SFinger.P[1][1] = 573.612657;
	 SFinger.P[1][2] = 1.191833;
	 SFinger.P[1][3] = -58.597483;
	 SFinger.P[1][4] = -8.765255;

	 SFinger.P[2][0] = -1.480602;
	 SFinger.P[2][1] = 1.191833;
	 SFinger.P[2][2] = 0.005316;
	 SFinger.P[2][3] = 0.106997;
	 SFinger.P[2][4] = 0.157024;

	 SFinger.P[3][0] = -300.765973;
	 SFinger.P[3][1] = -58.597483;
	 SFinger.P[3][2] = 0.106997;
	 SFinger.P[3][3] = 81.452438;
	 SFinger.P[3][4] = 47.097304;

	 SFinger.P[4][0] = -194.988840;
	 SFinger.P[4][1] = -8.765255;
	 SFinger.P[4][2] = 0.157024;
	 SFinger.P[4][3] = 47.097304;
	 SFinger.P[4][4] = 29.273343;


	SFinger.invP[0][0] = 0.000134;
	SFinger.invP[0][1] = -0.000103;
	SFinger.invP[0][2] = 0.032291;
	SFinger.invP[0][3] = -0.000287;
	SFinger.invP[0][4] = 0.001151;

	SFinger.invP[1][0] = -0.000103;
	SFinger.invP[1][1] = 0.004855;
	SFinger.invP[1][2] = -0.656576;
	SFinger.invP[1][3] = 0.021439;
	SFinger.invP[1][4] = -0.030206;

	SFinger.invP[2][0] = 0.032291;
	SFinger.invP[2][1] = -0.656576;
	SFinger.invP[2][2] = 1445.444163;
	SFinger.invP[2][3] = 31.852714;
	SFinger.invP[2][4] = -58.982171;

	SFinger.invP[3][0] = -0.000287;
	SFinger.invP[3][1] = 0.021439;
	SFinger.invP[3][2] = 31.852714;
	SFinger.invP[3][3] = 1.161668;
	SFinger.invP[3][4] = -2.035340;

	SFinger.invP[4][0] = 0.001151;
	SFinger.invP[4][1] = -0.030206;
	SFinger.invP[4][2] = -58.982171;
	SFinger.invP[4][3] = -2.035340;
	SFinger.invP[4][4] = 3.623785;



	SFinger.R[0][0] = 7895.858947;
	SFinger.R[0][1] = 0.497900;
	SFinger.R[0][2] = 8294.980000;
	SFinger.R[0][3] = 209.081579;
	SFinger.R[0][4] = -0.564469;
	SFinger.R[0][5] = -788.814211;

	SFinger.R[1][0] = 0.497900;
	SFinger.R[1][1] = 303.854636;
	SFinger.R[1][2] = -205.001263;
	SFinger.R[1][3] = 316.732737;
	SFinger.R[1][4] = 0.614738;
	SFinger.R[1][5] = 344.682000;

	SFinger.R[2][0] = 8294.980000;
	SFinger.R[2][1] = -205.001263;
	SFinger.R[2][2] = 9258.957895;
	SFinger.R[2][3] = 155.100000;
	SFinger.R[2][4] = -1.681815;
	SFinger.R[2][5] = -1808.468421;

	SFinger.R[3][0] = 209.081579;
	SFinger.R[3][1] = 316.732737;
	SFinger.R[3][2] = 155.100000;
	SFinger.R[3][3] = 699.502632;
	SFinger.R[3][4] = 1.318676;
	SFinger.R[3][5] = -164.713158;

	SFinger.R[4][0] = -0.564469;
	SFinger.R[4][1] = 0.614738;
	SFinger.R[4][2] = -1.681815;
	SFinger.R[4][3] = 1.318676;
	SFinger.R[4][4] = 0.005316;
	SFinger.R[4][5] = 1.308535;

	SFinger.R[5][0] = -788.814211;
	SFinger.R[5][1] = 344.682000;
	SFinger.R[5][2] = -1808.468421;
	SFinger.R[5][3] = -164.713158;
	SFinger.R[5][4] = 1.308535;
	SFinger.R[5][5] = 2032.871053;


	SFinger.invR[0][0] = 0.219949;
	SFinger.invR[0][1] = -0.112095;
	SFinger.invR[0][2] = -0.220321;
	SFinger.invR[0][3] = 0.115824;
	SFinger.invR[0][4] = -49.750982;
	SFinger.invR[0][5] = -0.050239;

	SFinger.invR[1][0] = -0.112095;
	SFinger.invR[1][1] = 0.342395;
	SFinger.invR[1][2] = 0.112924;
	SFinger.invR[1][3] = -0.328952;
	SFinger.invR[1][4] = 86.338974;
	SFinger.invR[1][5] = -0.083321;

	SFinger.invR[2][0] = -0.220321;
	SFinger.invR[2][1] = 0.112924;
	SFinger.invR[2][2] = 0.220830;
	SFinger.invR[2][3] = -0.116725;
	SFinger.invR[2][4] = 50.019260;
	SFinger.invR[2][5] = 0.050161;

	SFinger.invR[3][0] = 0.115824;
	SFinger.invR[3][1] = -0.328952;
	SFinger.invR[3][2] = -0.116725;
	SFinger.invR[3][3] = 0.320773;
	SFinger.invR[3][4] = -85.307683;
	SFinger.invR[3][5] = 0.077780;

	SFinger.invR[4][0] = -49.750982;
	SFinger.invR[4][1] = 86.338974;
	SFinger.invR[4][2] = 50.019260;
	SFinger.invR[4][3] = -85.307683;
	SFinger.invR[4][4] = 24966.972592;
	SFinger.invR[4][5] = -12.429204;

	SFinger.invR[5][0] = -0.050239;
	SFinger.invR[5][1] = -0.083321;
	SFinger.invR[5][2] = 0.050161;
	SFinger.invR[5][3] = 0.077780;
	SFinger.invR[5][4] = -12.429204;
	SFinger.invR[5][5] = 0.054051;


	//palec serdeczny

	 RFinger.P[0][0] = 8465.374582;
	 RFinger.P[0][1] = 345.004152;
	 RFinger.P[0][2] = -1.033221;
	 RFinger.P[0][3] = -431.608269;
	 RFinger.P[0][4] = -153.121165;

	 RFinger.P[1][0] = 345.004152;
	 RFinger.P[1][1] = 548.874626;
	 RFinger.P[1][2] = 0.788965;
	 RFinger.P[1][3] = -210.942662;
	 RFinger.P[1][4] = -54.042831;

	 RFinger.P[2][0] = -1.033221;
	 RFinger.P[2][1] = 0.788965;
	 RFinger.P[2][2] = 0.004481;
	 RFinger.P[2][3] = 0.065347;
	 RFinger.P[2][4] = 0.101536;

	 RFinger.P[3][0] = -431.608269;
	 RFinger.P[3][1] = -210.942662;
	 RFinger.P[3][2] = 0.065347;
	 RFinger.P[3][3] = 243.952191;
	 RFinger.P[3][4] = 80.730688;

	 RFinger.P[4][0] = -153.121165;
	 RFinger.P[4][1] = -54.042831;
	 RFinger.P[4][2] = 0.101536;
	 RFinger.P[4][3] = 80.730688;
	 RFinger.P[4][4] = 29.273343;


	RFinger.invP[0][0] = 0.000135;
	RFinger.invP[0][1] = -0.000094;
	RFinger.invP[0][2] = 0.057865;
	RFinger.invP[0][3] = 0.000370;
	RFinger.invP[0][4] = -0.000686;

	RFinger.invP[1][0] = -0.000094;
	RFinger.invP[1][1] = 0.004920;
	RFinger.invP[1][2] = -0.911412;
	RFinger.invP[1][3] = 0.005069;
	RFinger.invP[1][4] = -0.002224;

	RFinger.invP[2][0] = 0.057865;
	RFinger.invP[2][1] = -0.911412;
	RFinger.invP[2][2] = 689.786014;
	RFinger.invP[2][3] = 4.326258;
	RFinger.invP[2][4] = -15.703518;

	RFinger.invP[3][0] = 0.000370;
	RFinger.invP[3][1] = 0.005069;
	RFinger.invP[3][2] = 4.326258;
	RFinger.invP[3][3] = 0.105388;
	RFinger.invP[3][4] = -0.294351;

	RFinger.invP[4][0] = -0.000686;
	RFinger.invP[4][1] = -0.002224;
	RFinger.invP[4][2] = -15.703518;
	RFinger.invP[4][3] = -0.294351;
	RFinger.invP[4][4] = 0.892699;

	RFinger.R[0][0] = 7895.858947;
	RFinger.R[0][1] = 0.497900;
	RFinger.R[0][2] = 8133.765263;
	RFinger.R[0][3] = 532.851579;
	RFinger.R[0][4] = -0.175140;
	RFinger.R[0][5] = -788.814211;

	RFinger.R[1][0] = 0.497900;
	RFinger.R[1][1] = 303.854636;
	RFinger.R[1][2] = -204.310947;
	RFinger.R[1][3] = 194.939053;
	RFinger.R[1][4] = 0.563388;
	RFinger.R[1][5] = 344.682000;

	RFinger.R[2][0] = 8133.765263;
	RFinger.R[2][1] = -204.310947;
	RFinger.R[2][2] = 8788.515789;
	RFinger.R[2][3] = 525.594737;
	RFinger.R[2][4] = -1.350547;
	RFinger.R[2][5] = -1456.226316;

	RFinger.R[3][0] = 532.851579;
	RFinger.R[3][1] = 194.939053;
	RFinger.R[3][2] = 525.594737;
	RFinger.R[3][3] = 844.239474;
	RFinger.R[3][4] = 0.872489;
	RFinger.R[3][5] = -744.423684;

	RFinger.R[4][0] = -0.175140;
	RFinger.R[4][1] = 0.563388;
	RFinger.R[4][2] = -1.350547;
	RFinger.R[4][3] = 0.872489;
	RFinger.R[4][4] = 0.004481;
	RFinger.R[4][5] = 0.846130;

	RFinger.R[5][0] = -788.814211;
	RFinger.R[5][1] = 344.682000;
	RFinger.R[5][2] = -1456.226316;
	RFinger.R[5][3] = -744.423684;
	RFinger.R[5][4] = 0.846130;
	RFinger.R[5][5] = 2032.871053;


	RFinger.invR[0][0] = 0.630912;
	RFinger.invR[0][1] = -0.107137;
	RFinger.invR[0][2] = -0.630910;
	RFinger.invR[0][3] = 0.107016;
	RFinger.invR[0][4] = -156.927437;
	RFinger.invR[0][5] = -0.084462;

	RFinger.invR[1][0] = -0.107137;
	RFinger.invR[1][1] = 0.052635;
	RFinger.invR[1][2] = 0.107410;
	RFinger.invR[1][3] = -0.048107;
	RFinger.invR[1][4] = 31.767143;
	RFinger.invR[1][5] = -0.004394;

	RFinger.invR[2][0] = -0.630910;
	RFinger.invR[2][1] = 0.107410;
	RFinger.invR[2][2] = 0.631045;
	RFinger.invR[2][3] = -0.107372;
	RFinger.invR[2][4] = 157.025706;
	RFinger.invR[2][5] = 0.084342;

	RFinger.invR[3][0] = 0.107016;
	RFinger.invR[3][1] = -0.048107;
	RFinger.invR[3][2] = -0.107372;
	RFinger.invR[3][3] = 0.048565;
	RFinger.invR[3][4] = -32.347568;
	RFinger.invR[3][5] = 0.004015;

	RFinger.invR[4][0] = -156.927437;
	RFinger.invR[4][1] = 31.767143;
	RFinger.invR[4][2] = 157.025706;
	RFinger.invR[4][3] = -32.347568;
	RFinger.invR[4][4] = 40412.922201;
	RFinger.invR[4][5] = 17.538712;

	RFinger.invR[5][0] = -0.084462;
	RFinger.invR[5][1] = -0.004394;
	RFinger.invR[5][2] = 0.084342;
	RFinger.invR[5][3] = 0.004015;
	RFinger.invR[5][4] = 17.538712;
	RFinger.invR[5][5] = 0.023051;


/*

	 RFinger.P[0][0] = 7888.528068;
	 RFinger.P[0][1] = 406.435705;
	 RFinger.P[0][2] = -0.135018;
	 RFinger.P[0][3] = -435.621592;
	 RFinger.P[0][4] = -122.957084;

	 RFinger.P[1][0] = 406.435705;
	 RFinger.P[1][1] = 521.170785;
	 RFinger.P[1][2] = 0.491322;
	 RFinger.P[1][3] = -285.146180;
	 RFinger.P[1][4] = -70.328615;

	 RFinger.P[2][0] = -0.135018;
	 RFinger.P[2][1] = 0.491322;
	 RFinger.P[2][2] = 0.003764;
	 RFinger.P[2][3] = 0.034951;
	 RFinger.P[2][4] = 0.069659;

	 RFinger.P[3][0] = -435.621592;
	 RFinger.P[3][1] = -285.146180;
	 RFinger.P[3][2] = 0.034951;
	 RFinger.P[3][3] = 348.553026;
	 RFinger.P[3][4] = 97.536609;

	 RFinger.P[4][0] = -122.957084;
	 RFinger.P[4][1] = -70.328615;
	 RFinger.P[4][2] = 0.069659;
	 RFinger.P[4][3] = 97.536609;
	 RFinger.P[4][4] = 29.273343;


	RFinger.invR[0][0] = 0.529629;
	RFinger.invR[0][1] = 0.008552;
	RFinger.invR[0][2] = -0.530462;
	RFinger.invR[0][3] = -0.010065;
	RFinger.invR[0][4] = -146.296595;
	RFinger.invR[0][5] = -0.052785;

	RFinger.invR[1][0] = 0.008552;
	RFinger.invR[1][1] = 0.036847;
	RFinger.invR[1][2] = -0.008406;
	RFinger.invR[1][3] = -0.031746;
	RFinger.invR[1][4] = 1.035149;
	RFinger.invR[1][5] = -0.023261;

	RFinger.invR[2][0] = -0.530462;
	RFinger.invR[2][1] = -0.008406;
	RFinger.invR[2][2] = 0.531433;
	RFinger.invR[2][3] = 0.009892;
	RFinger.invR[2][4] = 146.558360;
	RFinger.invR[2][5] = 0.052817;

	RFinger.invR[3][0] = -0.010065;
	RFinger.invR[3][1] = -0.031746;
	RFinger.invR[3][2] = 0.009892;
	RFinger.invR[3][3] = 0.031387;
	RFinger.invR[3][4] = -1.029756;
	RFinger.invR[3][5] = 0.022456;

	RFinger.invR[4][0] = -146.296595;
	RFinger.invR[4][1] = 1.035149;
	RFinger.invR[4][2] = 146.558360;
	RFinger.invR[4][3] = -1.029756;
	RFinger.invR[4][4] = 41198.890066;
	RFinger.invR[4][5] = 11.950979;

	RFinger.invR[5][0] = -0.052785;
	RFinger.invR[5][1] = -0.023261;
	RFinger.invR[5][2] = 0.052817;
	RFinger.invR[5][3] = 0.022456;
	RFinger.invR[5][4] = 11.950979;
	RFinger.invR[5][5] = 0.020669;

*/
}


}//: namespace MAP_Estimation
}//: namespace Processors
