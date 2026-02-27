#include "predictor.hpp"

any MLPredictor::loadModel()
{
    return true;
}

any MLPredictor::predictEngagement(any features)
{
    return Math->random() * 100;
}

