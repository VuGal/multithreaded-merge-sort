#include "mergeSortFunctions.h"


/** Merges two subarrays of integer array:
 *  first subarray: from begin to mid
 *  second subarray: from mid+1 to right
 *
 *  @param[in] &array   reference to the array on which the merge algorithm will work
 *  @param[in] left     lower bound of the array
 *  @param[in] mid      middle of the array
 *  @param[in] right    upper bound of the array
 */
void merge(std::vector<int> &array, int const left, int const mid, int const right)
{
    auto const subArrayOne = mid - left + 1;
    auto const subArrayTwo = right - mid;

    // Create temp arrays
    auto *leftArray = new int[subArrayOne],
         *rightArray = new int[subArrayTwo];

    // Copy data to temp arrays leftArray[] and rightArray[]
    for (auto i = 0; i < subArrayOne; i++)
        leftArray[i] = array[left + i];
    for (auto j = 0; j < subArrayTwo; j++)
        rightArray[j] = array[mid + 1 + j];

    auto indexOfSubArrayOne = 0,   // Initial index of first sub-array
        indexOfSubArrayTwo = 0;    // Initial index of second sub-array
    int indexOfMergedArray = left; // Initial index of merged array

    // Merge the temp arrays back into the main array (from "left" to "right")
    while (indexOfSubArrayOne < subArrayOne && indexOfSubArrayTwo < subArrayTwo) {
        if (leftArray[indexOfSubArrayOne] <= rightArray[indexOfSubArrayTwo]) {
            array[indexOfMergedArray] = leftArray[indexOfSubArrayOne];
            indexOfSubArrayOne++;
        }
        else {
            array[indexOfMergedArray] = rightArray[indexOfSubArrayTwo];
            indexOfSubArrayTwo++;
        }
        indexOfMergedArray++;
    }

    // Copy the remaining elements of leftArray[], if there are any
    while (indexOfSubArrayOne < subArrayOne) {
        array[indexOfMergedArray] = leftArray[indexOfSubArrayOne];
        indexOfSubArrayOne++;
        indexOfMergedArray++;
    }

    // Copy the remaining elements of rightArray[], if there are any
    while (indexOfSubArrayTwo < subArrayTwo) {
        array[indexOfMergedArray] = rightArray[indexOfSubArrayTwo];
        indexOfSubArrayTwo++;
        indexOfMergedArray++;
    }
}


/** Performs merge sort algorithm
 *
 *  @param[in] &array   reference to the array on which the merge sort algorithm will work
 *  @param[in] begin    lower bound of the array
 *  @param[in] end      upper bound of the array
 */
void mergeSort(std::vector<int> &array, int const begin, int const end)
{
	if (begin < end) {
		auto mid = begin + (end - begin) / 2;
		mergeSort(array, begin, mid);
		mergeSort(array, mid + 1, end);
		merge(array, begin, mid, end);
	}
}
