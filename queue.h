#pragma once
#include <vector>
#include <stdint.h>

template<class T>
class FIFOQueue {
    private:
        uint64_t MaxSize;
        uint64_t First;
        uint64_t Last;
        std::vector<T*>Arr;

    public:
        FIFOQueue(uint64_t size):MaxSize(size),Arr(std::vector<T*>(MaxSize,nullptr)),Last(0),First(0) {

        }

        bool Enqueue (T* param) {
            if((Last +1) % MaxSize == First) {
                return false;
            }
            Arr[Last % MaxSize] = param;
            Last = (Last + 1) % MaxSize;
            return true;
        }

        bool Dequeue (T*& param) {
            if(Last == First) {
                return false;
            }

            param = Arr[First % MaxSize];
            First = (First + 1) % MaxSize;
            return true;
        }

        
};