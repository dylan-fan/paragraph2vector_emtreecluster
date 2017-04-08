#ifndef VECTORSTREAM_H
#define	VECTORSTREAM_H

#include "StdIncludes.h"
#include "SVector.h"

namespace lmw {

/**
 * The VectorStream concept provides a stream of vectors usually read from disk. 
 * 
 * The only operations supported are:
 * 
 * size_t VectorStream<T>.read(size_t n, vector<SVector<T>*>* data)
 *      where n is the number of vectors to read into data
 *      stream has ended when return == 0
 * 
 * VectorStream<T>.free(vector<T*>& data)
 *      frees the memory allocated by the stream
 * 
 * For example,
 *      VectorStream<bool> bvs(idFile, signatureFile);
 *      for (;;) {
 *          vector<SVector<bool>*> data;
 *          size_t read = bvs.read(10000, &data);
 *          if (read == 0) {
 *              break;
 *          }
 *          process(&data);
 *          bvs.free(&data);
 *      }
 */
template <typename SVECTOR>
class SVectorStream {
    size_t read(size_t n, vector<SVECTOR*>* data) {
        return 0;
    }
    
    void free(vector<SVECTOR*>* data) {
        
    }
};

template <>
class SVectorStream<SVector<bool>> {
public:
    /**
     * @param idFile An ASCII file with one object ID per line.
     * @param signatureFile A file of binary signatures containing as many
     *                      signatures as there are lines in idFile. 
     * @param signatureLength The length of a signature in bits.
     */
    SVectorStream(const string& idFile, const string& signatureFile,
            const size_t signatureLength)
            : SVectorStream(idFile, signatureFile, signatureLength, -1) { }
	
    /**
	 * @param idFile An ASCII file with one object ID per line.
	 * @param signatureFile A file of binary signatures containing as many
	 *                      signatures as there are lines in idFile.
	 * @param signatureLength The length of a signature in bits.
     * @param maxToRead The maximum number of vectors to read. A value of -1
     *                  indicates to read all.
	*/
	SVectorStream(const string& idFile, const string& signatureFile,
            const size_t signatureLength, const size_t maxToRead) 
            : _buffer(signatureLength / 8, 0),
            _idStream(idFile),
            _signatureStream(signatureFile, ios::in | ios::binary),
            _signatureLength(signatureLength),
            _maxToRead(maxToRead),
            _count(0) {
        if (signatureLength % 64 != 0) {
            throw runtime_error("length is not divisible by 64");
        }
        if (!_idStream) {
            throw runtime_error("failed to open " + idFile);
        }
        if (!_signatureStream) {
            throw runtime_error("failed to open " + signatureFile);
        }
	}

    size_t read(size_t n, vector<SVector<bool>*>* data) {
        string id;
        size_t read = 0;
		if (_maxToRead != -1 && _count >= _maxToRead) return 0;
		while (getline(_idStream, id)) {
            _signatureStream.read(&_buffer[0], _buffer.size());
            SVector<bool>* vector = new SVector<bool>(&_buffer[0], _signatureLength);
            vector->setID(id);
            data->push_back(vector);
			++_count;
			if (_maxToRead != -1 && _count >= _maxToRead) break;
            if (++read == n) {
                break;
            }
        }
        return read;
    }
    
    void free(vector<SVector<bool>*>* data) {
        for (auto vector : *data) {
            delete vector;
        }
    }
    
private:
    vector<char> _buffer; // temporary buffer for reading a signature
    ifstream _idStream;
    ifstream _signatureStream;
    size_t _signatureLength; // the length of signatures in _signatureStream
	size_t _maxToRead;
	size_t _count; // Number of vectors read so far
};


// add by fantao at 2015-8-19; 
// reading doc2vector file;
template <>
class SVectorStream<SVector<double>> {
public:   
	
    /**
	 * 
 	 * @param doc_vector_file A file of of doc2vector file;
	 * @param vector_length The length of a vector.
     * @param maxToRead The maximum number of vectors to read. A value of -1
     *                  indicates to read all.
	*/

	SVectorStream(const string& doc_vector_file,
            const size_t vector_length)
            : SVectorStream(doc_vector_file, vector_length,  -1) { }

	
	SVectorStream(const string& doc_vector_file,
            const size_t vector_length, const size_t maxToRead) 
            :
            _vectorStream(doc_vector_file),
			_count(0),
			 _maxToRead(maxToRead),
            _vector_length(vector_length){       
       
        if (!_vectorStream) {
            throw runtime_error("failed to open " + doc_vector_file);
        }
	}

    size_t read(size_t n, vector<SVector<double>*>* data) {
        string docid;
		string v1;
        size_t read = 0;
		if (_maxToRead != -1 && _count >= _maxToRead) return 0;
		string vec_str;
		while (getline(_vectorStream, vec_str)) {
			int size = vec_str.size();
			int flag = 0;
			int vec_pos = 0;			
			SVector<double>* vector = new SVector<double>(_vector_length);
			
			for(int i = 0; i < size; i++){
				int pos = vec_str.find(" ", i);
				if (pos < size && pos >= 0 ){
					v1 = vec_str.substr(i, pos - i); 			
					i = pos;						   
					}
				
				else{				 
					v1 = vec_str.substr(i, size-i);				
					i=size; 		   
					}
				
				if (flag == 0){
					docid = v1;
					flag = 1;				
					
					vector->setID(docid);				
					}
				
				else{					
					double vec = atof(v1.c_str());
					vector->set(vec_pos, vec);
					vec_pos++;
					}
				
				}
			
            data->push_back(vector);
			
			++_count;
			if (_maxToRead != -1 && _count >= _maxToRead) break;
            if (++read == n) {
                break;
            }
        }
		
        return read;
		
    }
    
    void free(vector<SVector<double>*>* data) {
        for (auto vector : *data) {
            delete vector;
        }
    }
    
private:    
    ifstream _vectorStream;    
    size_t _vector_length; // the length of signatures in _signatureStream
    size_t _maxToRead;
	size_t _count; // Number of vectors read so far
};


} // namespace lmw

#endif	/* VECTORSTREAM_H */

