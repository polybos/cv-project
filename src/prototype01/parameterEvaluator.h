#ifndef PARAMETEREVALUATOR_H
#define PARAMETEREVALUATOR_H

#include "fileLoader.h"
#include "tracking.h"
#include "classification.h"
#include "display.h"
#include "statistics.h"

#include <string>
#include <stack>

class ParameterEvaluator
{
public:
    ParameterEvaluator();

    void run(); //< need all objects to be set correctly

    void setFileLoaderObject( FileLoader *fileLoader );
    void setTrackingObject( Tracking *tracking );
    void setClassificationObject( Classification *classification );
    void setDisplayObject( Display *display );
    void setStatisticsObject( Statistics *statistics );

    void outputStatistics();

private:
    FileLoader *m_fileLoader;
    Tracking* m_tracking;
    Classification* m_classification;
    Display* m_display;
    Statistics* m_statistics;

    struct ClassificationTriplet
    {
        double scaleFactor;
        unsigned int minNeighbours;
        unsigned int minSize;
    };
    struct ParameterStatistic
    {
        ClassificationTriplet triplet;
        unsigned int detectedObjectsCount[eTrafficClassSize];
    };

    std::stack<ClassificationTriplet> m_classificationParameter;
    std::stack<ParameterStatistic> m_parameterStatistics;

    void mainLoop();
    bool finished(); //< return true, if there is no untestet parameter left
    void setDefaultTestParameter();
    void saveStatistic(); //< should be called, when one calculation is finish

    void printParameterStatistic( const ParameterStatistic parameterStatistic );
    template<typename T> void printElement( T t, const int &width ); //< simple helper function for nice output
};

#endif // PARAMETEREVALUATOR_H
