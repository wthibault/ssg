//
// Ptr.h
//
// Ptr is a "smart reference counted pointer" (can't use std::shared_ptr with some libs....uh.)
// and we don't want to use boost here
// note: the count is kept in the object not the pointer. 
//
#pragma once

namespace ssg {
  
  template <class T>
    class Ptr {
  public:
    T* operator-> () { return p_; }
    T& operator* ()  { return *p_; }

  Ptr() : p_(0) {} // should we have a default constructor?
  Ptr(T* p)    : p_(p) { if (p_) ++p_->count_; }  // p must not be NULL

    // create from derived class pointers: Ptr<ModelNode> x ( Ptr<Instance> p(new Instance() ) );
    template <class U> Ptr ( const Ptr<U>& p) 
      : p_(dynamic_cast<T*>(p.get())) // safely downcast the pointer
      { if (p_) ++p_->count_; }

    ~Ptr() 
      { 
	//      std::cout << "~Ptr\n";
	if (p_ && --p_->count_ == 0) delete p_; 
      }

    // copy constructor
  Ptr(Ptr const& p) : p_(p.p_) { if (p_) ++p_->count_; }

    Ptr& operator= (Ptr const& p)
      { // DO NOT CHANGE THE ORDER OF THESE STATEMENTS!
	// (This order properly handles self-assignment)
	// (This order also properly handles recursion, e.g., if a T contains Ptrs)
	T* const old = p_;
	p_ = p.p_;
	++p_->count_;
	if (old && (--old->count_ == 0)) delete old;
	return *this;
      }
    T* get() const {return p_;}
  private:
    T* p_;    // p_ is never NULL, except sometimes. 
  };

}
