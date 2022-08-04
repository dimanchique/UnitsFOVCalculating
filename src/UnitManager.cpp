#include "UnitManager.h"

uint32_t UnitManager::UnitsCount = 0;

void UnitManager::CreateUnits(int FieldSize, uint32_t NumOfUnits, float FOV, int ViewDistance) {
    int size = FieldSize/2;
    CreateClusters(size, ViewDistance);
    for (int i = 0; i < NumOfUnits; ++i) {
        Vector2 UnitPosition;
        GenerateNotOccupiedLocation(size, UnitPosition);
        Vector2 UnitRotation = Vector2::GetRandomRotatedVector();
        InitializeUnits(UnitPosition, UnitRotation, FOV, ViewDistance);
    }
}

bool UnitManager::FieldPointIsOccupied(Vector2 &Position) {
    return UnitsIDsByLocation.find((int)Position.X) != UnitsIDsByLocation.end()
           &&
           UnitsIDsByLocation.at((int)Position.X).find((int)Position.Y) != UnitsIDsByLocation.at((int)Position.X).end();
}

void UnitManager::GetClusterForPosition(Vector2 &Cluster, Vector2 &Position, int &ClusterSize) {
    int X_Cluster = Position.X > 0 ?
                    ClusterSize * ((int) Position.X / ClusterSize) :
                    ClusterSize * (((int) Position.X / ClusterSize) - 1);
    int Y_Cluster = Position.Y > 0 ?
                    ClusterSize * ((int) Position.Y / ClusterSize) :
                    ClusterSize * (((int) Position.Y / ClusterSize) - 1);
    Cluster = Vector2(X_Cluster, Y_Cluster);
}

void UnitManager::CreateClusters(uint32_t StartPosition, int ClusterSize) {
    for (int i = -StartPosition; i < (int) StartPosition; i += ClusterSize) {
        for (int j = -StartPosition; j < (int) StartPosition; j += ClusterSize) {
            UnitsIDsByCluster[i][j];
        }
    }
}

void UnitManager::GenerateNotOccupiedLocation(int Band, Vector2 &Position) {
    do {
        Position = Vector2(std::experimental::randint(-Band, Band - 1),
                           std::experimental::randint(-Band, Band - 1));
    } while (FieldPointIsOccupied(Position));
}

void UnitManager::InitializeUnits(Vector2 &position, Vector2 &rotation, float &fov, int &view_distance) {
    Unit unit(position, rotation, fov, view_distance, UnitsCount);
    UnitsByID[unit.ID] = unit;
    UnitsIDsByLocation[(int)unit.Position.X][(int)unit.Position.Y] = unit.ID;
    Vector2 UnitCluster;
    GetClusterForPosition(UnitCluster, unit.Position, unit.ViewDistance);
    UnitsIDsByCluster[(int)UnitCluster.X][(int)UnitCluster.Y].push_back(UnitsCount++);
}

void UnitManager::CalculateVisibleUnits() {
    for (auto &Pair : UnitsByID) {
        auto &RootUnit = Pair.second;
        auto VisibleClusters = RayTraceSector(RootUnit);
        for (auto &Cluster : VisibleClusters)
        {
            for (auto &TargetUnit : GetUnitsByIDs(GetUnitsInCluster(Cluster)))
            {
                if (RootUnit.ID != TargetUnit.ID)
                    if (RootUnit.CanSee(TargetUnit))
                        RootUnit.VisibleUnitsIDs.push_back(TargetUnit.ID);
            }
        }
    }
}

std::vector<Vector2> UnitManager::RayTraceSector(Unit &unit) {
    std::vector<Vector2> ClustersAroundUnit;
    float AngleStep = unit.FOV / 8;
    Vector2 TraceVector = unit.ViewDirection;
    TraceVector.rotate(-(AngleStep*4));
    Vector2 TracedCluster;
    for (int i = 0; i < 8; ++i) {
        TraceVector*=unit.ViewDistance;
        TraceVector.rotate(i * AngleStep);
        TraceVector+=unit.Position;
        GetClusterForPosition(TracedCluster, TraceVector, unit.ViewDistance);
        bool ClusterAlreadyTraced = false;
        for (auto &Cluster : ClustersAroundUnit)
        {
            if (Cluster == TracedCluster) {
                ClusterAlreadyTraced = true;
                break;
            }
        }
        if (!ClusterAlreadyTraced)
            ClustersAroundUnit.push_back(TracedCluster);
    }
    return ClustersAroundUnit;
}