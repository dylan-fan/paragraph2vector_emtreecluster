/** 
 * This file contains DISTANCE functions. They take two objects, and return a
 * similarity or disimilarity.
 * 
 * For example, to bit vectors are compared using the hamming distance.
 * 
 * A DISTANCE function is called with pointers to two objects of type T.
 * 
 * The required operations are
 *      // Returns the distance.
 *      double operator()(T* , T*)
 * 
 *      // Returns the distance squared. This is required for calculating values
 *      // based on the squared error such as RMSE.
 *      double squared(T*, T*)      
 * 
 * For example,
 *      SVector<bool> a, b;
 *      hammingDistance hamming;
 *      double distance = hamming(&a, &b);
 *      double squaredDistance = hamming.squared(&a, &b);
 */

#ifndef DISTANCE_H
#define	DISTANCE_H

#include "SVector.h"

namespace lmw {

struct hammingDistance {
    double operator()(const SVector<bool> *v1, const SVector<bool> *v2) const {
        return SVector<bool>::hammingDistance(*v1, *v2);
    }
    
    double squared(const SVector<bool> *v1, const SVector<bool> *v2) const {
        double distance = operator()(v1, v2);
        return distance * distance;
    }    
};

template <typename T>
struct euclideanDistanceSq {    
    double operator()(const T *t1, const T *t2) const {
        typename T::const_iterator it1 = t1->begin();
        typename T::const_iterator it2 = t2->begin();
        double d, sum = 0.0f;
        for (it1 = t1->begin(), it2 = t2->begin(); it1 != t1->end(), it2 != t2->end(); it1++, it2++) {
            d = *it1 - *it2;
            sum = sum + (d * d);
        }
        return sum;
    }
    
    double squared(const T *t1, const T *t2) const {
        return operator()(t1, t2);
    }    
};


// add by fantao at 2015-8-6;
template <typename T>
struct cosinedistance {  
	/*
    double operator()(const T *t1, const T *t2) const {
        typename T::const_iterator it1 = t1->begin();
        typename T::const_iterator it2 = t2->begin();
        double d, sum = 0.0f;
		double norm1 = 0.0f, norm2 = 0.0f;
		double cosdis = 0.0f;
        for (it1 = t1->begin(), it2 = t2->begin(); it1 != t1->end(), it2 != t2->end(); it1++, it2++) {
            d = (*it1) * (*it2);
            sum = sum + (d * d);
			norm1 += (*it1) * (*it1);
			norm2 += (*it2) * (*it2);
        }

		if (norm1 < 1e-6 || norm2 < 1e-6){
			return 0.0f;
		}
		
		cosdis = sum / sqrt(norm1 * norm2);	
		
        return cosdis;
		
    }
    */

	double operator()(const T *t1, const T *t2) const {
        ;
        double d, sum = 0.0f;
		double norm1 = 0.0f, norm2 = 0.0f;
		double cosdis = 0.0f;
		size_t length = t1->size();
        //cout<<endl;
        //cout<<"length:"<<length<<endl;
        //cout<<"t1[0]:"<<(*t1)[0]<<endl;
        //cout<<"t1[1]:"<<(*t1)[1]<<endl;
        for (int i = 0; i < length; i++) {
           // cout<<"t1: "<<i<<(*t1)[i]<<endl;
            d = (*t1)[i] * (*t2)[i];
            sum = sum + d;
			norm1 += (*t1)[i] * (*t1)[i];
			norm2 += (*t2)[i] * (*t2)[i];
        }
        
        //cout<<"norm1:\t"<<norm1<<endl;
        //cout<<"norm2:\t"<<norm2<<endl;
		if (norm1 < 0.0000001 || norm2 < 0.0000001){
			return 0.0f;
		}
		
		cosdis = sum / sqrt(norm1 * norm2);	
	//    cout<<"sum:"<<sum<<endl;
      //  cout<<"cos:"<<cosdis<<endl;	
        return cosdis;
		
    }
	
    double squared(const T *t1, const T *t2) const {
		double distance = operator()(t1, t2);
		distance = 1.0 / (distance * distance + 0.00001);
        return distance;
    }    
};


template <typename T>
struct euclideanDistance {
    double operator()(const T *t1, const T *t2) const {
        return sqrt(_squared(t1, t2));
    }
    
    double squared(T *t1, T *t2) const {
        return _squared(t1, t2);
    }
    
    euclideanDistanceSq<T> _squared;
};

} // namespace lmw

#endif	/* DISTANCE_H */

