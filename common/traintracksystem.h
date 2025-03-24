#pragma once

#include "lc_library.h"
#include "piece.h"
#include <vector>
#include <map>
#include <string>
#include <memory>

class TrainTrackSystem;

enum LC_TTS_TYPES {
    LC_TTS_STRAIGHT,
    LC_TTS_CURVED,
    LC_TTS_CROSS,
    LC_TTS_LEFT_BRANCH,
    LC_TTS_RIGHT_BRANCH,
    LC_TTS_NUMITEMS
};

enum class Direction {
    Forward,
    Backward
};

class TrainTrackConnectionPoint {
    public:
        TrainTrackConnectionPoint(float angle, float x, float y);
        float angle;
        lcVector2 xy_coordinate;
};


struct SplineElement {
    lcVector2 location;
    double rotation;
};


class Spline {
    public:
        Spline(std::vector<struct SplineElement>* spline, int fromConnectionNo, int toConnectionNo, Direction splineDirection) {
            mSpline = spline;
            mFromConnectionPointNo = fromConnectionNo;
            mToConnectionPointNo = toConnectionNo;
            mSplineDirection = splineDirection;
        }

        lcVector2 GetLocation(int splineIdx) {

            if(mSplineDirection == Direction::Backward)  //Forward
                splineIdx = mSpline->size() - 1 - splineIdx;
                
            return mSpline->at(splineIdx).location;
        
        };

        double GetRotation(int splineIdx) {
            
            if(mSplineDirection == Direction::Backward) {
                splineIdx = mSpline->size() - 1 - splineIdx;
                return mSpline->at(splineIdx).rotation + 180;
            }
            else {                    
                return mSpline->at(splineIdx).rotation;
            }        
        };



        Direction mSplineDirection; // 0 - Spline forward, 1 - Spline backwards

        int mFromConnectionPointNo;
        int mToConnectionPointNo;
        std::vector<struct SplineElement>* mSpline;
};


class TrainTrackType {
    public:
        
        void Load(lcPiecesLibrary* Library, char const* filename);
        void AddConnection(TrainTrackConnectionPoint *connectionPoint);        
        TrainTrackConnectionPoint *GetConnection(int index);

        std::vector<TrainTrackConnectionPoint *>& GetConnections();
        PieceInfo* pieceInfo;
        int GetNoOfConnections();

        void BezierLine(lcVector2* pointList, int NumberOfPoints, std::vector<struct SplineElement>* spline, int NumberOfResultPoints);
        void GenerateSplineAndDirection();

        std::vector<Spline*> vectorSplines; // vector of splines

        enum LC_TTS_TYPES trackTypeId;

    private:
        char const* filename;
        //lcArray<TrainTrackConnectionPoint *> connectionPoints;
        std::vector<TrainTrackConnectionPoint *> connectionPoints;
        
};

class TrainTrackSection {
    public:
        TrainTrackSection(TrainTrackType *trackType, lcMatrix44 location, int mCurrentStep, int mColorIndex);
        lcMatrix44 GetConnectionLocation(int connectionNo);

        lcMatrix44 GetSplineLocation(int splineNo, int splineIdx); 
        
        int GetNoOfConnections();
        int FindConnectionNumber(lcVector3 searchLocation);

        TrainTrackType *trackType;
        
        lcMatrix44 location;
        int mCurrentStep;
        int mColorIndex;
        
};


class TrainLocomotive
{
    public:

        

        TrainLocomotive(TrainTrackSystem* trackSystem);

        TrainTrackSystem* mTrackSystem;
    
        void CreateLocomotive();
        void IncrementTrainLocation();

        lcMatrix44 GetBoogieWorldPosition();

        void GetPieces(std::vector<lcPiece*>& pieces);

        lcVector2 GetSplineLocation() {

            return trainLocation.spline->GetLocation(trainLocation.splineIdx);

        };

        double GetSplineRotation() {

            return trainLocation.spline->GetRotation(trainLocation.splineIdx);

        };

        struct TrainPieces {
            lcPiece *pieceWheelBogie[2];
            lcPiece *pieceTrainBase;
        } trainPieces;

        struct TrainLocation {
            TrainTrackSection *section;	
            Spline* spline;	
            int splineIdx;
        } trainLocation;

        lcMatrix44 mat44BogieWheel2;
        lcMatrix44 mat44BasePlate;
            
        int mCurrentStep;




};


class TrainTrackSystem
{
    public:
        TrainTrackSystem();
        ~TrainTrackSystem();
	    void setColorIndex(int colorIndex);
	    void setCurrentStep(int mCurrentStep);

        void addFirstTrackSection(lcMatrix44 position, enum LC_TTS_TYPES trackTypeId);
        void addTrackSection(int fromTrackSectionIdx, int fromTrackSectionConnectionIdx, enum LC_TTS_TYPES toTrackType, int toTrackTypeConnectionIdx);
        void addTrackSection(TrainTrackSection* fromTrackSection, int fromTrackSectionConnectionIdx, enum LC_TTS_TYPES toTrackType, int toTrackTypeConnectionIdx);
        bool deleteTrackSection(lcPiece* piece);

        TrainTrackSection* findSectionConnectedTo(TrainTrackSection* fromTrackSection, int fromTrackSectionConnectionIdx, int& toTrackSectionConnectionIdx);

        //lcArray<PieceInfo*> getTrackTypes();
        std::vector<PieceInfo *> getTrackTypes();

        //lcArray<TrainTrackSection*> GetTrackSections();
        std::vector<TrainTrackSection *> GetTrackSections();
        //lcArray<TrainTrackSection*>* GetTrackSectionsPtr();
        std::vector<TrainTrackSection *>* GetTrackSectionsPtr();

        //lcArray<lcPiece* > GetPieces();
        std::vector<lcPiece *> GetPieces();

        lcModel* GetModel();
        void SetModel();
        LC_TTS_TYPES findTrackTypeByString(const char *tracktypeName); 

    //private:

        //lcArray<TrainTrackType*> trainTrackTypes;
        std::vector<TrainTrackType *> trainTrackTypes;

        static const char* mTrackTypeFilenames[LC_TTS_NUMITEMS];
        //lcArray<TrainTrackSection*> trackSections; 
        std::vector<TrainTrackSection *> trackSections;

        int mColorIndex;
        int mCurrentStep;

        std::unique_ptr<lcModel> mModel;

        std::map<std::string ,LC_TTS_TYPES> trackTypesByName;


        TrainLocomotive* mLocomotive;


};



