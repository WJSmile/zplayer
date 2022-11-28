

#ifndef ZPLAYER_IOBSERVER_H
#define ZPLAYER_IOBSERVER_H
#include "XData.h"
#include "XThread.h"
#include <vector>
#include <mutex>
class IObserver : public XThread{
public:
    virtual void Update(XData data){}

    void AddObs(IObserver *obs);

    void Notify(XData data);

protected:
    std::vector<IObserver *>observers;
    std::mutex mux;
};


#endif //ZPLAYER_IOBSERVER_H