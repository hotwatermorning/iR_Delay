#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <iostream>
#include <iomanip>
#include <memory>

template<class T>
class RingBuffer
{
public:
  // constructor
  RingBuffer() = delete;
  RingBuffer(int capacity);

  // destructor
  ~RingBuffer();

  int getCapacity();
  int getLength();
  void setMaxLength(int new_max_length);
  int getMaxLength();
  bool isFull();
  bool isEmpty();
  void clear();

  void push(T data);
  void pop();
  T getFront(int index = 0);
  T getBack(int index = 0);

  // test
  void dump();
  void dump_all();

private:
  std::unique_ptr<T[]> buffer_;
  int capacity_;
  int length_;
  int max_length_;
  int read_point_, write_point_;

  void setCapacity(int new_capacity);
  void moveForward();
  int roundUpPow2(int num);
  int normalizedIndex(int index);
};

template<class T>
RingBuffer<T>::RingBuffer(int max_length) :
  buffer_(std::make_unique<T[]>(roundUpPow2(max_length))),
  capacity_(roundUpPow2(max_length)),
  length_(0),
  max_length_(max_length),
  read_point_(0),
  write_point_(0)
{}

template<class T>
RingBuffer<T>::~RingBuffer()
{
  buffer_.reset();
}

template<class T>
void RingBuffer<T>::setCapacity(int new_capacity)
{
  capacity_ = roundUpPow2(new_capacity);
  length_ = read_point_ = write_point_ = 0;
  buffer_.reset(new T[roundUpPow2(new_capacity)]);
}

template<class T>
void RingBuffer<T>::clear()
{
  length_ = read_point_ = write_point_ = 0;
  buffer_.reset(new T[capacity_]);
}

template<class T>
int RingBuffer<T>::getCapacity() { return capacity_; }

template<class T>
int RingBuffer<T>::getLength() { return length_; }

template<class T>
void RingBuffer<T>::setMaxLength(int new_max_length)
{
  setCapacity(new_max_length);
  max_length_ = new_max_length;
}

template<class T>
int RingBuffer<T>::getMaxLength() { return max_length_; }

template<class T>
bool RingBuffer<T>::isFull() { return length_ == max_length_; }

template<class T>
bool RingBuffer<T>::isEmpty() { return length_ == 0; }

template<class T>
void RingBuffer<T>::push(T data)
{
  moveForward();
  if (max_length_ > length_ && !isFull())
    length_++;
  buffer_.get()[write_point_] = data;
  write_point_ = normalizedIndex(write_point_ + 1);
}

template<class T>
void RingBuffer<T>::pop()
{
  if (length_ == max_length_) {
    write_point_ = normalizedIndex(write_point_ + 1);
    T result = buffer_.get()[normalizedIndex(read_point_)];
    moveForward();
    return result;
  }
  return 0;
}

template<class T>
T RingBuffer<T>::getFront(int index)
{
  if (index < length_)
    return buffer_.get()[normalizedIndex(read_point_ + index)];
  return 0;
}

template<class T>
T RingBuffer<T>::getBack(int index)
{
  if (index < length_)
    return buffer_.get()[normalizedIndex(read_point_ + length_ - index - 1)];
  return 0;
}

template<class T>
void RingBuffer<T>::moveForward()
{
  if (length_ == max_length_)
    read_point_ = normalizedIndex(read_point_ + 1);
}

template<class T>
int RingBuffer<T>::roundUpPow2(int num)
{
  num--;
  for (int i = 1; i < 32; i *= 2) num |= (num >> i);
  num++;
  return num;
}

template<class T>
int RingBuffer<T>::normalizedIndex(int index)
{
  return index & (capacity_ - 1);
}

// test
template<class T>
void RingBuffer<T>::dump()
{
  for (int i = 0; i < length_; i++) {
    std::cout << buffer_.get()[normalizedIndex(i + read_point_)] << "  ";
  }
  std::cout << std::endl;
}

template<class T>
void RingBuffer<T>::dump_all()
{
  for (int i = 0; i < capacity_; i++) {
    std::cout << buffer_.get()[i] << "  ";
  }
  std::cout << std::endl;
}

#endif