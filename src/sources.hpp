#pragma once

/////////////////////////////////////////////
// Audio sources
/////////////////////////////////////////////

template <typename FloatType>
class ConstSource
{
public:

    template <typename BufferType>
    BufferType next(const bops::Type opType, BufferType buffer)
    {
        for (int ch=0; ch < buffer.getNumChannels(); ++ch)
        {
            FloatType* dst = buffer.channel(ch);
            
            for (int i=0; i<buffer.size(); ++i)
            {
                if (opType == bops::Type::add) dst[i] += 1.0;
                else dst[i] = 1.0;
            }
        }

        return buffer;
    }
};

template <typename FloatType>
class NoiseSource
{
public:

    template <typename BufferType>
    BufferType next(const bops::Type opType, BufferType buffer)
    {
        for (int ch=0; ch < buffer.numChannels; ++ch)
        {
            FloatType* dst = buffer.channel(ch);
            
            for (int i=0; i<buffer.numSamples; ++i)
            {
                const FloatType coef = static_cast<FloatType> (1) / static_cast<FloatType> (RAND_MAX/2);
                const FloatType r = static_cast<FloatType> (std::rand()) * coef - 1;

                if (opType == bops::Type::add) dst[i] += r;
                else dst[i] = r;
            }
        }

        return buffer;
    }
};

/////////////////////////////////////////////
// Envelope sources
/////////////////////////////////////////////

template <class FloatType>
class SineEnvelope
{
public:

    SineEnvelope(int lengthInSamples)
        : increment(math::pi<FloatType>() / static_cast<FloatType>(lengthInSamples-1))
        , position(increment)
    {
    }

    template <typename BufferType>
    BufferType next(const bops::Type opType, BufferType buffer)
    {
        // do sample-wise
        if (opType == bops::Type::add)
        {
            for (int i=0; i<buffer.size(); ++i)
            {
                const FloatType sample = sin(position);

                for (int ch=0; ch < buffer.getNumChannels(); ++ch)
                {
                    buffer.channel(ch)[i] = sample;
                }

                position += increment;
            }
        }
        else // bops::Type::replace
        {
            // do vectorised
            FloatType* env = buffer.channel(0);

            for (int i=0; i<buffer.size(); ++i)
            {
                env[i] = position;
                position += increment;
            }

            math::sin(env, buffer.size());

            // copy to other channels
            for (int ch=1; ch < buffer.getNumChannels(); ++ch)
            {
                FloatType* dst = buffer.channel(ch);
                math::copy(dst, env, buffer.size());
            }
        }

        return buffer;
    }

private:

    const FloatType increment;
    FloatType position;
};


template <class FloatType>
class HannEnvelope
{
public:

    HannEnvelope(int lengthInSamples, bool isSymmetric=true)
        : increment(2 * math::pi<FloatType>()
                    / static_cast<FloatType>(lengthInSamples + (isSymmetric ? 1 : 0)))
        , position(increment)
    {
    }

    template <typename BufferType>
    BufferType next(const bops::Type opType, BufferType buffer)
    {
        // do sample-wise
        if (opType == bops::Type::add)
        {
            const auto startingPos = position;

            for (int ch=0; ch < buffer.getNumChannels(); ++ch)
            {
                position = startingPos;
                FloatType* channel = buffer.channel(ch);

                for (int i=0; i<buffer.size(); ++i)
                {
                    const FloatType sample = (1 - cos(position)) / 2;
                    channel[i] = sample;

                    position += increment;
                }
            }
        }
        else
        {
            // do vectorised
            FloatType* env = buffer.channel(0);

            for (int i=0; i<buffer.size(); ++i)
            {
                const FloatType sample = (1 - cos(position)) / 2;
                env[i] = sample;
                position += increment;
            }

            for (int ch=1; ch < buffer.getNumChannels(); ++ch)
            {
                FloatType* dst = buffer.channel(ch);
                math::copy(dst, env, buffer.size());
            }
        }

        return buffer;
    }

private:

    const FloatType increment;
    FloatType position;
};



/*
template <typename FloatType>
class AudioBufferSource
{
public:
    AudioBufferSource(Buffer<FloatType>& fileBuffer, int startIndex)
        : sourceBuffer(fileBuffer)
        , index(startIndex)
    {
    }

    void next(Buffer<FloatType>& buffer, ops::Type opType)
    {
        // buffer will run out, trim it
        if (sourceBuffer.size()< index + buffer.size())
        {
            buffer.trimLength(sourceBuffer.size() - index);
        }

        // identical channel config
        if (buffer.getNumChannels() == sourceBuffer.getNumChannels())
        {
            for (int ch=0; ch<buffer.getNumChannels(); ++ch)
            {
                FloatType* dst = buffer.channel(ch);
                FloatType* src = &sourceBuffer.channel(ch)[index];

                if (opType == ops::Type::add)
                    math::add<FloatType>(dst, src, buffer.size());
                else
                    math::copy<FloatType>(dst, src, buffer.size());
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

                if (opType == ops::Type::add)
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

*/
