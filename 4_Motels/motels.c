#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct motel {
    int type;
    int distance;
} motel;

motel *motels;
int numberOfMotels;

// Checks if given 3 motel indexes are not the same type or invalid
bool TripletIsValid(int first, int mid, int last) {
    if(first == -1 || mid == -1 || last == -1) {
        return false;
    }
    return motels[first].type != motels[mid].type
           && motels[first].type != motels[last].type
           && motels[mid].type != motels[last].type;
}

bool InRangeOfAnArray(int checkMotel) {
    return checkMotel < numberOfMotels && checkMotel != -1;
}

bool MotelsAreUnique(int* uniqueMotels, int first) {
    for (int i = 0; i < 3; ++i) {
        if(motels[uniqueMotels[i]].type == motels[first].type) {
            return false;
        }
        for (int j = i + 1; j < 3; ++j) {
            if(motels[uniqueMotels[i]].type == motels[uniqueMotels[j]].type) {
                return false;
            }
        }
    }
    return true;
}

// If encountered 1 or more of the same mote types in the line
// move to the last one
int MoveToTheLastDupe(int currentIndex) {
    int lastDupe = currentIndex;
    for (int i = currentIndex; i < numberOfMotels; ++i) {
        if(motels[currentIndex].type == motels[i].type) {
            lastDupe = i;
        }
        else {
            break;
        }
    }
    return lastDupe;
}

// Calculate distances between first-mid and mid-last motel
// and return the maximum
int CalculateMaxDistance(int first, int mid, int last) {
    int distance1 = motels[mid].distance - motels[first].distance;
    int distance2 = motels[last].distance - motels[mid].distance;
    if(distance1 > distance2) {
        return distance1;
    }
    else {
        return distance2;
    }
}

// Calculate distances between first-mid and mid-last motel
// and return the minimum
int CalculateMinDistance(int first, int mid, int last) {
    int distance1 = motels[mid].distance - motels[first].distance;
    int distance2 = motels[last].distance - motels[mid].distance;
    if(distance1 < distance2) {
        return distance1;
    }
    else {
        return distance2;
    }
}

// loop through all the motels in between given start and the end
// return one that gives the best minimized distance
int FindBestMidClosest(int start, int end) {
    int max = motels[end].distance - motels[start].distance;
    int indexOfMax = -1;
    int mid = start + 1;
    while(mid < end){
        if(motels[mid].type == motels[start].type || motels[mid].type == motels[end].type) {
            mid++;
            continue;
        }
        int currentMax = CalculateMaxDistance(start, mid, end);
        if(currentMax <= max) {
            max = currentMax;
            indexOfMax = mid;
        }
        mid++;
    }
    return indexOfMax;
}

// loop through all the motels in between given start and the end
// return one that is valid and gives the best maximized distance
int FindBestMidFurthest(int start, int end) {
    int min = 0;
    int indexOfMin = -1;
    int mid = start + 1;
    while(mid < end) {
        if(motels[mid].type == motels[start].type || motels[mid].type == motels[end].type) {
            mid++;
            continue;
        }
        if(TripletIsValid(start, mid, end)) {
            int currentMin = CalculateMinDistance(start, mid, end);
            if(currentMin > min) {
                min = currentMin;
                indexOfMin = mid;
            }
            mid++;
        }
    }
    return indexOfMin;
}

// move forward to the first motel wit different type
int MoveToTheNextViableValue(int motelToMove) {
    for (int movedMotel = motelToMove + 1; movedMotel < numberOfMotels; ++movedMotel) {
        if(motels[movedMotel].type != motels[motelToMove].type) {
            return movedMotel;
        }
    }
    return -1;
}

// move backwards to the first motel with different type
int MoveToThePrevViableValue(int motelToMove) {
    for (int movedMotel = motelToMove - 1; movedMotel >= 0; --movedMotel) {
        if(motels[movedMotel].type != motels[motelToMove].type) {
            return movedMotel;
        }
    }
    return -1;
}

// this function calculates the closest viable motels and returns maximum distance in between them
// it does so by finding first triplet and simply moving first and last motel to the next one
// the first motel must always be the one furthest right if there are any dupes in a row
// and the last motel must always be the one furthest left
// after finding values for the first and the last one it checks every motel in between (those are the same type) to find the best one
// if such triplet is valid (it contains 3 different motel types) its value is calculated
// after reaching the last motel it returns the lowest calculated value
int MotelsClosestApartValue() {
    int validTriplets = 0; // simple value to check whether there are at least 3 hotels of different type
    int first = 0;
    first = MoveToTheLastDupe(first);
    int mid = MoveToTheNextViableValue(first);
    int last = MoveToTheNextViableValue(mid);
    mid = FindBestMidClosest(first, last);

    if(mid == -1 || last == -1) {
        return 0; // when there are no more than 2 motel types in a row
    }

    int minValue = INT_MAX;
    if(TripletIsValid(first, mid, last)) { // if the first chosen triplet is valid - calculate its value
        validTriplets++;
        minValue = CalculateMaxDistance(first, mid, last);
    }

    do { // loop through all motels by moving all possible motels at once
        last = MoveToTheNextViableValue(last);
        first = MoveToTheNextViableValue(first);
        first = MoveToTheLastDupe(first);
        mid = FindBestMidClosest(first, last);

        if(TripletIsValid(first, mid, last)) {
            validTriplets++;
            int value = CalculateMaxDistance(first, mid, last);
            if(value < minValue) {
                minValue = value;
            }
        }
    } while(InRangeOfAnArray(last));

    if(validTriplets == 0) {
        return 0;
    }
    return minValue;
}

// this function calculates the furthest viable motels and returns minimum distance in between them
// it does by taking three first motels and for each calculating best possible end and mid value
// since there are only 3 motels to choose for every first one it needs to choose on of 3 other that are unique as end ones
// end then find the best viable mid motel
int MotelsFurthestApartValue() {
    int first = 0;
    int maxValue = 0;

    for (int i = 0; i < 3; ++i) { // find best values for first 3 motels
        if(!InRangeOfAnArray(first)) { // if the first motel gets out of range
            break;
        }

        // set last motel to the last viable one in array
        int last = numberOfMotels - 1;
        if(motels[first].type == motels[last].type) {
            last = MoveToThePrevViableValue(last);
        }

        // setup array that will be filled with 3 motels of unique type
        int* uniqueLastMotels = (int *) malloc((3 * sizeof(int)));
        uniqueLastMotels[0] = last; // set the 1st unique motel
        uniqueLastMotels[1] = uniqueLastMotels[0];
        uniqueLastMotels[2] = -1; // not assigned yet

        // find the 2nd unique motel
        while(!MotelsAreUnique(uniqueLastMotels, first)) {
            if(uniqueLastMotels[1] <= first) {
                break;
            }
            uniqueLastMotels[1] = MoveToThePrevViableValue(uniqueLastMotels[1]);
        }

        // find the 3rd unique motel
        uniqueLastMotels[2] = uniqueLastMotels[1];
        while(!MotelsAreUnique(uniqueLastMotels, first)) {
            if(uniqueLastMotels[2] <= first) {
                break;
            }
            uniqueLastMotels[2] = MoveToThePrevViableValue(uniqueLastMotels[2]);
        }

        // check the best distance for every combination of "first" motel and every of 3 possible "last" ones
        // for each one loop through the array and find the best mid one
        for (int j = 0; j < 3; ++j) {
            if(first >= uniqueLastMotels[j])
            {
                break;
            }
            if(uniqueLastMotels[j] != -1) { // if motel isn't out of scope
                int midMax = FindBestMidFurthest(first, uniqueLastMotels[j]); // find the best mid motel to maximize the value
                if(TripletIsValid(first, midMax, uniqueLastMotels[j])) {
                    int currentValue = CalculateMinDistance(first, midMax, uniqueLastMotels[j]);
                    if(currentValue > maxValue) {
                        maxValue = currentValue;
                    }
                }
            }

        }

        // find next "first" motel that isn't equal to the first or previous one
        // since there are only 3 motels to check
        // at the last (3rd) iteration of this for-loop it should set 3 motels with unique types
        int prevMotel = first;
        while ((motels[first].type == motels[0].type || motels[first].type == motels[prevMotel].type) && first != -1) {
            first = MoveToTheNextViableValue(first);
        }
    }
    return maxValue;
}

int main() {
    scanf("%d", &numberOfMotels);
    motels = (motel *)(int *) malloc((long unsigned int)numberOfMotels * sizeof(motel));

    for (int i = 0; i < numberOfMotels; ++i) {
        scanf("%d", &motels[i].type);
        scanf("%d", &motels[i].distance);
    }

    printf("%d %d", MotelsClosestApartValue(), MotelsFurthestApartValue());
    free(motels);
    return 0;
}