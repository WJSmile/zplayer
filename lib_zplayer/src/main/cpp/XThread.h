
#ifndef ZPLAYER_XTHREAD_H
#define ZPLAYER_XTHREAD_H

#include <mutex>

void XSleep(int ms);

class XThread {
public:
    virtual bool Start();

    virtual void Stop();

    virtual void Main() {}

protected:
    bool isExit = false;
    std::mutex mux;
private:
    virtual void TextureMain();
};

#endif //ZPLAYER_XTHREAD_H
