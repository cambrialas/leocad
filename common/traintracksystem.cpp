#include "piece.h"
#include "traintracksystem.h"
#include "lc_model.h"
#include "lc_application.h"
#include "lc_library.h"
#include <QString>
#include <iostream>
#include <iomanip>

//const double epsilon = 1e-4;

const float precisionErrorNumber = 0.01;

TrainTrackConnectionPoint::TrainTrackConnectionPoint(float angle, float x, float y) {
	this->angle = angle;
	xy_coordinate.x = x;
	xy_coordinate.y = y;
}



void TrainTrackType::Load(lcPiecesLibrary* Library, char const* filename) {
	pieceInfo = Library->FindPiece(filename, nullptr, false, false);
	GenerateSplineAndDirection();
}

void TrainTrackType::AddConnection(TrainTrackConnectionPoint *connectionPoint) {
    connectionPoints.push_back(connectionPoint);
}

TrainTrackConnectionPoint* TrainTrackType::GetConnection(int index) {
	return connectionPoints[index];	
}

std::vector<TrainTrackConnectionPoint *>& TrainTrackType::GetConnections() {
	return connectionPoints;	
}

int TrainTrackType::GetNoOfConnections() {
	return connectionPoints.size();
}


void TrainTrackType::BezierLine(lcVector2* pointList, int NumberOfPoints, std::vector<struct SplineElement>* spline, int NumberOfResultPoints) {

    lcVector2 pointListOrders[NumberOfPoints];

    double PercentStep = 1 / (double)( NumberOfResultPoints - 1 );
    
    for(int resultCounter=0; resultCounter < NumberOfResultPoints; resultCounter++) {
        
        double Percentage = ( PercentStep * resultCounter );
		struct SplineElement splineElement;
        //cout << resultCounter << ", percent: " << Percentage << endl;
                
        //Copy from paramter 'pointList to pointListOrders and calculate 
        for(int pointNo=0;pointNo < NumberOfPoints-1; pointNo++) {
            
            pointListOrders[pointNo].x = pointList[pointNo].x + (( pointList[pointNo+1].x - pointList[pointNo].x ) * Percentage);
            pointListOrders[pointNo].y = pointList[pointNo].y + (( pointList[pointNo+1].y - pointList[pointNo].y ) * Percentage);                
            //cout << "Point no: " << pointNo << ",x: " << pointListOrders[pointNo].x << ",y: " << pointListOrders[pointNo].y << endl;    
        }


        for(int orders=NumberOfPoints-2;orders >= 1; orders--) {

            //cout << "Orders: " << orders << endl;			
			if(orders == 1) {
				splineElement.rotation = LC_RTOD * lcGetAngle(pointListOrders[0].x, pointListOrders[0].y, pointListOrders[1].x , pointListOrders[1].y);
			}

            for(int pointNo = 0; pointNo < orders; pointNo++) {
                
                pointListOrders[pointNo].x = pointListOrders[pointNo].x + (( pointListOrders[pointNo+1].x - pointListOrders[pointNo].x ) * Percentage);
                pointListOrders[pointNo].y = pointListOrders[pointNo].y + (( pointListOrders[pointNo+1].y - pointListOrders[pointNo].y ) * Percentage);                
                //cout << "Point no: " << pointNo << ",x: " << pointListOrders[pointNo].x << ",y: " << pointListOrders[pointNo].y << endl;    
            }
            
        }

        splineElement.location.x = pointListOrders[0].x;
        splineElement.location.y = pointListOrders[0].y;

		spline->push_back(splineElement);

        //cout << "Bezier point, x: " << pointListOrders[0].x << ", y: " << pointListOrders[0].y << endl << endl;                   
    }
}


void TrainTrackType::GenerateSplineAndDirection() {

	/*enum LC_TTS_TYPES trackTypeId
	enum LC_TTS_TYPES {
		LC_TTS_STRAIGHT,
		LC_TTS_CURVED,
		LC_TTS_CROSS,
		LC_TTS_LEFT_BRANCH,
		LC_TTS_RIGHT_BRANCH,
		LC_TTS_NUMITEMS
	};*/
	std::vector<struct SplineElement>* vectorSpline;
	
	Spline *spline;
	
	switch(this->trackTypeId) {
		
		case LC_TTS_STRAIGHT: {

			vectorSpline = new std::vector<struct SplineElement>();

			for(int itr = 0; itr <= 100; itr++) {

				struct SplineElement splineElement;					
				splineElement.location = lcVector2(-160.0 + ((320.0 / 100.0) * itr ), 0);
			
				splineElement.rotation = 0;
				vectorSpline->push_back(splineElement);

			}

			spline = new Spline(vectorSpline, 0, 1, Direction::Backward);
			vectorSplines.push_back(spline);

			spline = new Spline(vectorSpline, 1, 0, Direction::Forward);
			vectorSplines.push_back(spline);

			break;
		}

		case LC_TTS_CURVED: {

			vectorSpline = new std::vector<struct SplineElement>();

			for(int itr = 0; itr <= 100; itr++) {

				float degree = ((22.5f / 100.0f) * itr) - 11.25f;

				float xHalfBendOff = sin(LC_DTOR * degree) * 800.0f;
				float yHalfBendOff = ((cos(LC_DTOR * degree) * 800.0f) - 800.0f);
				
				
				struct SplineElement splineElement;
				splineElement.location = lcVector2(xHalfBendOff, yHalfBendOff);
			
				splineElement.rotation = -degree;
				vectorSpline->push_back(splineElement);
			}

			spline = new Spline(vectorSpline, 0, 1, Direction::Backward);
			vectorSplines.push_back(spline);

			spline = new Spline(vectorSpline, 1, 0, Direction::Forward);
			vectorSplines.push_back(spline);
			
			break;


		}

		case LC_TTS_CROSS: {

			Spline* crossSplines[4];

			vectorSpline = new std::vector<struct SplineElement>();
			
			for(int itr = 0; itr <= 100; itr++) {

				struct SplineElement splineElement;					
				splineElement.location = lcVector2(-160.0 + ((320.0 / 100.0) * itr ), 0);
			
				splineElement.rotation = 0;
				vectorSpline->push_back(splineElement);
			}
			
			spline = new Spline(vectorSpline, 2, 0, Direction::Forward);
			crossSplines[2] = spline;

			spline = new Spline(vectorSpline, 0, 2, Direction::Backward);
			crossSplines[0] = spline;


			// next spline
			vectorSpline = new std::vector<struct SplineElement>();

			for(int itr = 0; itr <= 100; itr++) {

				struct SplineElement splineElement;
				splineElement.location = lcVector2(0, -160.0 + ((320.0 / 100.0) * itr ));
			
				splineElement.rotation = 90;
				vectorSpline->push_back(splineElement);
			}

			spline = new Spline(vectorSpline, 3, 1, Direction::Forward);
			crossSplines[3] = spline;

			spline = new Spline(vectorSpline, 1, 3, Direction::Backward);
			crossSplines[1] = spline;

			vectorSplines.push_back(crossSplines[0]);
			vectorSplines.push_back(crossSplines[1]);
			vectorSplines.push_back(crossSplines[2]);
			vectorSplines.push_back(crossSplines[3]);

			break;

		}

		case LC_TTS_LEFT_BRANCH: {


			// Connction point 0 -> 2

			vectorSpline = new std::vector<struct SplineElement>();

			for(int itr = 0; itr <= 200; itr++) {

				struct SplineElement splineElement;					
				splineElement.location = lcVector2(-320.0 + ((640.0 / 200.0) * itr ), 0);
			
				splineElement.rotation = 0;
				vectorSpline->push_back(splineElement);

			}

			spline = new Spline(vectorSpline, 0, 2, Direction::Backward);
			vectorSplines.push_back(spline);


			// Connction point 2 -> 1 and 1 -> 2

			float p0_transform_x = -320;
			float p0_transform_y = 0;
		
			float p1_transform_x = 0;
			float p1_transform_y = 0;
		
			float p2_transform_x = 0.4683227539;
			float p2_transform_y = 121.0110168;
		
			float p3_transform_x = 333.85324096679688;
			float p3_transform_y = 259.10357666015625;
							
			lcVector2 pointList[] = { 
				lcVector2(p0_transform_x, p0_transform_y), 
				lcVector2(p1_transform_x, p1_transform_y), 
				lcVector2(p2_transform_x, p2_transform_y), 
				lcVector2(p3_transform_x, p3_transform_y) 
			};

			vectorSpline = new std::vector<struct SplineElement>();

			BezierLine(pointList, 4, vectorSpline, 200);

			for(int itr = 0; itr < 200; itr++) {
				std::cout << "pointListResult[" << itr << "] x: " << vectorSpline->at(itr).location.x << ", y: " << vectorSpline->at(itr).location.y << ", rotation: " << vectorSpline->at(itr).rotation << "\n";
			}

			spline = new Spline(vectorSpline, 1, 2, Direction::Backward);
			vectorSplines.push_back(spline);

			spline = new Spline(vectorSpline, 2, 1, Direction::Forward);
			vectorSplines.push_back(spline);


			break;
		}

		case LC_TTS_RIGHT_BRANCH: {


			// Connction point 0 -> 1

			vectorSpline = new std::vector<struct SplineElement>();

			for(int itr = 0; itr <= 200; itr++) {

				struct SplineElement splineElement;					
				splineElement.location = lcVector2(-320.0 + ((640.0 / 200.0) * itr ), 0);
			
				splineElement.rotation = 0;
				vectorSpline->push_back(splineElement);

			}

			spline = new Spline(vectorSpline, 0, 1, Direction::Backward);
			vectorSplines.push_back(spline);


			// Connction point 1 -> 2 and 2 -> 1

			float p0_transform_x = -320;
			float p0_transform_y = 0;
		
			float p1_transform_x = 0;
			float p1_transform_y = 0;
		
			float p2_transform_x = 0.4683227539;
			float p2_transform_y = -121.0110168;
		
			float p3_transform_x = 333.85324096679688;
			float p3_transform_y = -259.10357666015625;
							
			lcVector2 pointList[] = { 
				lcVector2(p0_transform_x, p0_transform_y), 
				lcVector2(p1_transform_x, p1_transform_y), 
				lcVector2(p2_transform_x, p2_transform_y), 
				lcVector2(p3_transform_x, p3_transform_y) 
			};

			vectorSpline = new std::vector<struct SplineElement>();

			BezierLine(pointList, 4, vectorSpline, 200);

			for(int itr = 0; itr < 200; itr++) {
				std::cout << "pointListResult[" << itr << "] x: " << vectorSpline->at(itr).location.x << ", y: " << vectorSpline->at(itr).location.y << ", rotation: " << vectorSpline->at(itr).rotation << "\n";
			}

			spline = new Spline(vectorSpline, 1, 2, Direction::Forward);
			vectorSplines.push_back(spline);

			spline = new Spline(vectorSpline, 2, 1, Direction::Backward);
			vectorSplines.push_back(spline);

			break;
		}		
		
	}
}




TrainTrackSection::TrainTrackSection(TrainTrackType *trackType, lcMatrix44 location, int mCurrentStep, int mColorIndex) {
	this->trackType = trackType;
	this->location = location;
	this->mCurrentStep = mCurrentStep;
	this->mColorIndex = mColorIndex;
	
}


lcMatrix44 TrainTrackSection::GetConnectionLocation(int connectionNo) {
	
	lcMatrix44 mat44;
	lcMatrix44 matOffset;

	TrainTrackConnectionPoint* connection = trackType->GetConnections()[connectionNo];
	
	mat44 = lcMatrix44Identity();
	mat44 =	lcMatrix44RotationZ(LC_DTOR * connection->angle);

	matOffset = lcMatrix44Identity();
	matOffset.SetTranslation(lcVector3(connection->xy_coordinate.x, connection->xy_coordinate.y,0)); 

	matOffset = lcMul(mat44, matOffset);

	return lcMul(matOffset,location);
}


lcMatrix44 TrainTrackSection::GetSplineLocation(int splineNo, int splineIdx) {
	
	lcVector2 loc2Spline = trackType->vectorSplines[splineNo]->GetLocation(splineIdx);

	lcMatrix44 matOffset = lcMatrix44Identity();
	matOffset.SetTranslation(lcVector3(loc2Spline.x, loc2Spline.y, 0)); 

	return lcMul(matOffset, location);
}


int TrainTrackSection::GetNoOfConnections() {
	return trackType->GetNoOfConnections();	
}

int TrainTrackSection::FindConnectionNumber(lcVector3 v3searchLocation) {
	float precisionErrorNumber = 0.001;

	lcMatrix44 matOffset;

	for(int con_no = 0; con_no < trackType->GetNoOfConnections(); con_no++) {

		lcMatrix44 matSelection = GetConnectionLocation(con_no);
		
		matOffset = lcMatrix44Identity();
		matOffset.SetTranslation(lcVector3(-10, 0, 8)); 

		lcVector3 v3Selection = lcMul(matOffset,matSelection).GetTranslation();

		if(abs(v3Selection.x - v3searchLocation.x) < precisionErrorNumber &&
			abs(v3Selection.y - v3searchLocation.y) < precisionErrorNumber &&
			abs(v3Selection.z - v3searchLocation.z) < precisionErrorNumber) 
		{
			return con_no;
		}
	}
	return -1;
}


TrainLocomotive::TrainLocomotive(TrainTrackSystem* trackSystem) {

	mTrackSystem = trackSystem;
	mCurrentStep = 1;
}


void TrainLocomotive::CreateLocomotive() {

	// Place 
	trainLocation.section = mTrackSystem->trackSections[0];
	trainLocation.splineIdx = 20;
	trainLocation.spline = trainLocation.section->trackType->vectorSplines[1];

	mat44BogieWheel2 = lcMatrix44Identity();
	mat44BasePlate = lcMatrix44Identity();

}


void TrainLocomotive::IncrementTrainLocation() {


	Spline *curSpline = trainLocation.spline;

	trainLocation.splineIdx++;

	if(trainLocation.splineIdx >= (curSpline->mSpline->size())) {

		// Find next section, train going forward

		int fromConnectionPoint = trainLocation.spline->mToConnectionPointNo;

		std::cout << "fromConnectionPoint(NEW): " << fromConnectionPoint << "\n";


		
		int toNewTrackConnectionIdx;		

		TrainTrackSection* toSection = mTrackSystem->findSectionConnectedTo(trainLocation.section, fromConnectionPoint, toNewTrackConnectionIdx);
		
		if(toSection != nullptr) {
			
			std::cout << "Got new section\n";
			std::cout << "toNewTrackConnectionIdx: " << toNewTrackConnectionIdx << "\n"; 

			trainLocation.spline = toSection->trackType->vectorSplines[toNewTrackConnectionIdx];
			trainLocation.section = toSection;
			trainLocation.splineIdx = 0;

		}
		else {
			std::cout << "Got *NO* new section\n";
			trainLocation.splineIdx--;
		}
		
	}

	std::cout << "START -- Find location of following bogie wheels\n";

	// Find previous section
	
	lcVector3 locBogieVec3 = GetBoogieWorldPosition().GetTranslation();
	
	locBogieVec3.z = 0;
	
	std::cout << "locomotive x: " << locBogieVec3.x << ", y: " << locBogieVec3.y << ", z: " << locBogieVec3.z << "\n";

	double distance = 0;

	int fromConnectionPoint = trainLocation.spline->mFromConnectionPointNo;

	std::cout << "fromConnectionPoint(NEW): " << fromConnectionPoint << "\n";

	TrainTrackSection *toPrevSection = trainLocation.section;
	
	lcVector3 connLocVector3 = toPrevSection->GetConnectionLocation(fromConnectionPoint).GetTranslation();

	distance = lcDistance(connLocVector3, locBogieVec3);

	std::cout << "Distance: " << distance << "\n";

	int toNewTrackConnectionIdx = trainLocation.spline->mToConnectionPointNo;

	// Start -- Make spline to go in opposite direction
	/*Spline prevSectionSpline = Spline(trainLocation.spline->mSpline,trainLocation.spline->mToConnectionPointNo,trainLocation.spline->mFromConnectionPointNo,Direction::Forward);

	if(trainLocation.spline->mSplineDirection == Direction::Backward)
		prevSectionSpline.mSplineDirection = Direction::Forward;
	else
		prevSectionSpline.mSplineDirection = Direction::Backward;*/
	// End -- Make spline to go in opposite direction


	Spline* prevSectionSpline = trainLocation.section->trackType->vectorSplines[toNewTrackConnectionIdx];
	

	bool noMoreTracks = false;
	
	while(distance < 360 && noMoreTracks == false) {

		toPrevSection = mTrackSystem->findSectionConnectedTo(toPrevSection, fromConnectionPoint, toNewTrackConnectionIdx);

		if(toPrevSection != nullptr) {

			prevSectionSpline = toPrevSection->trackType->vectorSplines[toNewTrackConnectionIdx];
			
			fromConnectionPoint = prevSectionSpline->mToConnectionPointNo;
			
			std::cout << "Got new section\n";
			//std::cout << "toNewTrackConnectionIdx: " << toNewTrackConnectionIdx << "\n"; 
	
			lcVector3 connLocVector3 = toPrevSection->GetConnectionLocation(fromConnectionPoint).GetTranslation();
			
			std::cout << "toPrevSection x: " << connLocVector3.x << ", y: " << connLocVector3.y << ", z: " << connLocVector3.z << "\n";
		
			distance = lcDistance(connLocVector3, locBogieVec3);
	
			std::cout << "Distance: " << distance << "\n";
		
		}
		else {
			noMoreTracks = true;
		}	
		
	}

	// Find lenght from bogie to 360
	if(noMoreTracks == false) {

		int maxPtr = prevSectionSpline->mSpline->size() - 1;
		int minPtr = 0;

		int middlePtr;

		bool bExactFound = false;

		while(abs(minPtr - maxPtr) > 1 && bExactFound == false) {
		
			middlePtr = (maxPtr + minPtr) / 2;

			lcMatrix44 mat44SplineLoc = toPrevSection->GetSplineLocation(toNewTrackConnectionIdx, middlePtr);

			lcVector3 loc3SplineWorld = mat44SplineLoc.GetTranslation();

			std::cout << "loc3SplineWorld x: " << loc3SplineWorld.x << ", y: " << loc3SplineWorld.y << ", z: " << loc3SplineWorld.z << "\n";		
		
			distance = lcDistance(loc3SplineWorld, locBogieVec3);

			std::cout << "Distance: " << distance << "\n";

			if(abs(distance - 360) < precisionErrorNumber) {
				bExactFound = true;
			}
			else {
				if(distance > 360) {
					std::cout << "maxPtr = middlePtr\n";
					maxPtr = middlePtr;
				}
				else {
					std::cout << "minPtr = middlePtr\n";
					minPtr = middlePtr;
				}
			}
			std::cout << "maxPtr: " << maxPtr << ", minPtr: " << minPtr << "\n";

		}

		if(bExactFound == false) {
			std::cout << std::setprecision(15);
			lcMatrix44 mat44SplineLoc = toPrevSection->GetSplineLocation(toNewTrackConnectionIdx, minPtr);
			lcVector3 loc3SplineWorldMin = mat44SplineLoc.GetTranslation();
			distance = lcDistance(loc3SplineWorldMin, locBogieVec3);
			std::cout << "Distance minPtr: " << distance << "\n";

			mat44SplineLoc = toPrevSection->GetSplineLocation(toNewTrackConnectionIdx, maxPtr);
			lcVector3 loc3SplineWorldMax = mat44SplineLoc.GetTranslation();
			distance = lcDistance(loc3SplineWorldMax, locBogieVec3);
			std::cout << "Distance maxPtr: " << distance << "\n";

			std::vector<Point2D> points = lcFindIntersectionLineCircle(Point2D(loc3SplineWorldMin.x, loc3SplineWorldMin.y),
										Point2D(loc3SplineWorldMax.x, loc3SplineWorldMax.y),
										Point2D(locBogieVec3.x, locBogieVec3.y),
										360);

			lcVector3 loc3SplineWorldInterpolation = lcVector3(points[0].x, points[0].y, 0);
			distance = lcDistance(loc3SplineWorldInterpolation, locBogieVec3);
			std::cout << "Distance interpolate: " << distance << "\n";


			mat44BogieWheel2  = lcMatrix44Identity();

			double rotItrMin = toPrevSection->trackType->vectorSplines[toNewTrackConnectionIdx]->GetRotation(minPtr);
			double rotItrMax = toPrevSection->trackType->vectorSplines[toNewTrackConnectionIdx]->GetRotation(maxPtr);

			double middleRotation = (rotItrMax + rotItrMin) / 2;
		

			//Get location for the train section the train is on			
			lcVector3 trainLoc = lcVector3(loc3SplineWorldInterpolation.x, loc3SplineWorldInterpolation.y, 76);
							
			mat44BogieWheel2 = toPrevSection->location;

			mat44BogieWheel2.SetTranslation(trainLoc); 
		
			lcMatrix44 matBogieRotate = lcMatrix44Identity();
			
			double rotationBogie = 90 + middleRotation;
		
			matBogieRotate = lcMatrix44RotationZ(LC_DTOR * rotationBogie);
		
			mat44BogieWheel2 = lcMul(matBogieRotate, mat44BogieWheel2);
		
			//matBogieRotate = lcMul(matBogieRotate,toPrevSection->location);

			/*double rotateBasePlate = lcGetAngle(loc3SplineWorldInterpolation.x, loc3SplineWorldInterpolation.y, 
													locBogieVec3.x, locBogieVec3.y);
		
			std::cout << "rotateBasePlate: " << rotateBasePlate << "\n";*/													

			// Set Baseplate location and rotation
			double basePlate_posX = (loc3SplineWorldInterpolation.x + locBogieVec3.x) / 2;
			double basePlate_posY = (loc3SplineWorldInterpolation.y + locBogieVec3.y) / 2;

			lcVector3 basePlateLoc = lcVector3(basePlate_posX, basePlate_posY, 84);
							
			mat44BasePlate = lcMatrix44Identity();
			mat44BasePlate.SetTranslation(basePlateLoc); 
		
			
			double rotateBasePlate = lcGetAngle(loc3SplineWorldInterpolation.x, loc3SplineWorldInterpolation.y, 
													locBogieVec3.x, locBogieVec3.y);
			
			std::cout << "rotateBasePlate: " << rotateBasePlate << "\n";

			lcMatrix44 mat44BasePlateRotate = lcMatrix44RotationZ(rotateBasePlate);

			mat44BasePlate = lcMul(mat44BasePlateRotate, mat44BasePlate);






		}
		else {
			std::cout << "Exaxct match: Spline item has exact length to first bogie wheels\n";
		}




	}


	std::cout << "END -- Find location of following bogie wheels\n";

}


lcMatrix44 TrainLocomotive::GetBoogieWorldPosition() {
		
	lcMatrix44 matBogieOffset = lcMatrix44Identity();

	//Get location for the train section the train is on
	lcVector3 trainLoc = lcVector3(0, 0, 76); 

	
	lcVector2 splineLoc = GetSplineLocation();

	trainLoc += lcVector3(splineLoc[0], splineLoc[1], 0);

	matBogieOffset.SetTranslation(trainLoc); 

	lcMatrix44 matBogieRotate = lcMatrix44Identity();
	
	double rotationBogie = 90 + GetSplineRotation();

	matBogieRotate = lcMatrix44RotationZ(LC_DTOR * rotationBogie);

	matBogieRotate = lcMul(matBogieRotate, matBogieOffset);

	matBogieRotate = lcMul(matBogieRotate,trainLocation.section->location);

	lcVector3 locVec3World = matBogieRotate.GetTranslation();

	return matBogieRotate;
	
}


void TrainLocomotive::GetPieces(std::vector<lcPiece*>& pieces) {
	
	//Calculate location
	lcPiecesLibrary* Library = lcGetPiecesLibrary();
	PieceInfo* pieceWheelBogie = Library->FindPiece("501c.dat", nullptr, false, false);
	
	lcPiece* piece = new lcPiece(pieceWheelBogie);

	lcMatrix44 matBogieRotate = GetBoogieWorldPosition();

	piece->Initialize(matBogieRotate, mCurrentStep);
	piece->SetColorIndex(1);

	pieces.push_back(piece);	

	//Following bogie wheels	
	piece = new lcPiece(pieceWheelBogie);

	piece->Initialize(mat44BogieWheel2, mCurrentStep);
	piece->SetColorIndex(1);

	pieces.push_back(piece);

	
	PieceInfo* pieceIndTrainBase = Library->FindPiece("92339.dat", nullptr, false, false);
	piece = new lcPiece(pieceIndTrainBase);

	piece->Initialize(mat44BasePlate, mCurrentStep);
	piece->SetColorIndex(1);

	pieces.push_back(piece);


}




const char* TrainTrackSystem::mTrackTypeFilenames[LC_TTS_NUMITEMS] =
{
	"74746.dat", 	// LC_TTS_STRAIGHT,
	"74747.dat", 	// LC_TTS_CURVED,
	"32087.dat", 	// LC_TTS_CROSS,
	"2861c04.dat", 	// LC_TTS_LEFT_BRANCH,
	"2859c04.dat" 	// LC_TTS_RIGHT_BRANCH	
};


TrainTrackSystem::TrainTrackSystem()
	: mModel(new lcModel(QString(), nullptr, false))
{
	lcPiecesLibrary* Library = lcGetPiecesLibrary();

	mLocomotive = new TrainLocomotive(this);

	float xHalfBendOff = sin(LC_DTOR * 11.25) * 800;
	float yHalfBendOff = ((cos(LC_DTOR * 11.25) * 800) - 800);

	std::cout << "xHalfBendOff: " << xHalfBendOff << ", yHalfBendOff: " << yHalfBendOff << "\n";

	float x_left_branch = 320 + 320 + 0 + (-(sin(LC_DTOR * 22.5)*800));
	float y_left_branch = 0 + 0 + 320 + ((cos(LC_DTOR * 22.5)*800)-800);

	float x_right_branch = 320 + 320 + 0 + (-(sin(LC_DTOR * 22.5)*800));
	float y_right_branch = 0 + 0 - 320 - ((cos(LC_DTOR * 22.5)*800)-800);

	TrainTrackType *trackType;

	trainTrackTypes.reserve(LC_TTS_NUMITEMS);

	// straight
	trackType = new TrainTrackType();
	trackType->trackTypeId = LC_TTS_STRAIGHT;
	trackType->Load(Library,mTrackTypeFilenames[LC_TTS_STRAIGHT]);
	trackType->AddConnection(new TrainTrackConnectionPoint(0,160,0));
	trackType->AddConnection(new TrainTrackConnectionPoint(180,-160,0));	
	trainTrackTypes[LC_TTS_STRAIGHT] = trackType;

	// curved
	trackType = new TrainTrackType();
	trackType->trackTypeId = LC_TTS_CURVED;
	trackType->Load(Library,mTrackTypeFilenames[LC_TTS_CURVED]);
	trackType->AddConnection(new TrainTrackConnectionPoint(-11.25,xHalfBendOff,yHalfBendOff));
	trackType->AddConnection(new TrainTrackConnectionPoint(-168.75,-xHalfBendOff,yHalfBendOff));
	trainTrackTypes[LC_TTS_CURVED] = trackType;

	// Crossing
	trackType = new TrainTrackType();
	trackType->trackTypeId = LC_TTS_CROSS;
	trackType->Load(Library,mTrackTypeFilenames[LC_TTS_CROSS]);
	trackType->AddConnection(new TrainTrackConnectionPoint(0,160,0));
	trackType->AddConnection(new TrainTrackConnectionPoint(90,0,160));
	trackType->AddConnection(new TrainTrackConnectionPoint(180,-160,0));
	trackType->AddConnection(new TrainTrackConnectionPoint(-90,0,-160));	
	trainTrackTypes[LC_TTS_CROSS] = trackType;

	// Left branch
	trackType = new TrainTrackType();
	trackType->trackTypeId = LC_TTS_LEFT_BRANCH;
	trackType->Load(Library,mTrackTypeFilenames[LC_TTS_LEFT_BRANCH]);
	trackType->AddConnection(new TrainTrackConnectionPoint(0,320,0));
	trackType->AddConnection(new TrainTrackConnectionPoint(22.50,x_left_branch,y_left_branch));	
	trackType->AddConnection(new TrainTrackConnectionPoint(180,-320,0));
	trainTrackTypes[LC_TTS_LEFT_BRANCH] = trackType;

	// Right branch
	trackType = new TrainTrackType();
	trackType->trackTypeId = LC_TTS_RIGHT_BRANCH;
	trackType->Load(Library,mTrackTypeFilenames[LC_TTS_RIGHT_BRANCH]);
	trackType->AddConnection(new TrainTrackConnectionPoint(0,320,0));
	trackType->AddConnection(new TrainTrackConnectionPoint(180,-320,0));
	trackType->AddConnection(new TrainTrackConnectionPoint(-22.50,x_right_branch,y_right_branch));	
	trainTrackTypes[LC_TTS_RIGHT_BRANCH] = trackType;
		
	// Create map for lookup track type by name
	for(int i = 0; i < LC_TTS_NUMITEMS; i++) trackTypesByName.insert({mTrackTypeFilenames[i], static_cast<LC_TTS_TYPES>(i)});

}


TrainTrackSystem::~TrainTrackSystem() {}

void TrainTrackSystem::setColorIndex(int mColorIndex) {
	this->mColorIndex = mColorIndex;
}

void TrainTrackSystem::setCurrentStep(int mCurrentStep) {
	this->mCurrentStep = mCurrentStep;
}

void TrainTrackSystem::addFirstTrackSection(lcMatrix44 position, enum LC_TTS_TYPES trackTypeId) {
	TrainTrackType* trainTrackType = trainTrackTypes[trackTypeId];
	
	TrainTrackSection *first_section = new TrainTrackSection(trainTrackType, position, mCurrentStep, mColorIndex);
	trackSections.push_back(first_section);

	addTrackSection(trackSections[0], 0, LC_TTS_STRAIGHT, 0);
	addTrackSection(trackSections[1], 1, LC_TTS_LEFT_BRANCH, 2);


	/*addTrackSection(trackSections[1], 1, LC_TTS_CURVED, 0);
	addTrackSection(trackSections[2], 1, LC_TTS_CURVED, 0);
	addTrackSection(trackSections[3], 1, LC_TTS_CURVED, 0);
	addTrackSection(trackSections[4], 1, LC_TTS_CURVED, 0);
	addTrackSection(trackSections[5], 1, LC_TTS_CURVED, 0);
	addTrackSection(trackSections[6], 1, LC_TTS_CURVED, 0);
	addTrackSection(trackSections[7], 1, LC_TTS_CURVED, 0);
	addTrackSection(trackSections[8], 1, LC_TTS_CURVED, 0);
	addTrackSection(trackSections[9], 1, LC_TTS_CURVED, 0);
	addTrackSection(trackSections[10], 1, LC_TTS_CURVED, 0);
	addTrackSection(trackSections[11], 1, LC_TTS_CURVED, 0);
	addTrackSection(trackSections[12], 1, LC_TTS_CURVED, 0);
	addTrackSection(trackSections[13], 1, LC_TTS_STRAIGHT, 0);
	addTrackSection(trackSections[14], 1, LC_TTS_STRAIGHT, 0);
	addTrackSection(trackSections[15], 1, LC_TTS_CROSS, 0);
	addTrackSection(trackSections[16], 2, LC_TTS_STRAIGHT, 0);
	addTrackSection(trackSections[17], 1, LC_TTS_STRAIGHT, 0);
	addTrackSection(trackSections[18], 1, LC_TTS_CURVED, 1);
	addTrackSection(trackSections[19], 0, LC_TTS_CURVED, 1);
	addTrackSection(trackSections[20], 0, LC_TTS_CURVED, 1);
	addTrackSection(trackSections[21], 0, LC_TTS_CURVED, 1);
	addTrackSection(trackSections[22], 0, LC_TTS_CURVED, 1);
	addTrackSection(trackSections[23], 0, LC_TTS_CURVED, 1);
	addTrackSection(trackSections[24], 0, LC_TTS_CURVED, 1);
	addTrackSection(trackSections[25], 0, LC_TTS_CURVED, 1);
	addTrackSection(trackSections[26], 0, LC_TTS_CURVED, 1);
	addTrackSection(trackSections[27], 0, LC_TTS_CURVED, 1);
	addTrackSection(trackSections[28], 0, LC_TTS_CURVED, 1);
	addTrackSection(trackSections[29], 0, LC_TTS_CURVED, 1);
	addTrackSection(trackSections[30], 0, LC_TTS_STRAIGHT, 0);
	addTrackSection(trackSections[31], 1, LC_TTS_STRAIGHT, 0);*/




	//addTrackSection(trackSections[1], 1, LC_TTS_CURVED, 0);

}

void TrainTrackSystem::addTrackSection(int fromTrackSectionIdx, int fromTrackSectionConnectionIdx, enum LC_TTS_TYPES toTrackType, int toTrackTypeConnectionIdx) 
{
	TrainTrackSection* fromTrackSection = trackSections[fromTrackSectionIdx];
	TrainTrackType* fromTrack = fromTrackSection->trackType;
	TrainTrackConnectionPoint * fromConnectIdx = fromTrack->GetConnection(fromTrackSectionConnectionIdx);
	TrainTrackType* toTrack = trainTrackTypes[toTrackType];
	TrainTrackConnectionPoint * toConnectIdx = toTrack->GetConnection(toTrackTypeConnectionIdx);

	lcMatrix44 currentLocation = fromTrackSection->location;
	lcMatrix44 mat44;
	lcMatrix44 matOffset;

	// calculate position on exiting connection point
	mat44 = lcMatrix44Identity();
	mat44 =	lcMatrix44RotationZ(LC_DTOR * fromConnectIdx->angle);

	matOffset = lcMatrix44Identity();
	matOffset.SetTranslation(lcVector3(fromConnectIdx->xy_coordinate.x, fromConnectIdx->xy_coordinate.y,0));

	matOffset = lcMul(mat44, matOffset);

	currentLocation = lcMul(matOffset,currentLocation);

	// calculate position on entering new track section
	mat44 = lcMatrix44Identity();
	mat44 =	lcMatrix44RotationZ(LC_DTOR * (180 - toConnectIdx->angle));

	matOffset = lcMatrix44Identity();
	matOffset.SetTranslation(lcVector3(-toConnectIdx->xy_coordinate.x, -toConnectIdx->xy_coordinate.y,0));

	matOffset = lcMul(matOffset,mat44);

	currentLocation = lcMul(matOffset,currentLocation);

	trackSections.push_back(new TrainTrackSection(toTrack, currentLocation, mCurrentStep, mColorIndex));
}


void TrainTrackSystem::addTrackSection(TrainTrackSection* fromTrackSection, int fromTrackSectionConnectionIdx, enum LC_TTS_TYPES toTrackType, int toTrackTypeConnectionIdx) 
{
	TrainTrackType* fromTrack = fromTrackSection->trackType;	
	TrainTrackConnectionPoint * fromConnectIdx = fromTrack->GetConnection(fromTrackSectionConnectionIdx);
	TrainTrackType* toTrack = trainTrackTypes[toTrackType];
	TrainTrackConnectionPoint * toConnectIdx = toTrack->GetConnection(toTrackTypeConnectionIdx);

	lcMatrix44 currentLocation = fromTrackSection->location;
	lcMatrix44 mat44;
	lcMatrix44 matOffset;

	// calculate position on exiting connection point
	mat44 = lcMatrix44Identity();
	mat44 =	lcMatrix44RotationZ(LC_DTOR * fromConnectIdx->angle);

	matOffset = lcMatrix44Identity();
	matOffset.SetTranslation(lcVector3(fromConnectIdx->xy_coordinate.x, fromConnectIdx->xy_coordinate.y,0)); 

	matOffset = lcMul(mat44, matOffset);

	currentLocation = lcMul(matOffset,currentLocation);

	// calculate position on entering new track section
	mat44 = lcMatrix44Identity();
	mat44 =	lcMatrix44RotationZ(LC_DTOR * (180 - toConnectIdx->angle));

	matOffset = lcMatrix44Identity();
	matOffset.SetTranslation(lcVector3(-toConnectIdx->xy_coordinate.x, -toConnectIdx->xy_coordinate.y,0)); 

	matOffset = lcMul(matOffset,mat44);

	currentLocation = lcMul(matOffset,currentLocation);	

	trackSections.push_back(new TrainTrackSection(toTrack, currentLocation, mCurrentStep, mColorIndex));				
}

TrainTrackSection* TrainTrackSystem::findSectionConnectedTo(TrainTrackSection* fromTrackSection, int fromTrackSectionConnectionIdx, int& toTrackSectionConnectionIdx) {

	//std::cout << std::setprecision(15);

	lcMatrix44 fromConnLocMaxtrix = fromTrackSection->GetConnectionLocation(fromTrackSectionConnectionIdx);
	
	lcVector3 fromConnLocVec3 = fromConnLocMaxtrix.GetTranslation();

	//std::cout << "fromConnLocVec3, x: " << fromConnLocVec3[0] << ", y: " << fromConnLocVec3[1] << "\n";
	
	for(int trackSectionIdx = 0; trackSectionIdx < trackSections.size(); trackSectionIdx++) {
		
		TrainTrackSection* toSection = trackSections[trackSectionIdx];
		
		if(toSection != fromTrackSection) {
	 
			for(int connectionIdx = 0; connectionIdx < toSection->GetNoOfConnections(); connectionIdx++) {

				//std::cout << "trackSectionIdx: " << trackSectionIdx << ", connectionIdx: " << connectionIdx << "\n";

				lcMatrix44 connLocMaxtrix = toSection->GetConnectionLocation(connectionIdx);
				
				lcVector3 connLocVec3 = connLocMaxtrix.GetTranslation();

				//std::cout << "connLocVec3, x: " << connLocVec3[0] << ", y: " << connLocVec3[1] << "\n";

				if(abs(fromConnLocVec3[0] - connLocVec3[0]) < precisionErrorNumber && 
					abs(fromConnLocVec3[1] - connLocVec3[1]) < precisionErrorNumber) {
					
					toTrackSectionConnectionIdx = connectionIdx;

					return toSection;

				}				
			}
		}
	}
	return nullptr;
}


bool TrainTrackSystem::deleteTrackSection(lcPiece* piece)
{	
	
	lcVector3 vec3PieceDel = piece->GetRotationCenter();

	for(unsigned long i = 0; i < trackSections.size(); i++) {
		
		lcVector3 vec3PieceCur = trackSections[i]->location.GetTranslation(); 

		if(abs(vec3PieceDel.x - vec3PieceCur.x) < precisionErrorNumber &&
			abs(vec3PieceDel.y - vec3PieceCur.y) < precisionErrorNumber &&
			abs(vec3PieceDel.z - vec3PieceCur.z) < precisionErrorNumber) 
		{
            trackSections.erase(trackSections.begin() + i);
			return true;
		}
	}	
	return false;
}

std::vector<PieceInfo *> TrainTrackSystem::getTrackTypes()
{
    std::vector<PieceInfo*> trackTypes;
	for(int i = 0; i < LC_TTS_NUMITEMS; i++) {
		trackTypes.push_back(trainTrackTypes[i]->pieceInfo);		
	}
	return trackTypes;	
} 

std::vector<TrainTrackSection*> TrainTrackSystem::GetTrackSections()
{
	return trackSections;
}

std::vector<TrainTrackSection*>* TrainTrackSystem::GetTrackSectionsPtr()
{
	return &trackSections;
}

std::vector<lcPiece*> TrainTrackSystem::GetPieces()
{
    std::vector<lcPiece*> pieces;
	PieceInfo* pieceInfo;

	lcPiecesLibrary* Library = lcGetPiecesLibrary();
	PieceInfo* pieceInfoSel = Library->FindPiece("3023.dat", nullptr, false, false);
	
	TrainTrackSection* trackSection;	
	lcPiece* piece;
		
	for(unsigned long i = 0; i < trackSections.size(); i++)
    {
		trackSection = trackSections[i];
		pieceInfo = trackSection->trackType->pieceInfo;
		piece = new lcPiece(pieceInfo);
		piece->Initialize(trackSection->location, trackSection->mCurrentStep);
		piece->SetColorIndex(trackSection->mColorIndex);
        pieces.push_back(piece);
		
		// Add 1 x 2 plate on each connecton point, for detect selection
		lcMatrix44 mat44;
		lcMatrix44 matOffset;

		for(int con_no = 0; con_no < trackSection->GetNoOfConnections(); con_no++) 
        {
			mat44 = trackSection->GetConnectionLocation(con_no);

			matOffset = lcMatrix44Identity();
			matOffset.SetTranslation(lcVector3(-10, 0, 8)); 

			mat44 = lcMul(matOffset,mat44);

			matOffset = lcMatrix44Identity();
			matOffset =	lcMatrix44RotationZ(LC_DTOR * 90);

			mat44 = lcMul(matOffset,mat44);

			piece = new lcPiece(pieceInfoSel);
			piece->Initialize(mat44, trackSection->mCurrentStep);
			piece->SetColorIndex(trackSection->mColorIndex);
			//pieces.Add(piece);
            pieces.push_back(piece);
		}
	}


	/* call locomotive to get pieces */
	mLocomotive->GetPieces(pieces);

	return pieces;
}	

lcModel* TrainTrackSystem::GetModel()
{
    std::vector<lcPiece*> trackSystem = this->GetPieces();
	mModel->SetTrainTrackSystem(trackSystem);
	return mModel.get();
}

void TrainTrackSystem::SetModel()
{
    std::vector<lcPiece*> trackSystem = this->GetPieces();
	mModel->SetTrainTrackSystem(trackSystem);	
}


LC_TTS_TYPES TrainTrackSystem::findTrackTypeByString(const char *tracktypeName) 
{
	std::string str_search = tracktypeName;
	std::map<std::string ,LC_TTS_TYPES>::iterator search = trackTypesByName.find(str_search);
    if (search != trackTypesByName.end()) {
		return search->second;
	}		
    return static_cast<LC_TTS_TYPES>(-1);
}
