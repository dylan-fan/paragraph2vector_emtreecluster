#ifndef KMEANS_H
#define KMEANS_H

#include "Cluster.h"
#include "Clusterer.h"
#include "Seeder.h"
#include "StdIncludes.h"
#include "tbb/atomic.h"
#include "tbb/blocked_range.h"
#include "tbb/parallel_for.h"

namespace lmw {

template <typename T, typename SEEDER, typename OPTIMIZER>
class KMeans : public Clusterer<T> {
public:

    KMeans(int numClusters) : _numClusters(numClusters), _seeder(new SEEDER()) {
    }

    KMeans(int numClusters, float eps) : _numClusters(numClusters), _eps(eps),
            _seeder(new SEEDER()) {
    }

    ~KMeans() {
        // Need to clean up any created cluster objects
        Utils::purge(_clusters);
    }

    //vector<size_t>& getNearestCentroids() {
    //    return _nearestCentroid;
    //}

    void setNumClusters(size_t numClusters) {
        _numClusters = numClusters;
    }

    void setMaxIters(int maxIters) {
        _maxIters = maxIters;
    }
    
    void setEnforceNumClusters(bool enforceNumClusters) {
        _enforceNumClusters = enforceNumClusters;
    }

    int numClusters() {
        return _numClusters;
    }

    vector<Cluster<T>*>& cluster(vector<T*> &data) {
        Utils::purge(_clusters);
        _clusters.clear();
        _finalClusters.clear();
        cluster(data, _numClusters);
        finalizeClusters(data);
        return _finalClusters;
    }
    
    /**
     * pre: cluster() has been called
     */
    double getRMSE() {
        double SSE = 0;
        size_t objects = 0;
        for (Cluster<T>* cluster : _clusters) {
            auto neighbours = cluster->getNearestList();
            objects += neighbours.size();
            SSE += _optimizer.sumSquaredError(cluster->getCentroid(), neighbours);
        }
        return sqrt(SSE / objects);
    }

private:
    void finalizeClusters(vector<T*> &data) {      
        // Create list of final clusters to return;
        bool emptyCluster = assignClusters(data);
        if (emptyCluster && _enforceNumClusters) {
            // randomly shuffle if k cluster were not created to enforce the number of clusters if required
            //std::cout << std::endl << "k-means is splitting randomly";
            vector<T*> shuffled(data);
            std::random_shuffle(shuffled.begin(), shuffled.end());
            {
                size_t step = shuffled.size(), i = 0, clusterIndex = 0;
                for (; i < shuffled.size(); i += step, ++clusterIndex) {
                    for (size_t j = i; j < i + step && j < shuffled.size(); ++j) {
                        _nearestCentroid[j] = clusterIndex;
                    }
                }
            }
            vectorsToNearestCentroid(data);
            recalculateCentroids(data);
            assignClusters(data);
        }
    }
    
    bool assignClusters(vector<T*> &data) {      
        // Create list of final clusters to return;
        bool emptyCluster = false;
        for (Cluster<T>* c : _clusters) {
            if (!c->getNearestList().empty()) {
                _finalClusters.push_back(c);
            } else {
                emptyCluster = true;
            }
        }
        return emptyCluster;
    }   

    /**
     * @param vectors       the vectors to form clusters for
     * @param clusters      the number of clusters to find (i.e. k)
     */
    void cluster(vector<T*> &data, size_t clusters) {
        // Setup initial state.
        _iterCount = 0;
        _numClusters = clusters;
        _nearestCentroid.resize(data.size());
        _seeder->seed(data, _centroids, _numClusters);

        // Create as many cluster objects as there are centroids
        for (T* c : _centroids) {
            _clusters.push_back(new Cluster<T>(c));
        }
        
        // First iteration
        vectorsToNearestCentroid(data);
        //cout<<"maxIter"<<_maxIters<<endl;
        if (_maxIters == 0) {
            return;
        }
        recalculateCentroids(data);
        if (_maxIters == 1) {
            return;
        }

        // Repeat until convergence.
        _converged = false;
        _iterCount = 1;
		
		// For testing
		//std::cout << endl << "Iteration: " << _iterCount << "\t" << getRMSE();

        while (!_converged) {
            vectorsToNearestCentroid(data);
            recalculateCentroids(data);
            _iterCount++;

			// For testing
			// std::cout << endl << "Iteration: " << _iterCount << "\t" << getRMSE();
			
            if (_maxIters != -1 && _iterCount >= _maxIters) {

                break;
            }
        }
    }

    /**
     * Assign vectors to nearest centroid.
     * Pre: seedCentroids() OR recalculateCentroids() has been called
     * Post: nearestCentroid contains all the indexes into centroids for the
     *       nearest centroid for the vector (nearestCentroid and vectors are
     *       aligned).
     * @return boolean indicating if there were any changes, i.e. was there
     *                 convergence
     */
    void vectorsToNearestCentroid(vector<T*> &data) {
        // Clear the nearest vectors in each cluster
        for (Cluster<T> *c : _clusters) {
            c->clearNearest();
        }
        _converged = true;


        // Parallel
        tbb::parallel_for(tbb::blocked_range<size_t>(0, data.size(), 1000),
                [&](const tbb::blocked_range<size_t>& r) {
                    for (size_t i = r.begin(); i != r.end(); ++i) {
                        //size_t nearest = nearestObj(data[i], _centroids);
                        auto nearest = _optimizer.nearest(data[i], _centroids);
                        if (nearest.index != _nearestCentroid[i]) {
                            _converged = false;
                        }
                        _nearestCentroid[i] = nearest.index;
                    }
                }
        );
        tbb::atomic_fence(); // make sure all writes are visible on all CPUs

        // Serial
        // Clear the nearest vectors in each cluster
        for (Cluster<T> *c : _clusters) {
            c->clearNearest();
        }
        // Accumlate into clusters
        for (size_t i = 0; i < data.size(); i++) {
            size_t nearest = _nearestCentroid[i];
            _clusters[nearest]->addNearest(data[i]);
        }


    }

    /**
     * Recalculate centroids after vectors have been moved to there nearest
     * centroid.
     * Pre: vectorsToNearestCentroid() has been called
     * Post: centroids has been updated with new vector data
     */
    void recalculateCentroids(vector<T*> &data) {
        tbb::parallel_for(tbb::blocked_range<size_t>(0, _clusters.size(), 2),
                [&](const tbb::blocked_range<size_t>& r) {
                    for (size_t i = r.begin(); i != r.end(); ++i) {
                        Cluster<T>* c = _clusters[i];
                        if (c->size() > 0) {
                            _optimizer.updatePrototype(c->getCentroid(), c->getNearestList(), _weights);
                        }
                    }
                }
        );
        tbb::atomic_fence(); // make sure all writes are visible on all CPUs
    }

    SEEDER *_seeder;
    OPTIMIZER _optimizer;
    
    // enforce the number of clusters required
    // if less than k clusters are produced, shuffle vectors randomly and split into k cluster
    bool _enforceNumClusters = false;

    // present number of iterations
    int _iterCount = 0;

    // maximum number of iterations
    // -1 - run until complete convergence
    // 0 - only assign nearest neighbors after seeding
    // >= 1 - perform this many iterations
    int _maxIters = 100;

    // How many clusters should be found? i.e. k
    int _numClusters = 0;

    vector<T*> _centroids;
    vector<Cluster<T>*> _clusters;
    vector<Cluster<T>*> _finalClusters;

    // The centroid index for each vector. Aligned with vectors member variable.
    vector<size_t> _nearestCentroid;

    // Weights for prototype function (we don't have to use these)
    vector<int> _weights;

    // Residual for convergence
    float _eps = 0.00001f;
    
    // has the clustering converged
    atomic<bool> _converged;    
};

} // namespace lmw

#endif	/* KMEANS_H */



