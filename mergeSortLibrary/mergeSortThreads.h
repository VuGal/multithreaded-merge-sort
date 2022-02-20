#ifndef MERGESORTLIBRARY_MERGESORTTHREADS_H_
#define MERGESORTLIBRARY_MERGESORTTHREADS_H_

class MergeSortThread : public osapi::MortalThread
{
public:
	MergeSortThread(std::vector<int> &a, int l, int h) : MortalThread(0, 0) {
		arr = &a;
		lower = l;
		higher = h;
		run();
	}

private:

	std::vector<int> *arr;
	int lower;
	int higher;

	virtual void begin()
	{
		;
	}
	virtual void loop()
	{
		mergeSort(*arr, lower, higher);
		kill();
	}
	virtual void end()
	{
		;
	}
};

class MergeThread : public osapi::MortalThread
{
public:
	MergeThread(std::vector<int> &a, int l, int m, int h) : MortalThread(0, 0) {
		arr = &a;
		lower = l;
		mid = m;
		higher = h;
		run();
	}

private:

	std::vector<int> *arr;
	int lower;
	int mid;
	int higher;

	virtual void begin()
	{
		;
	}
	virtual void loop()
	{
		merge(*arr, lower, mid, higher);
		kill();
	}
	virtual void end()
	{
		;
	}
};


#endif /* MERGESORTLIBRARY_MERGESORTTHREADS_H_ */
