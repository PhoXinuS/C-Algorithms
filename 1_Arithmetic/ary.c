#include <math.h>
#include "ary.h"

typedef struct separatedPart {
    double end;
    double infinityType;
}separatedPart;

const double epsilon = 1e-12;
bool equal(double x, double y) {
    return fabs(x - y) < epsilon;
}

wartosc wartosc_dokladnosc(double x, double p) {
    wartosc w;
    w.isSeparated = false;
    w.x = x - (x * p / 100);
    w.y = x + (x * p / 100);
    if(w.x > w.y) {
        double temp = w.x;
        w.x = w.y;
        w.y = temp;
    }
    return w;
}

wartosc wartosc_od_do(double x, double y) {
    wartosc w;
    w.isSeparated = false;
    w.x = x;
    w.y = y;
    return w;
}

wartosc wartosc_dokladna(double x) {
    wartosc w;
    w.isSeparated = false;
    w.x = x;
    w.y = x;
    return w;
}

bool in_wartosc(wartosc w, double x) {
    if(w.isSeparated) {
        return x <= w.y || x >= w.x;
    }
    else {
        return x <= w.y && x >= w.x;
    }
}

double min_wartosc(wartosc w) {
    if(w.isSeparated) {
        return -HUGE_VAL;
    }
    return w.x;
}

double max_wartosc(wartosc w) {
    if(w.isSeparated) {
        return HUGE_VAL;
    }
    return w.y;
}

double sr_wartosc(wartosc w) {
    if(equal(w.x, HUGE_VAL) && equal(w.y, -HUGE_VAL)) {
        return HUGE_VAL;
    }
    else if(!equal(w.x, HUGE_VAL) && equal(w.y, -HUGE_VAL)) {
        return -HUGE_VAL;
    }
    if((equal(w.x, -HUGE_VAL) && equal(w.y, HUGE_VAL)) || w.isSeparated) {
        return NAN;
    }
    return (w.x + w.y) / 2;
}

////////////

wartosc plus(wartosc a, wartosc b) {
    wartosc w;

    w.isSeparated = false;
    if(isnan(a.x) || isnan(a.y) || isnan(b.x) || isnan(b.y)) {
        w.x = NAN;
        w.y = NAN;
        return w;
    }

    w.x = a.x + b.x;
    w.y = a.y + b.y;
    if(a.isSeparated) {

        // if the range is separated but the gap was filled
        if(b.isSeparated && w.y >= w.x) {
            w.isSeparated = false;
            w.x = -HUGE_VAL;
            w.y = HUGE_VAL;
            return w;
        }
        w.isSeparated = true;
    }
    else if(b.isSeparated && !a.isSeparated) {
        w.isSeparated = true;
    }
    return w;
}

wartosc minus(wartosc a, wartosc b) {
    wartosc w;

    w.isSeparated = false;
    if(isnan(a.x) || isnan(a.y) || isnan(b.x) || isnan(b.y)) {
        w.x = NAN;
        w.y = NAN;
        return w;
    }

    w.x = a.x - b.y;
    w.y = a.y - b.x;

    if(a.isSeparated) {

        // if the range is separated but the gap has been filled
        if(b.isSeparated && w.y >= w.x) {
            w.isSeparated = false;
            w.x = -HUGE_VAL;
            w.y = HUGE_VAL;
            return w;
        }
        w.isSeparated = true;
    }
    else if(b.isSeparated && !a.isSeparated) {
        w.isSeparated = true;
    }
    return w;
}

// a function that handles the case of multiplying inf by zero to be equal 0
// which is needed in case of this programme
double multiply(double a, double b) {
    if((equal(a, 0) && isinf(b)) || (isinf(a) && equal(b, 0))) {
        return 0;
    }
    return a * b;
}

wartosc razy(wartosc a, wartosc b) {
    wartosc w;
    w.isSeparated = false;

    if(isnan(a.x) || isnan(a.y) || isnan(b.x) || isnan(b.y)) {
        w.x = NAN;
        w.y = NAN;
        return w;
    }

    // case of multiplying two ranges that are not separated
    if(!a.isSeparated && !b.isSeparated) {
        // end values used to calculate possible range closures
        double end[4];
        end[0] = multiply(a.x, b.x);
        end[1] = multiply(a.x, b.y);
        end[2] = multiply(a.y, b.x);
        end[3] = multiply(a.y, b.y);

        double min, max;
        min = end[0];
        max = end[0];
        for (int i = 1; i < 4; ++i) // find the min and max possible end closure
        {
            if(end[i] < min) {
                min = end[i];
            }
            else if (end[i] > max) {
                max = end[i];
            }
        }
        w.x = min;
        w.y = max;
    }
    // case of multiplying two ranges that are both separated
    else if(a.isSeparated && b.isSeparated) {
        if(a.x < 0 || a.y > 0 || b.x < 0 || b.y > 0) { // if any of separated ranges contain both negative and positive
            w.isSeparated = false;
            w.x = -HUGE_VAL;
            w.y = HUGE_VAL;
            return w;
        }

        // calculate the lowest possible [x, inf]
        w.x = multiply(a.y, b.y);
        if(multiply(a.x, b.x) < w.x) {
            w.x = multiply(a.x, b.x);
        }

        // calculate the highest possible [-inf, y]
        w.y = multiply(a.x, b.y);
        if(multiply(a.y, b.x) > w.y) {
            w.y = multiply(a.y, b.x);
        }
        w.isSeparated = true;
    }
    // case of multiplying a not-separated range with a separated one
    else {
        // algorithm assumes that a is separated and b is not so if otherwise exchange them
        if(b.isSeparated) {
            wartosc temp = a;
            a = b;
            b = temp;
        }

        // represent a separated range as two independent parts
        separatedPart separated1;
        separated1.end = a.y;
        separated1.infinityType = -INFINITY; // [-inf, y]

        separatedPart separated2;
        separated2.end = a.x;
        separated2.infinityType = INFINITY; // [x, inf]

        // calculate how independent parts of a range change during multiplication
        separatedPart separatedEnds[4];
        separatedEnds[0].end = multiply(separated1.end, b.x);
        separatedEnds[0].infinityType = multiply(separated1.infinityType, b.x);
        separatedEnds[1].end = multiply(separated1.end, b.y);
        separatedEnds[1].infinityType = multiply(separated1.infinityType, b.y);

        separatedEnds[2].end = multiply(separated2.end, b.x);
        separatedEnds[2].infinityType = multiply(separated2.infinityType, b.x);
        separatedEnds[3].end = multiply(separated2.end, b.y);
        separatedEnds[3].infinityType = multiply(separated2.infinityType, b.y);

        double lowestPlusInfinity = INFINITY; // lowest independent part wih +inf
        double highestMinusInfinity = -INFINITY; // highest independent part with -inf
        for (int i = 0; i < 4; ++i) {
            // is +inf
            if(separatedEnds[i].infinityType > 0) {
                if (separatedEnds[i].end < lowestPlusInfinity) {
                    lowestPlusInfinity = separatedEnds[i].end;
                }
            }
            // is -inf
            else {
                if (separatedEnds[i].end > highestMinusInfinity) {
                    highestMinusInfinity = separatedEnds[i].end;
                }
            }
        }
        w.isSeparated = true;
        w.y = highestMinusInfinity;
        w.x = lowestPlusInfinity;
    }

    // if the gap has been filled
    if(w.isSeparated && w.y >= w.x) {
        w.isSeparated = false;
        w.x = -HUGE_VAL;
        w.y = HUGE_VAL;
        return w;
    }
    return w;
}

wartosc reverse_wartosc(wartosc reverseMe) {
    // reverses the value, if value is 0 it gets a negative or positive HUGE_VAL
    wartosc reversed;
    if(equal(reverseMe.y, 0)) {
        reversed.x = -HUGE_VAL;
    }
    // if the value isn't 0 it is simply reversed as 1/x
    else {
        reversed.x = 1 / reverseMe.y;
    }
    if(equal(reverseMe.x, 0)) {
        reversed.y = HUGE_VAL;
    }
    else {
        reversed.y = 1 / reverseMe.x;
    }

    // if the range to be reversed has negative, positive and a zero it means it gets separated
    if(!reverseMe.isSeparated && reverseMe.x < 0 && reverseMe.y > 0) {
        reversed.isSeparated = true;
    }
    else {
        reversed.isSeparated = false;
    }
    return reversed;
}

// most of this function covers severe edge cases for which reversing the divisor wouldn't work
// so those edge cases are calculated in different ways
// if none of edge cases were met then it is reversing the divisor and getting a result from a razy() function above
// because division is just a multiplication by a reversed value (a / b = a * 1 / b)
wartosc podzielic(wartosc a, wartosc b) {
    wartosc w;

    // EDGE CASES:

    bool dividendHasPositive = max_wartosc(a) > 0;
    bool dividendHasNegative = min_wartosc(a) < 0;

    bool divisorHasPositive = max_wartosc(b) > 0;
    bool divisorHasNegative = min_wartosc(b) < 0;

    // if both divisor and dividend has 0 AND at least of them contains both negative and positive values
    if((in_wartosc(a, 0.0) && in_wartosc(b, 0.0))
    && ((dividendHasNegative && dividendHasPositive) || (divisorHasPositive && divisorHasNegative))) {
        w.isSeparated = false;
        w.x = -HUGE_VAL;
        w.y = HUGE_VAL;
        return w;
    }

    if(equal(b.x, 0) && equal(b.y, 0)) {
        w.isSeparated = false;
        w.x = NAN;
        w.y = NAN;
        return w;
    }

    // if divisor is separated and contains 0
    if(b.isSeparated && in_wartosc(b, 0)) {

        if(dividendHasNegative && dividendHasPositive) {
            w.isSeparated = false;
            w.x = -HUGE_VAL;
            w.y = HUGE_VAL;
            return w;
        }

        // represent this calculation in 3 independent parts
        // for example [10] / [-inf, 10] u [100, inf]
        // which is a sum of 3 ranges
        // 0. [10] / [-inf, 0]
        // 1. [10] / [100, inf]
        // 2. [10] / [0, 10]
        // those 3 ranges are represented in an independentRange array
        wartosc independentRange[3];
        if(b.x <= 0) { // [lower than 0, inf]
            if(dividendHasNegative) {
                // [- inf, 0]
                independentRange[0].x = -HUGE_VAL;
                independentRange[0].y = 0;
            }
            else if(dividendHasPositive) {
                // [0, inf]
                independentRange[0].x = 0;
                independentRange[0].y = HUGE_VAL;
            }
            independentRange[1] = podzielic(a, wartosc_od_do(-HUGE_VAL, b.y));
            independentRange[2] = podzielic(a, wartosc_od_do(b.x, 0));
        }
        else if(b.y >= 0) { // (-inf, higher than zero)
            if(dividendHasNegative) {
                // (0, inf)
                independentRange[0].x = 0;
                independentRange[0].y = HUGE_VAL;
            }
            else if(dividendHasPositive) {
                // (-inf, 0)
                independentRange[0].x = -HUGE_VAL;
                independentRange[0].y = 0;
            }
            independentRange[1] = podzielic(a, wartosc_od_do(b.x, HUGE_VAL));
            independentRange[2] = podzielic(a, wartosc_od_do(0, b.y));
        }
        else {
            independentRange[0].x = NAN;
            independentRange[0].y = NAN;
            independentRange[1] = independentRange[0];
            independentRange[2] = independentRange[0];
        }

        // connect those independent ranges into a single separated range
        w.isSeparated = false;
        w.x = independentRange[0].x;
        w.y = independentRange[0].y;
        if(independentRange[1].x < w.x) {
            w.x = independentRange[1].x;
        }
        if(independentRange[1].y > w.y) {
            w.y = independentRange[1].y;
        }
        if(isinf(independentRange[2].x) || isinf(independentRange[2].y)) {
            w.isSeparated = true;
            if(isinf(w.x)) {
                w.x = independentRange[2].x;
            }
            else if(isinf(w.y)) {
                w.y = independentRange[2].y;
            }
        }
        return w;
    }

    // if the gap has been filled
    if(a.isSeparated && b.isSeparated) {
        w.isSeparated = false;
        w.x = -HUGE_VAL;
        w.y = HUGE_VAL;
        return w;
    }

    // NORMAL DIVISION:

    // if not, reverse a divisor and multiply it by dividend
    wartosc reversed = reverse_wartosc(b);
    return razy(a, reversed);
}