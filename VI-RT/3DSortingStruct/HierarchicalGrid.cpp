#include "HierarchicalGrid.hpp"
#include "scene.hpp"

void GridCell::calculateSizes()
{
    cellSizeX = (boundingBox.max.X - boundingBox.min.X) / 3;
    cellSizeY = (boundingBox.max.Y - boundingBox.min.Y) / 3;
    cellSizeZ = (boundingBox.max.Z - boundingBox.min.Z) / 3;
}

void HierarchicalGrid::build(Scene *scene)
{
    this->scene = scene;

    std::vector<Primitive *> primitives = scene->getPrims();
    std::vector<Primitive *> triangles;

    // build vector with every triangle in the mesh and their respective material_ndx

    for (const auto &prim : primitives)
    {
        if (dynamic_cast<Mesh *>(prim->g))
        {
            Mesh *mesh = (Mesh *)prim->g;

            for (const auto &face : mesh->faces)
            {
                // fetch triangle vertices
                Point v1 = mesh->vertices[face.vert_ndx[0]];
                Point v2 = mesh->vertices[face.vert_ndx[1]];
                Point v3 = mesh->vertices[face.vert_ndx[2]];

                // create triangle
                Triangle *tri = new Triangle(v1, v2, v3, face.geoNormal);

                // create primitive with the triangle as its geometry
                Primitive *primitive = new Primitive();
                primitive->g = tri;
                primitive->material_ndx = prim->material_ndx;

                triangles.push_back(primitive);
            }
        }
    }

    rootCell = new GridCell(0);

    // Determine the bounding box of the entire scene
    for (const auto &prim : primitives)
    {
        rootCell->boundingBox.update(prim->g->bb.min);
        rootCell->boundingBox.update(prim->g->bb.max);
    }

    // Compute the size of each cell
    rootCell->calculateSizes();

    // Build the root grid
    buildSubgrid(rootCell, triangles, 0);
}

int max(int a, int b)
{
    return (a > b) ? a : b;
}

int min(int a, int b)
{
    return (a < b) ? a : b;
}

float interpolate(float min, float max, float t)
{
    return t * max + (1 - t) * min;
}

void HierarchicalGrid::buildSubgrid(GridCell *cell, const std::vector<Primitive *> &primitives, int level)
{

    if (level >= maxDepth)
    { // Limit the depth of the hierarchy
        cell->primitives = primitives;
        return;
    }
    
    Point min = cell->boundingBox.min;
    Point max = cell->boundingBox.max;
    // printf("\nCell\nMin: %f %f %f; Max: %f %f %f\n", min.X, min.Y, min.Z, max.X, max.Y, max.Z);


    for (int x = 0; x < 3; x++)
    {
        for (int y = 0; y < 3; y++)
        {
            for (int z = 0; z < 3; z++)
            {
                GridCell *sub = new GridCell(level + 1);
                float limX = x/3.0f, limY = y/3.0f, limZ = z/3.0f;

                sub->boundingBox.min.X = interpolate(min.X, max.X, limX);
                sub->boundingBox.max.X = interpolate(min.X, max.X, limX + (1/3.0f));
                sub->boundingBox.min.Y = interpolate(min.Y, max.Y, limY);
                sub->boundingBox.max.Y = interpolate(min.Y, max.Y, limY + (1/3.0f));
                sub->boundingBox.min.Z = interpolate(min.Z, max.Z, limZ);
                sub->boundingBox.max.Z = interpolate(min.Z, max.Z, limZ + (1/3.0f));

                sub->calculateSizes();
                // printf("lims %f %f %f\n", limX, limY, limZ);
                cell->subgrid[x][y][z] = sub;
            }
        }
    }

    std::vector<Primitive *> subgridPrimitives[3][3][3];
    for (auto &prim : primitives)
    {

        for (int x = 0; x < 3; ++x)
        {
            for (int y = 0; y < 3; ++y)
            {
                for (int z = 0; z < 3; ++z)
                {

                    Triangle *geometry = (Triangle *)prim->g;

                    GridCell *sub = cell->subgrid[x][y][z];

                    BB cellBB = sub->boundingBox;

                    if (geometry->intersects(cellBB))
                        subgridPrimitives[x][y][z].push_back(prim);
                }
            }
        }
    }

    for (int x = 0; x < 3; ++x)
    {
        for (int y = 0; y < 3; ++y)
        {
            for (int z = 0; z < 3; ++z)
            {
                GridCell *sub = cell->subgrid[x][y][z];
                if (!subgridPrimitives[x][y][z].empty())
                {
                    buildSubgrid(sub, subgridPrimitives[x][y][z], level + 1);
                }
            }
        }
    }
}

int clamp(int val, int min, int max)
{
    return (val < min) ? min : ((val > max) ? max : val);
}

bool HierarchicalGrid::trace(Ray ray, Intersection *isect)
{
    // Compute the starting cell
    if (!rootCell->boundingBox.intersect(ray))
    {
        return false;
    }

    return intersectSubgrid(rootCell, ray, isect);
}

bool HierarchicalGrid::intersectSubgrid(GridCell *cell, Ray &ray, Intersection *isect)
{
    if (!cell)
        return false;

    // Check for primitive intersections at the current level
    Intersection curr_isect;
    bool hit = false;
    for (auto &prim : cell->primitives)
    {
        if (prim->g->intersect(ray, &curr_isect))
        {
            // printf("HIT TRIANGLE\n");
            if (!hit)
            {
                hit = true;
                *isect = curr_isect;
                isect->f = this->scene->getMaterial(prim->material_ndx);
            }
            else if (isect->depth > curr_isect.depth)
            {
                *isect = curr_isect;
                isect->f = this->scene->getMaterial(prim->material_ndx);
            }
        }
    }

    if (cell->depth >= maxDepth)
        return hit;



    // Compute the size of the subcells
    Point cellMin = cell->boundingBox.min;
    Point cellMax = cell->boundingBox.max;
    Vector cellSizeV = cellMax - cellMin;
    Point cellSize(cellSizeV.X, cellSizeV.Y, cellSizeV.Z);
    Point subcellSize = cellSize / 3.0f;

    // Compute the entry and exit points for the ray in the grid
    float tmin, tmax;
    if (!cell->boundingBox.intersect(ray, &tmin, &tmax))
        return hit;

    // printf("%f %f\n", tmin, tmax);

    Point entry = ray.o + tmin * ray.dir;
    Point exit = ray.o + tmax * ray.dir; 

    // printf("\nCell\nEntry: %f %f %f; Exit: %f %f %f\n", entry.X, entry.Y, entry.Z, exit.X, exit.Y, exit.Z);

    // printf("%f %f %f\n", subcellSize.X, (entry.X - cellMin.X) / subcellSize.X, (exit.X - cellMin.X) / subcellSize.X);

    // Compute the starting and ending subcell indices
    int startX = std::max(0, std::min(2, static_cast<int>((entry.X - cellMin.X) / subcellSize.X)));
    int startY = std::max(0, std::min(2, static_cast<int>((entry.Y - cellMin.Y) / subcellSize.Y)));
    int startZ = std::max(0, std::min(2, static_cast<int>((entry.Z - cellMin.Z) / subcellSize.Z)));
    int endX = std::max(0, std::min(2, static_cast<int>((exit.X - cellMin.X) / subcellSize.X)));
    int endY = std::max(0, std::min(2, static_cast<int>((exit.Y - cellMin.Y) / subcellSize.Y)));
    int endZ = std::max(0, std::min(2, static_cast<int>((exit.Z - cellMin.Z) / subcellSize.Z)));

    int temp;
    if (startX > endX) {
        temp = endX;
        endX = startX;
        startX = temp;
    }

    if (startY > endY) {
        temp = endY;
        endY = startY;
        startY = temp;
    }

    if (startZ > endZ) {
        temp = endZ;
        endZ = startX;
        startZ = temp;
    }

    // Traverse the relevant subcells
    for (int x = startX; x <= endX; ++x)
    {
        for (int y = startY; y <= endY; ++y)
        {
            for (int z = startZ; z <= endZ; ++z)
            {
                if (intersectSubgrid(cell->subgrid[x][y][z], ray, &curr_isect))
                {
                    if (!hit)
                    {
                        hit = true;
                        *isect = curr_isect;
                    }
                    else if (isect->depth > curr_isect.depth)
                    {
                        *isect = curr_isect;
                    }
                }
            }
        }
    }
    // Compute the next cell in the subgrid

    // for (int x = 0; x < 3; x++)
    // {
    //     for (int y = 0; y < 3; y++)
    //     {
    //         for (int z = 0; z < 3; z++)
    //         {
    //             if (intersectSubgrid(cell->subgrid[x][y][z], ray, &curr_isect))
    //             {
    //                 if (!hit)
    //                 {
    //                     hit = true;
    //                     *isect = curr_isect;
    //                 }
    //                 else if (isect->depth > curr_isect.depth)
    //                 {
    //                     *isect = curr_isect;
    //                 }
    //             }
    //         }
    //     }
    // }

    return hit;
}
