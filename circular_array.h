//
// Created by AJ Olpin on 2016-11-26.
//

#ifndef PROJECT_CIRCULAR_ARRAY_H
#define PROJECT_CIRCULAR_ARRAY_H

template<class T>
class Circular_Array{
    private:
        T *array;
        int array_size;
        int insertion_pos;

       /*
        * Shifts all elements forward by one position, dropping the first element
        */
        void cycle_elements();

    public:
        /*
         * Constructor for the class
         * @param insize - the size of the array to create
         */
        Circular_Array<T>(int insize);

        /*
         * Get the size of the array
         * @returns integer representing the current size
         */
        int size();

        /*
         * Gets the number of elements in the array
         * @returns number of elements in the array
         */
        int elements();

        /*
         * Pushes an element onto the back of the array, moves all elements
         * forward if maximum size is reached, dropping the first element
         * @param T element to push into the array
         */
        void push_back(T element);

        /*
         * Retrieves the element at the provided index
         * @param position - the position of the element to return
         */
        T element(int position);

        /*
         * Cycle the array elements and reduce the size of the array by a particular value
         * @param size - new size of the array
         */
        void cycle_resize(int size);

        /*
         * Erase the current array and create a new one
         */
        void clear();

};


#endif //PROJECT_CIRCULAR_ARRAY_H
