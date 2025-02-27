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
	std::vector<struct Spline> vectorSpline;

	switch(this->trackTypeId) {
		
		case LC_TTS_STRAIGHT: {

			for(int itr = 0; itr <= 100; itr++) {

				struct Spline splineElement;					
				splineElement.location = lcVector2(-160.0 + ((320.0 / 100.0) * itr ), 0);
			
				splineElement.rotation = 0;
				vectorSpline.push_back(splineElement);

			}

			vectorSplines.push_back(vectorSpline);

			std::cout << "Spline\n";
			for(int itr = 0; itr <= 100; itr++) {

				std::cout << itr << ", " << vectorSpline[itr].location[0] << ", " << vectorSpline[itr].location[1] << "\n";

			}
			std::cout << "Spline end\n";

			// Direction 

			struct DirectionFromConnectionPoint directionConnPoint0;			
			directionConnPoint0.SplineNo = 0;
			directionConnPoint0.direction = 1;
			vectorConnectionpointDirection.push_back(directionConnPoint0);

			struct DirectionFromConnectionPoint directionConnPoint1;			
			directionConnPoint1.SplineNo = 0;
			directionConnPoint1.direction = 0;
			vectorConnectionpointDirection.push_back(directionConnPoint1);

			break;
		}

		case LC_TTS_CURVED: {

			for(int itr = 0; itr <= 100; itr++) {

				float degree = ((22.5f / 100.0f) * itr) - 11.25f;

				float xHalfBendOff = sin(LC_DTOR * degree) * 800.0f;
				float yHalfBendOff = ((cos(LC_DTOR * degree) * 800.0f) - 800.0f);
				
				
				struct Spline splineElement;
				splineElement.location = lcVector2(xHalfBendOff, yHalfBendOff);
			
				splineElement.rotation = -degree;
				vectorSpline.push_back(splineElement);
			}

			vectorSplines.push_back(vectorSpline);

			std::cout << "Spline\n";
			for(int itr = 0; itr <= 100; itr++) 
				std::cout << itr << ", " << vectorSpline[itr].location[0] << ", " << vectorSpline[itr].location[1] << "\n";
			std::cout << "Spline end\n";

			// Direction 

			struct DirectionFromConnectionPoint directionConnPoint0;			
			directionConnPoint0.SplineNo = 0;
			directionConnPoint0.direction = 1;
			vectorConnectionpointDirection.push_back(directionConnPoint0);

			struct DirectionFromConnectionPoint directionConnPoint1;			
			directionConnPoint1.SplineNo = 0;
			directionConnPoint1.direction = 0;
			vectorConnectionpointDirection.push_back(directionConnPoint1);
			
			break;


		}

		case LC_TTS_CROSS: {

			for(int itr = 0; itr <= 100; itr++) {

				struct Spline splineElement;					
				splineElement.location = lcVector2(-160.0 + ((320.0 / 100.0) * itr ), 0);
			
				splineElement.rotation = 0;
				vectorSpline.push_back(splineElement);
			}
			vectorSplines.push_back(vectorSpline);

			vectorSpline.clear();

			for(int itr = 0; itr <= 100; itr++) {

				struct Spline splineElement;					
				splineElement.location = lcVector2(0, -160.0 + ((320.0 / 100.0) * itr ));
			
				splineElement.rotation = 90;
				vectorSpline.push_back(splineElement);
			}
			vectorSplines.push_back(vectorSpline);

			std::cout << "Spline\n";
			for(int itr = 0; itr <= 100; itr++) {

				std::cout << itr << ", " << vectorSpline[itr].location[0] << ", " << vectorSpline[itr].location[1] << "\n";

			}
			std::cout << "Spline end\n";

			// Direction 

			struct DirectionFromConnectionPoint directionConnPoint0;			
			directionConnPoint0.SplineNo = 0;
			directionConnPoint0.direction = 1;
			vectorConnectionpointDirection.push_back(directionConnPoint0);

			struct DirectionFromConnectionPoint directionConnPoint1;
			directionConnPoint1.SplineNo = 1;
			directionConnPoint1.direction = 1;
			vectorConnectionpointDirection.push_back(directionConnPoint1);
						
			struct DirectionFromConnectionPoint directionConnPoint2;
			directionConnPoint2.SplineNo = 0;
			directionConnPoint2.direction = 0;
			vectorConnectionpointDirection.push_back(directionConnPoint2);

			struct DirectionFromConnectionPoint directionConnPoint3;
			directionConnPoint3.SplineNo = 1;
			directionConnPoint3.direction = 0;
			vectorConnectionpointDirection.push_back(directionConnPoint3);

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


int TrainTrackSection::FindConnectionNumberNoTranslate(lcVector3 v3searchLocation) {
	float precisionErrorNumber = 0.001;

	lcMatrix44 matOffset;

	for(int con_no = 0; con_no < trackType->GetNoOfConnections(); con_no++) {

		lcVector3 v3Selection = GetConnectionLocation(con_no).GetTranslation();
		
		std::cout << "v3Selection x: " << v3Selection.x << ", y: " << v3Selection.y << ", z: " << v3Selection.z << "\n";

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

	// Place 
	trainLocation.section = trackSystem->trackSections[0];
	trainLocation.pointNo = 20;
	trainLocation.splineAndDirection = trainLocation.section->trackType->vectorConnectionpointDirection[1];

	mCurrentStep = 1;

}


lcVector3 TrainLocomotive::findOtherEndOfCurrentSpline() {
	
	std::cout << "Find next part\n";

	std::vector<struct Spline> curVectorSpline = trainLocation.section->trackType->vectorSplines[trainLocation.splineAndDirection.SplineNo];

	bool ForOrBackward = trainLocation.splineAndDirection.direction;

	int SplineIdx;

	if(ForOrBackward == 0)  //Forward
		SplineIdx = curVectorSpline.size() - 1;
	else 
		SplineIdx = 0;


	lcVector2 splineLoc = curVectorSpline[SplineIdx].location;


	lcVector3 v3searchLocation = lcVector3(splineLoc[0], splineLoc[1], 0);
	lcMatrix44 matBogieOffset = lcMatrix44Identity();
	
	matBogieOffset.SetTranslation(v3searchLocation);

	lcMatrix44 matBogieLocation = lcMul(matBogieOffset,trainLocation.section->location);

	return matBogieLocation.GetTranslation();
}


void TrainLocomotive::IncrementTrainLocation() {

	TrainTrackType *trackType = trainLocation.section->trackType;

	//trainLocation.splineAndDirection.SplineNo;

	if(trainLocation.pointNo < (trackType->vectorSplines[trainLocation.splineAndDirection.SplineNo].size() - 1)) {
	
		trainLocation.pointNo++;

		std::vector<struct Spline> curVectorSpline = trainLocation.section->trackType->vectorSplines[trainLocation.splineAndDirection.SplineNo];

		bool ForOrBackward = trainLocation.splineAndDirection.direction;
	
		int SplineIdx;
	
		if(ForOrBackward == 0)  //Forward
			SplineIdx = trainLocation.pointNo;		
		else 
			SplineIdx = curVectorSpline.size() - 1 - trainLocation.pointNo;
	
		
		lcVector2 splineLoc = curVectorSpline[SplineIdx].location;
		
		std::cout << "pointNo: " << trainLocation.pointNo << "trainLocation.section->vectorSpline.size(): " << curVectorSpline.size() << "\n";
	}
	else {

		lcVector3 lcVec3BogieLocation = findOtherEndOfCurrentSpline();

		std::cout << "lcVec3BogieLocation x: " << lcVec3BogieLocation.x << ", y: " << lcVec3BogieLocation.y << ", z: " << lcVec3BogieLocation.z << "\n";
	
		int fromConnectionPoint = trainLocation.section->FindConnectionNumberNoTranslate(lcVec3BogieLocation);
		
		std::cout << "fromConnectionPoint: " << fromConnectionPoint << "\n";



		int toNewTrackConnectionIdx;		

		TrainTrackSection* toSection = mTrackSystem->findSectionConnectedTo(trainLocation.section, fromConnectionPoint, toNewTrackConnectionIdx);
		
		if(toSection != nullptr) {
			
			std::cout << "Got new section\n";
			std::cout << "toNewTrackConnectionIdx: " << toNewTrackConnectionIdx << "\n"; 


			struct DirectionFromConnectionPoint dir = toSection->trackType->vectorConnectionpointDirection[toNewTrackConnectionIdx];

			trainLocation.splineAndDirection = dir;

			std::cout << "DirectionFromConnectionPoint SplineNo: " << dir.SplineNo << "\n"; 
			std::cout << "DirectionFromConnectionPoint Forward/Backward: " << dir.direction << "\n"; 
			trainLocation.section = toSection;
			trainLocation.pointNo = 0;

		}
		else {
			std::cout << "Got *NO* new section\n";
		}
		
	}

	std::cout << "START -- Find location of following bogie wheels\n";

	lcVector3 lcVec3BogieLocation = findOtherEndOfCurrentSpline();

	std::cout << "lcVec3BogieLocation x: " << lcVec3BogieLocation.x << ", y: " << lcVec3BogieLocation.y << ", z: " << lcVec3BogieLocation.z << "\n";

	int fromConnectionPoint = trainLocation.section->FindConnectionNumberNoTranslate(lcVec3BogieLocation);

	std::cout << "fromConnectionPoint: " << fromConnectionPoint << "\n";
	
	struct DirectionFromConnectionPoint dir = trainLocation.section->trackType->vectorConnectionpointDirection[fromConnectionPoint];

//-->
	std::vector<struct Spline> curVectorSplineBW2 = trainLocation.section->trackType->vectorSplines[dir.SplineNo];

	bool ForOrBackward = dir.direction;

	int SplineIdx;

	if(ForOrBackward == 0)  //Forward
		SplineIdx = curVectorSplineBW2.size() - 1;
	else 
		SplineIdx = 0;

	
	lcVector2 splineLoc = curVectorSplineBW2[SplineIdx].location;

	lcVector3 v3searchLocation = lcVector3(splineLoc[0], splineLoc[1], 0);
	lcMatrix44 matBogieOffsetBW2 = lcMatrix44Identity();
	
	matBogieOffsetBW2.SetTranslation(v3searchLocation);

	lcMatrix44 matBogieLocationBW2 = lcMul(matBogieOffsetBW2,trainLocation.section->location);

	lcVector3 lcVec3BogieLocationBW2 =  matBogieLocationBW2.GetTranslation();

	std::cout << "lcVec3BogieLocation x: " << lcVec3BogieLocationBW2.x << ", y: " << lcVec3BogieLocationBW2.y << ", z: " << lcVec3BogieLocationBW2.z << "\n";

	int otherEndConnectionPoint = trainLocation.section->FindConnectionNumberNoTranslate(lcVec3BogieLocationBW2);
		
	std::cout << "otherEndConnectionPoint: " << otherEndConnectionPoint << "\n";

	int toNewTrackConnectionIdx2;		

	TrainTrackSection* toSection2 = mTrackSystem->findSectionConnectedTo(trainLocation.section, otherEndConnectionPoint, toNewTrackConnectionIdx2);
	
	if(toSection2 != nullptr) {
		
		std::cout << "Got new section\n";
		std::cout << "toNewTrackConnectionIdx: " << toNewTrackConnectionIdx2 << "\n"; 


		struct DirectionFromConnectionPoint dir2 = toSection2->trackType->vectorConnectionpointDirection[toNewTrackConnectionIdx2];

		std::cout << "DirectionFromConnectionPoint SplineNo: " << dir2.SplineNo << "\n"; 
		std::cout << "DirectionFromConnectionPoint Forward/Backward: " << dir2.direction << "\n"; 

		std::vector<struct Spline> curVectorSpline2 = toSection2->trackType->vectorSplines[dir2.SplineNo];

		lcVector2 splineLoc2 = curVectorSpline2[0].location;

		lcVector3 trainLoc2 = lcVector3(splineLoc2[0], splineLoc2[1], 0);

		// Bogie wheel location on train
	
		std::cout << "TrainLocation - WheelBogie 2: " << trainLoc2[0] << ", " << trainLoc2[1] << ", " << trainLoc2[2] << "\n";
	
		lcMatrix44 matBogieOffset2 = lcMatrix44Identity();
		
		matBogieOffset2.SetTranslation(trainLoc2); 
	
		lcMatrix44 matBogieWorld = lcMul(matBogieOffset2,toSection2->location);
	
		lcVector3 lcVec3BogieLocationWorld =  matBogieWorld.GetTranslation();

		std::cout << "lcVec3BogieLocationWorld x: " << lcVec3BogieLocationWorld.x << ", y: " << lcVec3BogieLocationWorld.y << ", z: " << lcVec3BogieLocationWorld.z << "\n";


	}
	else {
		std::cout << "Got *NO* new section\n";
	}


//-->





	

	std::cout << "END -- Find location of following bogie wheels\n";






}

void TrainLocomotive::GetPieces(std::vector<lcPiece*>& pieces) {
	
	//Calculate location
	lcPiecesLibrary* Library = lcGetPiecesLibrary();
	PieceInfo* pieceWheelBogie = Library->FindPiece("501c.dat", nullptr, false, false);
	
	lcPiece* piece = new lcPiece(pieceWheelBogie);

	lcMatrix44 matBogieOffset = lcMatrix44Identity();

	//Get location for the train section the train is on
	lcVector3 trainLoc; // = trainLocation.section->location.GetTranslation();

	//Get location for the spline with index

	std::vector<struct Spline> curVectorSpline = trainLocation.section->trackType->vectorSplines[trainLocation.splineAndDirection.SplineNo];

	bool ForOrBackward = trainLocation.splineAndDirection.direction;

	int SplineIdx;

	if(ForOrBackward == 0)  //Forward
		SplineIdx = trainLocation.pointNo;		
	else 
		SplineIdx = curVectorSpline.size() - 1 - trainLocation.pointNo;

	
	lcVector2 splineLoc = curVectorSpline[SplineIdx].location;


	trainLoc += lcVector3(splineLoc[0], splineLoc[1], 0);

	// Bogie wheel location on train
	trainLoc += lcVector3(0, 0, 76); 

	std::cout << "TrainLocation - WheelBogie 1: " << trainLoc[0] << ", " << trainLoc[1] << ", " << trainLoc[2] << "\n";

	matBogieOffset.SetTranslation(trainLoc); 

	lcMatrix44 matBogieRotate = lcMatrix44Identity();
	
	double rotationBogie = 90 + curVectorSpline[SplineIdx].rotation;
	if(ForOrBackward == true) {
		rotationBogie += 180;
	}
	matBogieRotate = lcMatrix44RotationZ(LC_DTOR * rotationBogie);

	matBogieRotate = lcMul(matBogieRotate, matBogieOffset);

	matBogieRotate = lcMul(matBogieRotate,trainLocation.section->location);

	piece->Initialize(matBogieRotate, mCurrentStep);
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

	mLocomotive = new TrainLocomotive(this);

	addTrackSection(trackSections[0], 0, LC_TTS_STRAIGHT, 0);
	addTrackSection(trackSections[1], 1, LC_TTS_CURVED, 0);
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
	addTrackSection(trackSections[31], 1, LC_TTS_STRAIGHT, 0);




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

	std::cout << std::setprecision(15);

	lcMatrix44 fromConnLocMaxtrix = fromTrackSection->GetConnectionLocation(fromTrackSectionConnectionIdx);
	
	lcVector3 fromConnLocVec3 = fromConnLocMaxtrix.GetTranslation();

	std::cout << "fromConnLocVec3, x: " << fromConnLocVec3[0] << ", y: " << fromConnLocVec3[1] << "\n";

	
	for(int trackSectionIdx = 0; trackSectionIdx < trackSections.size(); trackSectionIdx++) {
		
		TrainTrackSection* toSection = trackSections[trackSectionIdx];
		
		if(toSection != fromTrackSection) {
	 
			for(int connectionIdx = 0; connectionIdx < toSection->GetNoOfConnections(); connectionIdx++) {

				std::cout << "trackSectionIdx: " << trackSectionIdx << ", connectionIdx: " << connectionIdx << "\n";

				lcMatrix44 connLocMaxtrix = toSection->GetConnectionLocation(connectionIdx);
				
				lcVector3 connLocVec3 = connLocMaxtrix.GetTranslation();

				std::cout << "connLocVec3, x: " << connLocVec3[0] << ", y: " << connLocVec3[1] << "\n";

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
