#pragma once

#include "Vector2.h"

struct Unit {
public:
    Unit()
    {
        Position = Vector2();
        ViewDirection = Vector2();
        FOV = 0;
        ViewDistance = 0;
        ID = 0;
    }
    Unit(Vector2 &position, Vector2 &rotation, float &fov, int &view_distance, uint32_t &id){
        Position = position;
        ViewDirection = rotation;
        FOV = fov;
        ViewDistance = view_distance;
        ID = id;
        ViewDirection.normalize();
    }

    bool CanSee(Unit &other)
    {
        float Distance = Vector2::distance(Position, other.Position);
        if (Distance > ViewDistance)
            return false;
        Vector2 TargetDirection = other.Position - Position;
        TargetDirection.normalize();
        float Angle = RadToDeg(acos(Vector2::dot(ViewDirection, TargetDirection)));
        if (Angle > FOV/2)
            return false;
        return true;
    }

    Vector2 Position;
    Vector2 ViewDirection;
    float FOV;
    int ViewDistance;
    std::vector<uint32_t> VisibleUnitsIDs;
    uint32_t ID;
};