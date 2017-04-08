#ifndef SEEDER_H
#define SEEDER_H

#include "StdIncludes.h"

namespace lmw {

    template <typename T>
    class Seeder {
    public:

        Seeder() { }

        virtual ~Seeder() { }

        // Pre: The centroids vector is empty
        virtual void seed(vector<T*> &data, vector<T*> &centroids, int numCentres) = 0;
    };

} // namespace lmw

#endif



