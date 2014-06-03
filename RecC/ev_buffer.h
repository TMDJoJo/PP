/*
 * ev_buffer.h
 *
 *  Created on: May 16, 2014
 *      Author: tmdjojo
 */

#ifndef EV_BUFFER_H_
#define EV_BUFFER_H_

#define NULL 0

class EvBuffer{
public:
    EvBuffer(int max_buffer_size);
    virtual ~EvBuffer();
    virtual int Peek(char*,int size);
    virtual int Read(char*,int size);
    virtual int Write(const char*,int size);

    int Clean();
    inline int buffer_size(){return buffer_length_;}
    inline int buffer_free_size(){return max_buffer_size_ - buffer_length_;}
private:
    char* data_;
    const int max_buffer_size_;
    int buffer_head_;
    int buffer_tail_;
    int buffer_length_;
};

#endif /* EV_BUFFER_H_ */
