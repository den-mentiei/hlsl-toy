#pragma once

namespace toy {

template <typename T>
class ComPtr {
public:
	explicit ComPtr(T* p = nullptr);	
	~ComPtr();

	ComPtr(const ComPtr<T>& other);
	ComPtr<T>& operator=(const ComPtr<T>& other);

	T& operator*() const;
	T* operator->() const;
	T** operator&();

	T* get() const;
	void set(T* p);
private:
	T* _p;
};

template <typename T>
ComPtr<T>::ComPtr(T* p = nullptr)
	: _p(p)
{}

template <typename T>
ComPtr<T>::~ComPtr() {
	if (_p) {
		_p->Release();
	}
}

template <typename T>
ComPtr<T>::ComPtr(const ComPtr<T>& other) {
	*this = other;
}

template <typename T>
ComPtr<T>& ComPtr<T>::operator=(const ComPtr<T>& other) {
	_p = other._p;
	if (_p) {
		_p->AddRef();
	}
	return *this;
}

template <typename T>
T& ComPtr<T>::operator*() const {
	return *_p;
}

template <typename T>
T* ComPtr<T>::operator->() const {
	return _p;
}

template <typename T>
T** ComPtr<T>::operator&() {
	if (_p) {
		_p->Release();
		_p = nullptr;
	}
	return &_p;
}

template <typename T>
T* ComPtr<T>::get() const {
	return _p;
}

template <typename T>
void ComPtr<T>::set(T* p) {
	if (_p) {
		_p->Release();
	}
	_p = p;
}

} // namespace toy