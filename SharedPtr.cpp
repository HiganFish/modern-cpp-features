//
// Created by rjd67 on 2021/2/28.
//

#ifndef BASE_MEMORY_H
#define BASE_MEMORY_H

template<class T>
class Counter
{
public:
	explicit Counter(T* ptr);
	~Counter();
	void Destroy();
	void Dispose();
	void Release();
	Counter* AddRefCopy();
	T* Get();
private:
	T* ptr_;
	int shared_count_;
};

template<class T>
Counter<T>::Counter(T* ptr)
	:
	ptr_(ptr),
	shared_count_(1)
{
}

template<class T>
Counter<T>::~Counter()
= default;

template<class T>
void Counter<T>::Destroy()
{
	delete this;
}

template<class T>
void Counter<T>::Release()
{
	shared_count_--;
	if (shared_count_ == 0)
	{
		Dispose();
		Destroy();
	}
}

template<class T>
Counter<T>* Counter<T>::AddRefCopy()
{
	shared_count_++;
	return this;
}

template<class T>
void Counter<T>::Dispose()
{
	delete ptr_;
}

template<class T>
T* Counter<T>::Get()
{
	return ptr_;
}

template<class T>
class SharedPtr
{
public:
	SharedPtr();
	explicit SharedPtr(T* ptr);
	~SharedPtr();
	T* Get();
	void Reset(T* ptr);
	SharedPtr(const SharedPtr&);
	SharedPtr<T>& operator=(const SharedPtr&);
	T& operator*();
	void Swap(SharedPtr& shared_ptr);
private:
	T* ptr_;
	Counter<T>* counter_;
};

template<class T>
SharedPtr<T>::SharedPtr()
	:
	ptr_(nullptr),
	counter_(new Counter<T>(nullptr))
{

}

template<class T>
SharedPtr<T>::SharedPtr(T* ptr)
	:
	ptr_(ptr),
	counter_(new Counter<T>(ptr))
{

}

template<class T>
SharedPtr<T>::~SharedPtr()
{
	counter_->Release();
}
template<class T>
T* SharedPtr<T>::Get()
{
	return ptr_;
}

template<class T>
void SharedPtr<T>::Reset(T* ptr)
{
	SharedPtr<T>(ptr).Swap(*this);
}

template<class T>
void SharedPtr<T>::Swap(SharedPtr& shared_ptr)
{
	std::swap(ptr_, shared_ptr.ptr_);
	std::swap(counter_, shared_ptr.counter_);
}

template<class T>
SharedPtr<T>::SharedPtr(const SharedPtr& shared_ptr)
{
	Counter<T>* other_counter = shared_ptr.counter_;

	counter_ = other_counter->AddRefCopy();
	ptr_ = other_counter->Get();
}

template<class T>
SharedPtr<T>& SharedPtr<T>::operator=(const SharedPtr& shared_ptr)
{
	Counter<T>* other_counter = shared_ptr.counter_;
	if (counter_ != other_counter)
	{
		counter_->Release();
		counter_ = other_counter->AddRefCopy();
		ptr_ = other_counter->Get();
	}
	return *this;
}

template<class T>
T& SharedPtr<T>::operator*()
{
	return *ptr_;
}
#endif //BASE_MEMORY_H