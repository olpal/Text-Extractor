//
// Created by AJ Olpin on 2016-11-26.
//

#include "circular_array.h"

template<class T>
Circular_Array<T>::Circular_Array(int insize){
    array_size = insize;
    array = new T [array_size];
    insertion_pos = 0;
}

template<class T>
int Circular_Array<T>::size(){
    return array_size;
}

template<class T>
int Circular_Array<T>::elements(){
    return insertion_pos;
}

template<class T>
void Circular_Array<T>::push_back(T element){
    //If insertion position is outside of the array
    if(insertion_pos == array_size){
        cycle_elements();
        array[insertion_pos-1] = element;
    }
    else {
        array[insertion_pos] = element;
        insertion_pos++;
    }
}

template<class T>
void Circular_Array<T>::cycle_elements() {
    for (int i = 0; i < array_size-1; i++){
        //printf("Replacing: %s with %s \n",array[i].c_str(),array[i+1].c_str());
        array[i] = array[i+1];
    }
}

template<class T>
T Circular_Array<T>::element(int position) {
    if(position >= insertion_pos){
        throw "Out of Bound Exception";
    }
    return array[position];
}

template<class T>
void Circular_Array<T>::cycle_resize(int size) {
    if(size > array_size){
        throw "Invalid Size Exception";
    }
    else if (size == array_size){
        return;
    }
    T * tempArray = array;
    delete [] array;
    array = new T[size];
    //The amount of shift in element copy
    int offset = array_size - size;
    //Copy elements
    for(int i = 0;i< size;i++){
        array[i] = tempArray[i+offset];
    }
    //Set new class variables
    array_size = size;
    insertion_pos = size;
}

template<class T>
void Circular_Array<T>::clear() {
    delete [] array;
    array = new T [array_size];
    insertion_pos = 0;
}



