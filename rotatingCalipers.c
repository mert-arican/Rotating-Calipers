//
//  rotatingCalipers.c
//  Rotating Calipers
//
//  Created by Mert Arıcan on 12.06.2020.
//  Copyright © 2020 Mert Arıcan. All rights reserved.
//

#include "rotatingCalipers.h"

// Stores the number of edges that the polygon has.
int N = 1;

// Stores the points with the highest and lowest y values.
int minID = 0;
int maxID = 0;

// Polygon is an array of 'Point's
// These points should be counter-clockwise order and in standart form.
Point *polygon;

// For convenience.
bool toDisplay = true;

// For determining diameter.
double maxDist = 0.0;
char bestPair[5];

// MARK: Calculating the angle of a line given two points

float getArctan(Point p1, Point p2) {
    // Given two points calculate the POSITIVE arctan(y/x)
    // Later we will use this value together with quadrant of the line
    // to calculate exact angle.
    if (p1.x == p2.x || p1.y == p2.y) { return 0.0; }
    float y = fabs(p2.y - p1.y);
    float x = fabs(p2.x - p1.x);
    float tan = y / x;
    float arctan = atan(tan) * D;
    return arctan;
}

float getDegree(Point p1, Point p2) {
    // Calculates and returns the angle of line that passes through given points 'p1' and 'p2'.
    // First function will calculate the arctan value, then depending on the quadrant of the line
    // it will calculate and return the exact angle.
    
    // Get arctan value.
    float alpha = getArctan(p1, p2);
    
    // Calculate and return exact angle.
    if ((p2.y - p1.y) == 0 && (p2.x - p1.x) > 0) { return 0.0; }
    if ((p2.y - p1.y) > 0 && (p2.x - p1.x) > 0) { return alpha; }
    if ((p2.y - p1.y) > 0 && (p2.x - p1.x) == 0) { return 90.0; }
    if ((p2.y - p1.y) > 0 && (p2.x - p1.x) < 0) { return 180 - alpha; }
    if ((p2.y - p1.y) == 0 && (p2.x - p1.x) < 0) { return 180.0; }
    if ((p2.y - p1.y) < 0 && (p2.x - p1.x) < 0) { return 180.0 + alpha; }
    if ((p2.y - p1.y) < 0 && (p2.x - p1.x) == 0) { return 270.0; }
    if ((p2.y - p1.y) < 0 && (p2.x - p1.x) > 0) { return 360.0 - alpha; }
    
    return 0.0;
}

float getDegreeOfASegmentWithStartingPoint(Point p) {
    // Calculates and returns the degree of the segment (edge) of the polygon that starts with given point 'p'.
    return getDegree(polygon[p.ID], polygon[(p.ID+1)%N]);
}

float calculateDistanceBetween(Point p1, Point p2) {
    // Calculates and returns the Euclidean distance between two points.
    return sqrtf(powf((p1.x - p2.x), 2) + powf((p1.y - p2.y), 2));
}

void findInitialAntipodals(Point *polygon) {
    // Finds highest and lowest points on the polygon.
    // Assigns these points to 'minID' and 'maxID' variables.
    float minY = 1000000.0;
    float maxY = -1000000.0;
    for (int i = 0; i < N; i++) {
        if (polygon[i].y < minY) {
            minY = polygon[i].y;
            minID = polygon[i].ID;
        }
        if (polygon[i].y > maxY) {
            maxY = polygon[i].y;
            maxID = polygon[i].ID;
        }
    }
}

void initializeCalipers(Caliper *L, Caliper *U) {
    // Puts a lower caliper on the lowest point of the polygon and sets its degree to 0,
    // and puts a upper caliper on highest point of the polygon and sets its degree to 180.
    L->initialPointID = minID;
    L->currentPointID = minID;
    L->degree = 0.0;
    L->diffDegree = getDegreeOfASegmentWithStartingPoint(polygon[minID]) - 0.0;
    L->completedPath = false;
    
    U->initialPointID = maxID;
    U->currentPointID = maxID;
    U->degree = 180.0;
    U->diffDegree = getDegreeOfASegmentWithStartingPoint(polygon[maxID]) - 180.0;
    U->completedPath = false;
}

void addAntipodalPairs(int first, int second, int a, int b) {
    // Depending on the 'a' and 'b' values, this function adds 4, 2 or 1 antipodal pairs.
    // exp: If 'a' is 1 then that means not just add 'first' point but add 'first+1'th point also
    // as an antipodal pair. This is same for 'b' and 'second'.
    int temp = b;
    while (a >= 0) {
        while (b >= 0) {
            if (!(a == 1 && b == 1) && toDisplay) {
                struct point p1 = polygon[(first+a) % N];
                struct point p2 = polygon[(second+b) % N];
                if (calculateDistanceBetween(p1, p2) > maxDist) {
                    sprintf(bestPair, "%c-%c", p1.label, p2.label);
                    maxDist = calculateDistanceBetween(p1, p2);
                }
                printf("pair: %c-%c\n", p1.label, p2.label);
            } b--;
        }
        b = temp; a--;
    }
}

void updateCalipersAfterRotation(Caliper *L, Caliper *U) {
    // Makes necessary updates for calipers after rotation.
    L->currentPointID = L->currentPointID % N;
    U->currentPointID = U->currentPointID % N;
    
    // If any of the degrees is greater than 360.0 then subtract 360.0 from it.
    // Modulo doesn't work for float values.
    if (L->degree > 360.0 ) { L->degree = L->degree - 360.0; }
    if (U->degree > 360.0 ) { U->degree = U->degree - 360.0; }
    
    // When upper caliper reaches the lower calipers initial point or vice versa
    // that means caliper completed its path. When both complete all procedure stops.
    if (L->currentPointID == U->initialPointID) {
        L->completedPath = true;
    }
    if (U->currentPointID == L->initialPointID) {
        U->completedPath = true;
    }
    
}

void rotatingCalipers(Caliper *L, Caliper *U) {
    // Main procedure of the process of calculating the diameter of a convex polygon.
    
    while (!L->completedPath || !U->completedPath) {
        L->diffDegree = fabs(getDegreeOfASegmentWithStartingPoint(polygon[L->currentPointID]) - L->degree);
        U->diffDegree = fabs(getDegreeOfASegmentWithStartingPoint(polygon[U->currentPointID]) - U->degree);
        
        // If next segments (edges) are parallel with each other...
        // Also means that both of the calipers overlap with polygons edges
        if (L->diffDegree == U->diffDegree) {
            addAntipodalPairs(L->currentPointID, U->currentPointID, 1, 1);
            L->currentPointID++;
            U->currentPointID++;
            L->degree += L->diffDegree;
            U->degree += U->diffDegree;
        }
        // If one of the calipers is overlaps with one of the edges of a polygon...
        else if ( L->diffDegree == 0.0 || U->diffDegree == 0.0 ) {
            Caliper *tmp1 = (L->diffDegree == 0) ? L : U;
            Caliper *tmp2 = (tmp1 == L) ? U : L;
            addAntipodalPairs(tmp1->currentPointID, tmp2->currentPointID, 1, 0);
            tmp1->currentPointID++;
        }
        // If "difference with the next segment" is different for two calipers...
        else {
            Caliper *far = (L->diffDegree >= U->diffDegree) ? L : U;
            Caliper *close = (L->diffDegree < U->diffDegree) ? L : U;
            float difference = fabs(far->diffDegree - close->diffDegree);
            far->degree += difference;
            close->degree += difference;
            addAntipodalPairs(far->currentPointID, close->currentPointID, 0, 0);
            close->currentPointID++;
        }
        updateCalipersAfterRotation(L, U);
    }
}

void complexityAnalysis() {
    // Function for printing the time used by the 'rotatingCalipers' procedure with different sized inputs
    // And drawing bar diagram for the results.
    
    // Initialize variables for storing the starting and ending time of the process
    clock_t start, end;
    
    // Set 'toDisplay' value to 'false' to indicate that we don't want results to get printed on screen.
    toDisplay = false;
    
    // Allocate memory for polygon
    polygon = malloc(sizeof(Point)*2);
    
    // Initialize variable for storing the time of the process.
    double cpuTimeUsed;
    
    // Create variable 'x' for determining maximum coefficient (max is 'x-1').
    int x = 11;
    
    // Initialize an array of size 'x' for storing values from index 1 to 'x-1'.
    // Starts from 1 because we don't need the coefficient value of zero in the loop below.
    int results[x];
    
    // Initialize variable for storing unit time.
    double unit = 0.15;
    
    for (int i = 1; i <= x-1; i++) {
        // for every coefficient from 1 to x-1...
        
        // Determine the number of sides of the polygon
        N = 1000000 * i;
        
        // Reallocate memory for polygon with that size.
        Point *temp = realloc(polygon, sizeof(Point)*N);
        polygon = temp;
        
        // Create the polygon
        createNSidedPolygon(polygon, N);
        
        // Allocate memory for calipers
        Caliper *L = malloc(sizeof(Caliper));
        Caliper *U = malloc(sizeof(Caliper));
        
        // Find initial antipodal pairs for beginning of the process
        findInitialAntipodals(polygon);
        
        // Set appropriate initial values for calipers
        initializeCalipers(L, U);
        
        // Get time when procedure starts
        start = clock();
        
        // Start procedure
        rotatingCalipers(L, U);
        
        // Get time when procedure ends
        end = clock();
        
        // Free memory used by calipers
        free(L); free(U);
        
        // Calculate the time used by process
        cpuTimeUsed = ((double) (end - start)) / CLOCKS_PER_SEC;
        
        // Determine first 'cpuTimeUsed' as unit time.
        if (i == 1) { unit = cpuTimeUsed; }
        
        // Store the time in results array
        results[i] = (int) (cpuTimeUsed / unit);
        
        // Prinf time info to the consol
        printf("For n: %d, time used: %f\n", 1000000 * i, cpuTimeUsed);
    }
    
    printf("\nUnit time: %f seconds\n\n", unit);
    
    // MARK: Drawing bar diagram
    for (int i = 1; i <= x-1; i++) {
        printf("%d ", 1000000 * i);
        
        // For the symmetry of the bar diagram
        if (i != x-1) { printf(" "); }
        
        for (int j = 0; j < results[i]; j++) {
            printf("🟧");
        }
        printf(" %d\n", results[i]);
    }
    free(polygon);
}

void createNSidedPolygon(Point *polygon, int n) {
    // Function for constructing the corners of a regular 'n' sided polygon.
    double radius = 100.0;
    char label = 'A';
    for (int i = 0; i < n; i++) {
        struct point p;
        p.ID = n-1-i;
        p.label = label+i;
        p.x = sin((double) i / (double) n * 2.0 * PI) * radius;
        p.y = cos((double) i / (double) n * 2.0 * PI) * radius;
        polygon[n-i-1] = p;
//        printf("%d. point %c x: %f, y: %f\n", p.ID, p.label, p.x, p.y);
    }
}

int main(void) {
  
//    MARK: Special cases for testing the validity of results.
    N = 7;
    polygon = malloc(sizeof(Point)*N);
    if (polygon == NULL) { return 1; }

    if (N == 7) { createSpecialCase(polygon); }
    else { printf("N must be set to 7 before calling 'createSpecialCase' function.\n"); return 1;}
//    createNSidedPolygon(polygon, N);

    Caliper *L = malloc(sizeof(Caliper));
    Caliper *U = malloc(sizeof(Caliper));
    if (L == NULL || U == NULL) { return 1; }

    findInitialAntipodals(polygon);
    initializeCalipers(L, U);

    rotatingCalipers(L, U);

    free(polygon);
    free(L);
    free(U);
    printf("Diameter: %f, pairs: %s\n", maxDist, bestPair);
    
//    MARK: Complexity analysis
//    complexityAnalysis();
    
    printf("\n");
    return 0;
}
