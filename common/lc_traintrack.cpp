#include "lc_global.h"
#include "lc_traintrack.h"
#include "lc_library.h"
#include "pieceinf.h"
#include "piece.h"
#include "lc_application.h"
#include <QJsonObject>
#include <iostream>
#include <iomanip>
#include <limits>

// todo:
// add cross 32087.dat and auto replace when going over a straight section
// draw icon on thumbnails
// detect existing connections
// new gizmo mesh
// move config to json
// add other track types

std::pair<PieceInfo*, lcMatrix44> lcTrainTrackInfo::GetPieceInsertPosition(lcPiece* Piece, quint32 fromConnectionIdx, quint32 TrainTrackType, quint32 toConnectionIdx) const
{


	if (fromConnectionIdx >= mConnections.size())
		return { nullptr, lcMatrix44Identity() };

	//PieceInfo* Info = lcGetPiecesLibrary()->FindPiece(TrainTrackPieceNames[static_cast<int>(TrainTrackType)], nullptr, false, false);
    PieceInfo* Info = Piece->mPieceInfo->GetTrainTrackInfo()->GetGroup()->GetIndex(TrainTrackType)->GetPieceInfo();

	if (!Info)
		return { nullptr, lcMatrix44Identity() };

	lcTrainTrackInfo* TrainTrackInfo = Info->GetTrainTrackInfo();

    toConnectionIdx = toConnectionIdx % TrainTrackInfo->mConnections.size();

	if (!TrainTrackInfo || TrainTrackInfo->mConnections.empty())
		return { nullptr, lcMatrix44Identity() };

	lcMatrix44 Transform;

    Transform = lcMatrix44AffineInverse(TrainTrackInfo->mConnections[toConnectionIdx].Transform);
    Transform = lcMul(Transform, lcMatrix44RotationZ(LC_PI));

	Transform = lcMul(Transform, mConnections[fromConnectionIdx].Transform);
	Transform = lcMul(Transform, Piece->mModelWorld);

	return { Info, Transform };
}


void lcTrainTrackInit(lcPiecesLibrary* Library)
{

    double myDouble = 123.45678901234578;

    // setting precision to max precision for double
    std::cout << fixed
         << std::setprecision(
                std::numeric_limits<double>::max_digits10)
         << myDouble << endl;


    QFile loadFile("train_track_editor.json");

    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open save file.");
        return;
    }  

    QByteArray trainTrackEditorConfig = loadFile.readAll();

    QJsonDocument loadDoc(QJsonDocument::fromJson(trainTrackEditorConfig));

    if (!loadDoc.object().contains("track_sections") || !loadDoc["track_sections"].isArray()) {
        std::cout << "Json missing track sections\n";
        return;
    }

    QJsonArray tracksectionsJson = loadDoc["track_sections"].toArray();

    std::cout << tracksectionsJson.size() << "\n";


    lcTrainTrackGroup* trainTrackGroup = new lcTrainTrackGroup();


    for (int sectionIdx = 0; sectionIdx < tracksectionsJson.size(); ++sectionIdx) {
        QJsonObject sectionObject = tracksectionsJson[sectionIdx].toObject();

        QString mFilename = sectionObject["filename"].toString();
        std::cout << mFilename.toStdString() << "\n";
        

        PieceInfo* Info = Library->FindPiece(mFilename.toStdString().c_str(), nullptr, false, false);

        if (Info)
        {
            lcTrainTrackInfo* TrainTrackInfo = new lcTrainTrackInfo(trainTrackGroup, Info);

            if (!sectionObject.contains("connections") || !sectionObject["connections"].isArray()) {
                std::cout << "Json track section missing sections\n";
                continue;
            }

            QJsonArray connectionsJson = sectionObject["connections"].toArray();

            std::cout << "No of connections: " << connectionsJson.size() << "\n";

            for (int connIdx = 0; connIdx < connectionsJson.size(); ++connIdx) {
                    QJsonObject connObject = connectionsJson[connIdx].toObject();
                    //QString sTrans = connObject["transform_x"].toString();
                    
                    double transX = connObject["transform_x"].toDouble();
                    double transY = connObject["transform_y"].toDouble();
                    double rotateZ = connObject["rotate_z"].toDouble();
                    
                    std::cout << "transX: " << transX << ", transY: " << transY << ", rotateZ: " << rotateZ << "\n";

                    TrainTrackInfo->AddConnection({lcMatrix44(lcMatrix33RotationZ(rotateZ * LC_DTOR), lcVector3(transX, transY, 0.0f))});                    
            }
            //TrainTrackInfo->SetGroup(trainTrackGroup);
            //TrainTrackInfo->SetPieceInfo(Info);
            //trainTrackGroup->AddTrackToGroup(TrainTrackInfo);

            Info->SetTrainTrackInfo(TrainTrackInfo);
            
        }    
    }

	/*Info = Library->FindPiece("74746.dat", nullptr, false, false);

	if (Info)
	{
		lcTrainTrackInfo* TrainTrackInfo = new lcTrainTrackInfo();

		TrainTrackInfo->AddConnection({lcMatrix44Translation(lcVector3(160.0f, 0.0f, 0.0f))});
		TrainTrackInfo->AddConnection({lcMatrix44(lcMatrix33RotationZ(LC_PI), lcVector3(-160.0f, 0.0f, 0.0f))});

		Info->SetTrainTrackInfo(TrainTrackInfo);
	}*/

	/*Info = Library->FindPiece("74747.dat", nullptr, false, false);

	if (Info)
	{
		lcTrainTrackInfo* TrainTrackInfo = new lcTrainTrackInfo();

		const float CurveX = sinf(LC_DTOR * 11.25f) * 800.0f;
		const float CurveY = (cosf(LC_DTOR * 11.25f) * 800.0f) - 800.0f;

        std::cout << "CurveX:" << CurveX << ", CurveY: " << CurveY << "\n";

		TrainTrackInfo->AddConnection({lcMatrix44(lcMatrix33RotationZ(-11.25f * LC_DTOR), lcVector3(CurveX, CurveY, 0.0f))});
		TrainTrackInfo->AddConnection({lcMatrix44(lcMatrix33RotationZ(-168.75f * LC_DTOR), lcVector3(-CurveX, CurveY, 0.0f))});

		Info->SetTrainTrackInfo(TrainTrackInfo);
	}*/

	//const float BranchX = 320.0f + 320.0f + (-(sinf(LC_DTOR * 22.5f) * 800.0f));
	//const float BranchY = 320.0f + ((cosf(LC_DTOR * 22.5f) * 800.0f) - 800.0f);

    /*std::cout << "BranchX: " << BranchX << ", BranchY: " << BranchY << "\n";

	Info = Library->FindPiece("2861c04.dat", nullptr, false, false);

	if (Info)
	{
		lcTrainTrackInfo* TrainTrackInfo = new lcTrainTrackInfo();

		TrainTrackInfo->AddConnection({lcMatrix44Translation(lcVector3(320.0f, 0.0f, 0.0f))});
		TrainTrackInfo->AddConnection({lcMatrix44(lcMatrix33RotationZ(22.5f * LC_DTOR), lcVector3(BranchX, BranchY, 0.0f))});	
		TrainTrackInfo->AddConnection({lcMatrix44(lcMatrix33RotationZ(LC_PI), lcVector3(-320.0f, 0.0f, 0.0f))});

		Info->SetTrainTrackInfo(TrainTrackInfo);
	}*/

	/*PieceInfo* Info = Library->FindPiece("2859c04.dat", nullptr, false, false);

	if (Info)
	{
		lcTrainTrackInfo* TrainTrackInfo = new lcTrainTrackInfo();

		TrainTrackInfo->AddConnection({lcMatrix44Translation(lcVector3(320.0f, 0.0f, 0.0f))});
		TrainTrackInfo->AddConnection({lcMatrix44(lcMatrix33RotationZ(-22.5f * LC_DTOR), lcVector3(BranchX, -BranchY, 0.0f))});	
		TrainTrackInfo->AddConnection({lcMatrix44(lcMatrix33RotationZ(LC_PI), lcVector3(-320.0f, 0.0f, 0.0f))});

		Info->SetTrainTrackInfo(TrainTrackInfo);
	}

    Info = Library->FindPiece("32087.dat", nullptr, false, false);

	if (Info)
	{
		lcTrainTrackInfo* TrainTrackInfo = new lcTrainTrackInfo();

		TrainTrackInfo->AddConnection({lcMatrix44(lcMatrix33RotationZ(0.0f * LC_DTOR), lcVector3(160.0f, 0.0f, 0.0f))});
		TrainTrackInfo->AddConnection({lcMatrix44(lcMatrix33RotationZ(90.0f * LC_DTOR), lcVector3(0.0f, 160.0f, 0.0f))});
        TrainTrackInfo->AddConnection({lcMatrix44(lcMatrix33RotationZ(180.0f * LC_DTOR), lcVector3(-160.0f, 0.0f, 0.0f))});
        TrainTrackInfo->AddConnection({lcMatrix44(lcMatrix33RotationZ(-90.0f * LC_DTOR), lcVector3(0.0f, -160.0f, 0.0f))});

		Info->SetTrainTrackInfo(TrainTrackInfo);
	}
	}*/
}
