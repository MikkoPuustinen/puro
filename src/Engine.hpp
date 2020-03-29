#pragma once

#include <vector>
#include "Buffer.hpp"

template <class GrainType>
class EngineInterface
{
public:
    virtual GrainType* allocateGrain() = 0;
};

template <typename FloatType, class BufferType, class GrainType, class PoolType, class ControllerType>
class EngineTemplate : public EngineInterface<GrainType>
{
public:

    EngineTemplate(ControllerType& s)
        : scheduler(s)
    {
        scheduler.setEngine(this);
    }

    void tick(FloatType* output, int numSamples)
    {
        // scheduler operation
        scheduler.tick(numSamples);

        // grain operations
        for (auto& it : pool)
        {
            //it->addNextOutput(output, n);
            it->getNextOutput(audioBuffer.getRaw(), envelopeBuffer.getRaw(), numSamples);

            for (int i=0; i<numSamples; i++)
            {
                output[i] += audioBuffer[i] * envelopeBuffer[i];
            }

            if (it->terminated())
            {
                pool.remove(it);
            }
        }
    }

    GrainType* allocateGrain() override
    {
        return pool.allocate();
    }

private:

    BufferType audioBuffer;
    BufferType envelopeBuffer;

    PoolType pool;
    ControllerType& scheduler;
};
