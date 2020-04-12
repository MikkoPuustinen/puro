#pragma once

template <typename FloatType, class ProcessorType, class ProcessorContextType>
class SoundObject
{
public:
    SoundObject(int offset, int lengthInSamples, ProcessorType processor) 
        : processor(processor)
        , offset(offset)
        , remaining(lengthInSamples)
    {
    }

    void addNextOutput(const Buffer<FloatType>& outputBuffer, ProcessorContextType& context)
    {
        if (depleted())
            return;

        // local copy for possible mutating
        Buffer<FloatType> buffer = outputBuffer;

        // no operations needed for this block
        if (offset >= buffer.size())
        {
            offset -= buffer.size();
            return;
        }

        if (offset > 0)
            buffer.trimBegin(offset);

        // restrict range if grain should terminate this block
        if (remaining < buffer.size())
            buffer.trimLength(remaining);

        const int numSamplesRequested = buffer.size();
        processor.next(buffer, context);

        // if grain changed the numSamples, one of the sources was depleted
        if (buffer.size()!= numSamplesRequested)
        {
            remaining = 0;
            return;
        }

        remaining -= numSamplesRequested;
        offset = 0;
    }

    ProcessorType* getProcessor() { return &processor; };
    bool depleted() { return remaining <= 0; }
    void terminate() { remaining = 0; }

private:
    ProcessorType processor;
    int offset = 0;
    int remaining;
};

template <typename FloatType, class SoundObjectType, class ProcessorType, class ProcessorContextType, class PoolType>
class SoundObjectEngine
{
public:
    SoundObjectEngine() = default;

    void addNextOutput(Buffer<FloatType>& output)
    {
        for (auto& it : pool)
        {
            it->addNextOutput(output, context);

            if (it->depleted())
                pool.remove(it);
        }
    }

    template <typename... ProcessorArgs>
    SoundObjectType* addSound(int offsetFromBlockStart, int lengthInSamples, ProcessorArgs... processorArgs)
    {
        SoundObjectType* s = pool.allocate();
        if (s != nullptr)
        {
            new (s) SoundObjectType (offsetFromBlockStart, lengthInSamples, ProcessorType(processorArgs...));

            return s;
        }

        return nullptr;
    }

    template <typename... ProcessorArgs>
    SoundObjectType* addAndRunSound(Buffer<FloatType>& output,
                                    int offsetFromBlockStart,
                                    int lengthInSamples,
                                    ProcessorArgs... processorArgs)
    {
        auto* s = addSound(offsetFromBlockStart, lengthInSamples, processorArgs);

        if (s != nullptr)
        {
            s->addNextOutpupt(output, context);
            return s;
        }

        return nullptr;
    }

    ProcessorContextType context;
    PoolType pool;
};


