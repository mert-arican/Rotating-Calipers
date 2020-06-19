//
//  rotatingCalipers.h
//  Rotating Calipers
//
//  Created by Mert Arıcan on 12.06.2020.
//  Copyright © 2020 Mert Arıcan. All rights reserved.
//

#ifndef rotatingCalipers_h
#define rotatingCalipers_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

#define PI 3.14159265
float const D = 180.0 / PI;

// Create 'Point' struct to represent points of polygon
typedef struct point {
    int ID;
    char label;
    float x;
    float y;
} Point;

// Create 'Caliper' struct to represent calipers
typedef struct caliper {
    int initialPointID;
    int currentPointID;
    float degree;
    float diffDegree;
    bool completedPath;
} Caliper;

void createNSidedPolygon(Point *polygon, int n);

void createSpecialCase(struct point *polygon) {
    // This function creates special polygon for describing how the algorithm works.
    struct point A;
    A.label = 'A';
    A.x = 5;
    A.y = 7;
    A.ID = 0;
    polygon[0] = A;

    struct point B;
    B.label = 'B';
    B.x = 3;
    B.y = 6;
    B.ID = 1;
    polygon[1] = B;

    struct point C;
    C.label = 'C';
    C.x = 2;
    C.y = 5;
    C.ID = 2;
    polygon[2] = C;

    struct point D;
    D.label = 'D';
    D.x = 3;
    D.y = 2;
    D.ID = 3;
    polygon[3] = D;

    struct point E;
    E.label = 'E';
    E.x = 7;
    E.y = 1;
    E.ID = 4;
    polygon[4] = E;

    struct point F;
    F.label = 'F';
    F.x = 8;
    F.y = 2;
    F.ID = 5;
    polygon[5] = F;

    struct point G;
    G.label = 'G';
    G.x = 9;
    G.y = 5;
    G.ID = 6;
    polygon[6] = G;
}

#endif /* rotatingCalipers_h */
