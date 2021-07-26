#pragma once

#include "communicate/Communicator.hpp"

namespace tibus {

class BusMailman {
protected:
    BusMailman()
    {
        //communicate::Communicator::GetReference().
        //    Create<communicate::Publisher>(
        //        , true);
    }

private:
    communicate::Publisher* publisher;
};

}
