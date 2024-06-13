//
//  AABB.hpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 30/01/2023.
//

#ifndef BB_hpp
#define BB_hpp

#include "vector.hpp"
#include "ray.hpp"
#include <algorithm>

typedef struct BB
{
public:
    Point min, max;
    void update(Point p)
    {
        if (p.X < min.X)
            min.X = p.X;
        else if (p.X > max.X)
            max.X = p.X;
        if (p.Y < min.Y)
            min.Y = p.Y;
        else if (p.Y > max.Y)
            max.Y = p.Y;
        if (p.Z < min.Z)
            min.Z = p.Z;
        else if (p.Z > max.Z)
            max.Z = p.Z;
    }

    void update(const BB &other)
    {
        if (other.min.X < min.X)
            min.X = other.min.X;
        if (other.max.X > max.X)
            max.X = other.max.X;

        if (other.min.Y < min.Y)
            min.Y = other.min.Y;
        if (other.max.Y > max.Y)
            max.Y = other.max.Y;

        if (other.min.Z < min.Z)
            min.Z = other.min.Z;
        if (other.max.Z > max.Z)
            max.Z = other.max.Z;
    }

    Point center() const
    {
        return (min + max) * 0.5f;
    }

    bool intersect(Ray r)
    {
        float temp;
        float tmin = (min.X - r.o.X) / r.dir.X;
        float tmax = (max.X - r.o.X) / r.dir.X;

        if (tmin > tmax)
        {
            temp = tmin;
            tmin = tmax;
            tmax = temp;
        }

        float tymin = (min.Y - r.o.Y) / r.dir.Y;
        float tymax = (max.Y - r.o.Y) / r.dir.Y;

        if (tymin > tymax)
        {
            temp = tymin;
            tymin = tymax;
            tymax = temp;
        }

        if ((tmin > tymax) || (tymin > tmax))
            return false;

        if (tymin > tmin)
            tmin = tymin;
        if (tymax < tmax)
            tmax = tymax;

        float tzmin = (min.Z - r.o.Z) / r.dir.Z;
        float tzmax = (max.Z - r.o.Z) / r.dir.Z;

        if (tzmin > tzmax)
        {
            temp = tzmin;
            tzmin = tzmax;
            tzmax = temp;
        }

        if ((tmin > tzmax) || (tzmin > tmax))
            return false;

        if (tzmin > tmin)
            tmin = tzmin;
        if (tzmax < tmax)
            tmax = tzmax;

        return true;
    }

    bool intersect(const Ray &ray, float *tmin, float *tmax) const
    {
        float temp;
        float tmin_local = (min.X - ray.o.X) / ray.dir.X;
        float tmax_local = (max.X - ray.o.X) / ray.dir.X;

        if (tmin_local > tmax_local)
        {
            temp = tmin_local;
            tmin_local = tmax_local;
            tmax_local = temp;
        }

        float tymin = (min.Y - ray.o.Y) / ray.dir.Y;
        float tymax = (max.Y - ray.o.Y) / ray.dir.Y;

        if (tymin > tymax)
        {
            temp = tymin;
            tymin = tymax;
            tymax = temp;
        }

        if ((tmin_local > tymax) || (tymin > tmax_local))
            return false;

        if (tymin > tmin_local)
            tmin_local = tymin;
        if (tymax < tmax_local)
            tmax_local = tymax;

        float tzmin = (min.Z - ray.o.Z) / ray.dir.Z;
        float tzmax = (max.Z - ray.o.Z) / ray.dir.Z;

        if (tzmin > tzmax)
        {
            temp = tzmin;
            tzmin = tzmax;
            tzmax = temp;
        }

        if ((tmin_local > tzmax) || (tzmin > tmax_local))
            return false;

        if (tzmin > tmin_local)
            tmin_local = tzmin;
        if (tzmax < tmax_local)
            tmax_local = tzmax;

        *tmin = tmin_local;
        *tmax = tmax_local;

        return true;
    }

    bool
    isInside(Point p1)
    {
        return p1.X >= min.X - 0.0001 && p1.X <= max.X + 0.0001 &&
               p1.Y >= min.Y - 0.0001 && p1.Y <= max.Y + 0.0001 &&
               p1.Z >= min.Z - 0.0001 && p1.Z <= max.Z + 0.0001;
    }
} BB;

#endif /* AABB_hpp */
