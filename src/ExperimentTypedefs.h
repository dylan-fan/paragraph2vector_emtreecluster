#ifndef EXPERIMENTTYPEDEFS_H
#define	EXPERIMENTTYPEDEFS_H

#include "lmw/Distance.h"
#include "lmw/Prototype.h"
#include "lmw/SVector.h"
#include "lmw/Cluster.h"
#include "lmw/Clusterer.h"
#include "lmw/Seeder.h"
#include "lmw/DSquaredSeeder.h"
#include "lmw/RandomSeeder.h"
#include "lmw/VectorGenerator.h"
#include "lmw/StdIncludes.h"
#include "lmw/SVectorStream.h"
#include "lmw/Optimizer.h"

#include "lmw/KMeans.h"
#include "lmw/TSVQ.h"
#include "lmw/KTree.h"
#include "lmw/EMTree.h"
#include "lmw/StreamingEMTree.h"

using namespace lmw;

// change by fantao at 2015-8-16;
typedef SVector<double> vecType;
//typedef SVector<bool> vecType;


typedef RandomSeeder<vecType> RandomSeeder_t;

// change by fantao at 2015-8-16;
//typedef Optimizer<vecType, hammingDistance, Minimize, meanBitPrototype2> OPTIMIZER;

typedef cosinedistance<vecType>cosinedistance_type; 
typedef meanPrototype<vecType> meanPrototype_double;

typedef Optimizer<vecType, cosinedistance_type, Maximize, meanPrototype_double> OPTIMIZER;

typedef KMeans<vecType, RandomSeeder_t, OPTIMIZER> KMeans_t;

// change by fantao at 2015-8-16;
//typedef TSVQ<vecType, KMeans_t, hammingDistance> TSVQ_t;
typedef TSVQ<vecType, KMeans_t, cosinedistance_type> TSVQ_t;

typedef KTree<vecType, KMeans_t, OPTIMIZER> KTree_t;
typedef EMTree<vecType, KMeans_t, OPTIMIZER> EMTree_t;
typedef SVector<double> ACCUMULATOR;
typedef StreamingEMTree<vecType, ACCUMULATOR, OPTIMIZER> StreamingEMTree_t;

#endif	/* EXPERIMENTTYPEDEFS_H */

