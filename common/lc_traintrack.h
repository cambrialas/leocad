#pragma once

#include "lc_math.h"

class lcPiece;
class lcPiecesLibrary;
class lcTrainTrackInfo;

struct lcTrainTrackConnection
{
	lcMatrix44 Transform;
};


class lcTrainTrackGroup 
{

public:

    lcTrainTrackGroup() {}
    int AddTrackToGroup(lcTrainTrackInfo* trackInfo) 
    {
        listTrainTrackInfos.push_back(trackInfo);
        return listTrainTrackInfos.size() - 1;
    };

    lcTrainTrackInfo* GetIndex(int index) 
    {
        return listTrainTrackInfos[index];
    }

    int Size() 
    {
        return listTrainTrackInfos.size();
    }

protected:

    std::vector<lcTrainTrackInfo*> listTrainTrackInfos;
};


class lcTrainTrackInfo
{
public:

    lcTrainTrackInfo(lcTrainTrackGroup* group, PieceInfo* pieceInfo) 
    {
        mGroup = group;
        mPieceInfo = pieceInfo;
        mGroupIndex = group->AddTrackToGroup(this);
    }

    std::pair<PieceInfo*, lcMatrix44> GetPieceInsertPosition(lcPiece* Piece, quint32 fromConnectionIdx, quint32 TrainTrackType, quint32 toConnectionIdx) const;

	void AddConnection(const lcTrainTrackConnection& TrainTrackConnection)
	{
		mConnections.emplace_back(TrainTrackConnection);
	}

	const std::vector<lcTrainTrackConnection>& GetConnections() const
	{
		return mConnections;
	}

    lcTrainTrackGroup* GetGroup() {
        return mGroup;
    }

    PieceInfo* GetPieceInfo() {
        return mPieceInfo;
    }

protected:
    lcTrainTrackGroup* mGroup = nullptr;
    int mGroupIndex = -1;
    PieceInfo* mPieceInfo = nullptr;
	std::vector<lcTrainTrackConnection> mConnections;
};

void lcTrainTrackInit(lcPiecesLibrary* Library);
