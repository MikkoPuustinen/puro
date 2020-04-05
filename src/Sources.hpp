#pragma once

/////////////////////////////////////////////
// Audio sources
/////////////////////////////////////////////

template <typename FloatType>
class ConstSource
{
public:

    void next(Buffer<FloatType>& buffer, SourceOperations::Type opType)
    {
        for (int ch=0; ch < buffer.numChannels; ++ch)
        {
            FloatType* dst = buffer.channels[ch];
            
            for (int i=0; i<buffer.numSamples; ++i)
            {
                if (opType == SourceOperations::Type::add) dst[i] += 1.0;
                else dst[i] = 1.0;
            }
        }
    }
};

template <typename FloatType>
class NoiseSource
{
public:
    void next(Buffer<FloatType>& buffer, SourceOperations::Type opType)
    {
        for (int ch=0; ch < buffer.numChannels; ++ch)
        {
            FloatType* dst = buffer.channels[ch];
            
            for (int i=0; i<buffer.numSamples; ++i)
            {
                const FloatType coef = static_cast<FloatType> (1) / static_cast<FloatType> (RAND_MAX/2);
                const FloatType r = static_cast<FloatType> (std::rand()) * coef - 1;

                if (opType == SourceOperations::Type::add) dst[i] += r;
                else dst[i] = r;
            }
        }
    }
};

/////////////////////////////////////////////
// Audio sources
/////////////////////////////////////////////

template <class FloatType>
class SineEnvelope
{
public:

    SineEnvelope(int lengthInSamples)
        : increment(Math::pi<FloatType>() / static_cast<FloatType>(lengthInSamples-1))
        , position(increment)
    {
    }

    void next(Buffer<FloatType>& buffer, SourceOperations::Type opType)
    {
        // do sample-wise
        if (opType == SourceOperations::Type::add)
        {
            for (int i=0; i<buffer.size(); ++i)
            {
                for (int ch=0; ch < buffer.getNumChannels(); ++ch)
                {
                    const FloatType sample = sin(position);
                    buffer.channel(ch)[i] = sample;
                }

                position += increment;
            }
        }
        else
        {
            // do vectorised
            FloatType* env = buffer.channel(0);

            for (int i=0; i<buffer.size(); ++i)
            {
                env[i] = position;
                position += increment;
            }

            Math::sin(env, buffer.size());

            for (int ch=1; ch < buffer.getNumChannels(); ++ch)
            {
                FloatType* dst = buffer.channel(ch);
                Math::copy(dst, env, buffer.size());
            }
        }
    }

private:

    const FloatType increment;
    FloatType position;
};



template <class FloatType>
class HannEnvelope
{
public:

    HannEnvelope(int lengthInSamples)
        : increment(2 * Math::pi<FloatType>() / static_cast<FloatType>(lengthInSamples-1))
        , position(increment)
    {
    }


    void next(Buffer<FloatType>& buffer, SourceOperations::Type opType)
    {
        // do sample-wise
        if (opType == SourceOperations::Type::add)
        {
            for (int i=0; i<buffer.numSamples; ++i)
            {
                for (int ch=0; ch < buffer.numChannels; ++ch)
                {
                    const FloatType sample = (1 - cos(position)) / 2;
                    buffer.channels[ch][i] = sample;
                }

                position += increment;
            }
        }
        else
        {
            // do vectorised
            FloatType* env = buffer.channels[0];

            for (int i=0; i<buffer.numSamples; ++i)
            {
                const FloatType sample = (1 - cos(position)) / 2;
                env[i] = sample;
                position += increment;
            }

            for (int ch=1; ch < buffer.numChannels; ++ch)
            {
                FloatType* dst = buffer.channels[ch];
                Math::copy(dst, env, buffer.numSamples);
            }
        }
    }

private:

    const FloatType increment;
    FloatType position;
};



template <typename FloatType>
class AudioBufferSource
{
public:
    AudioBufferSource(Buffer<FloatType>& fileBuffer, int startIndex)
        : sourceBuffer(fileBuffer)
        , index(startIndex)
    {
    }

    void next(Buffer<FloatType>& buffer, SourceOperations::Type opType)
    {
        // buffer will run out, trim it
        if (sourceBuffer.size()< index + buffer.size())
        {
            buffer.trimLength(sourceBuffer.size() - index);
        }

        // identical channel config
        if (buffer.getNumChannels() == sourceBuffer.getNumChannels())
        {
            for (int ch=0; ch<buffer.size(); ++ch)
            {
                FloatType* dst = buffer.channel(ch);
                FloatType* src = &sourceBuffer.channel(ch)[index];

                if (opType == SourceOperations::Type::add)
                    Math::add<FloatType>(dst, src, buffer.size());
                else
                    Math::copy<FloatType>(dst, src, buffer.size());
            }
            index += buffer.size();
        }
        // mono source, use for all channels
        else if (sourceBuffer.getNumChannels() == 1)
        {
            FloatType* src = &sourceBuffer.channel(0)[index];

            for (int ch=0; ch<buffer.getNumChannels(); ++ch)
            {
                FloatType* dst = buffer.channel(ch);

                if (opType == SourceOperations::Type::add)
                    Math::add(dst, src, buffer.size());
                else
                    Math::copy(dst, src, buffer.size());
            }
            index += buffer.size();
        }
        else
        {
            errorif(true, "channel config combination not implemented");
        }

    }

private:
    int index;
    Buffer<FloatType>& sourceBuffer;
};

