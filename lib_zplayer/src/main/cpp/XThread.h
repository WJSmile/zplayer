
#ifndef ZPLAYER_XTHREAD_H
#define ZPLAYER_XTHREAD_H

void XSleep(int ms);

class XThread {
public:
    virtual bool Start();

    virtual void Stop();

    virtual void Main() {}

protected:
    bool isExit = false;
private:
    virtual void TextureMain();
};

#endif //ZPLAYER_XTHREAD_H
