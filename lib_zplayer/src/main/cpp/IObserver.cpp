
#include "IObserver.h"
#include "XLog.h"

void IObserver::AddObs(IObserver *obs) {
    if (!obs) return;
    mux.lock();
    observers.push_back(obs);
    mux.unlock();
}

void IObserver::Notify(XData data) {
    mux.lock();

    for (auto & observer : observers) {
        observer->Update(data);
    }
    mux.unlock();
}
