/*
  ==============================================================================

    DistortionEquation.h
    Created: 29 Jun 2021 12:38:30pm
    Author:  CookiMonstor

  ==============================================================================
*/

#pragma once
#include <JUCEHeader.h>

template <typename SampleType>
class DistortionEquation
{
public:
    static inline SampleType identity(SampleType x, SampleType drive, SampleType warp)
    {
        return x;
    }

    class Sigmoid
    {
    public:

        static inline SampleType tanh(SampleType x, SampleType drive, SampleType warp)
        {
            SampleType driveRemap = juce::jmap(drive,
                static_cast<SampleType>(0.0),
                static_cast<SampleType>(10.0),
                static_cast<SampleType>(1.0),
                static_cast<SampleType>(20.0));

            SampleType output = std::tanh(driveRemap * x);

            output = (static_cast<SampleType>(1.0) - warp) * output + warp * x;

            return output;
        }

        static inline SampleType parabolic(SampleType x, SampleType drive, SampleType warp)
        {
            SampleType driveRemap = juce::jmap(drive,
                static_cast<SampleType>(0.0),
                static_cast<SampleType>(10.0),
                static_cast<SampleType>(1.0),
                static_cast<SampleType>(10.0));
            SampleType absX = std::abs(driveRemap * x);
            SampleType output = static_cast<SampleType>(0.0);

            if (absX <= static_cast<SampleType>(2.0))
            {
                output = driveRemap * x * (static_cast<SampleType>(1.0) - absX * static_cast<SampleType>(0.25));
            }
            else
            {
                std::signbit(driveRemap * x) ? output = -static_cast<SampleType>(1.0)
                    : output = static_cast<SampleType>(1.0);
            }

            output = (static_cast<SampleType>(1.0) - warp) * output + warp * x;

            return output;
        }

        static inline SampleType hyperbolic(SampleType x, SampleType drive, SampleType warp)
        {
            SampleType driveRemap = juce::jmap(drive,
                static_cast<SampleType>(0.0),
                static_cast<SampleType>(10.0),
                static_cast<SampleType>(1.0),
                static_cast<SampleType>(10.0));
            SampleType absX = std::abs(driveRemap * x);
            SampleType den = std::pow(static_cast<SampleType>(1.0) + std::pow(absX, static_cast<SampleType>(2.5)), static_cast<SampleType>(0.4));
            SampleType output = driveRemap * x / den;

            output = (static_cast<SampleType>(1.0) - warp) * output + warp * x;

            return output;
        }


        static inline SampleType asinh(SampleType x, SampleType drive, SampleType warp)
        {
            SampleType driveRemap = juce::jmap(drive,
                static_cast<SampleType>(0.0),
                static_cast<SampleType>(10.0),
                static_cast<SampleType>(1.0),
                static_cast<SampleType>(4.0));
            SampleType output = std::asinh(driveRemap * x);

            output = (static_cast<SampleType>(1.0) - warp) * output + warp * x;

            return output;
        }

        static inline SampleType unbSat1(SampleType x, SampleType drive, SampleType warp)
        {
            SampleType driveRemap = juce::jmap(drive,
                static_cast<SampleType>(0.0),
                static_cast<SampleType>(10.0),
                static_cast<SampleType>(1.0),
                static_cast<SampleType>(20.0));
            SampleType absX = std::abs(drive * x);
            SampleType output = static_cast<SampleType>(1.0) + static_cast<SampleType>(4.0) * driveRemap * absX;
            SampleType num = static_cast<SampleType>(2.0) * driveRemap * x;
            SampleType den = static_cast<SampleType>(1.0) + std::sqrt(output);
            
            output = num / den;

            output = (static_cast<SampleType>(1.0) - warp) * output + warp * x;

            return output;
        }

        static inline SampleType fuzz(SampleType x, SampleType drive, SampleType warp)
        {
            SampleType driveRemap = juce::jmap(drive,
                static_cast<SampleType>(0.0),
                static_cast<SampleType>(10.0),
                static_cast<SampleType>(1.0),
                static_cast<SampleType>(10.0));

            SampleType absX = std::abs(x);
            SampleType output = static_cast<SampleType>(0.0);
            //SampleType xOverAbsX = x / absX;

            x > static_cast<SampleType>(0.0) ?
                output = static_cast<SampleType>(1.0) * (static_cast<SampleType>(1.0) - std::exp(-driveRemap * x))
                : output = static_cast<SampleType>(-1.0) * (static_cast<SampleType>(1.0) - std::exp(driveRemap * x));

            output = (static_cast<SampleType>(1.0) - warp) * output + warp * x;

            return output;
        }

        static inline SampleType softClipper(SampleType x, SampleType drive, SampleType warp)
        {
            SampleType driveRemap = juce::jmap(drive,
                static_cast<SampleType>(0.0),
                static_cast<SampleType>(10.0),
                static_cast<SampleType>(1.0),
                static_cast<SampleType>(10.0));

            SampleType xDrive = x * driveRemap;

            SampleType output = static_cast<SampleType>(0.0);

            output = xDrive < static_cast<SampleType>(-1.0) ? static_cast<SampleType>(-2.0 / 3.0)
                : xDrive > static_cast<SampleType>(1.0) ? static_cast<SampleType>(2.0 / 3.0)
                : xDrive - xDrive * xDrive * xDrive / static_cast<SampleType>(3.0);
                
            output = (static_cast<SampleType>(1.0) - warp) * output + warp * x;

            return output;
        }
    };

    class Symetric
    {
    public:
        static inline SampleType hardclip(SampleType x, SampleType drive, SampleType warp)
        {
            SampleType driveRemap = juce::jmap(drive, static_cast<SampleType>(0.0), static_cast<SampleType>(10.0), static_cast<SampleType>(1.0), static_cast<SampleType>(20.0));
            SampleType output = juce::jlimit(static_cast<SampleType>(-1.0), static_cast<SampleType>(1.0), driveRemap * x);

            output = (static_cast<SampleType>(1.0) - warp) * output + warp * x;

            return output;
        }

        static inline SampleType sin(SampleType x, SampleType drive, SampleType warp)
        {
            SampleType t = juce::jmap(drive, static_cast<SampleType>(0.0), static_cast<SampleType>(10.0), static_cast<SampleType>(1.5) * juce::MathConstants<SampleType>::twoPi, static_cast<SampleType>(200.0) + static_cast<SampleType>(1.5) * juce::MathConstants<SampleType>::twoPi);
            SampleType f = t / juce::MathConstants<SampleType>::twoPi;
            SampleType output = std::sin(f * x);

            output = (static_cast<SampleType>(1.0) - warp) * output + warp * x;

            return output;
        }

        static inline SampleType sinh(SampleType x, SampleType drive, SampleType warp)
        {
            SampleType driveRemap = juce::jmap(drive,
                static_cast<SampleType>(0.0),
                static_cast<SampleType>(10.0),
                static_cast<SampleType>(0.5),
                static_cast<SampleType>(1.5));
            SampleType output = std::sinh(driveRemap * x);

            output = (static_cast<SampleType>(1.0) - warp) * output + warp * x;

            return output;
        }

        static inline SampleType sinhAlt(SampleType x, SampleType drive, SampleType warp)
        {
            SampleType driveRemap = juce::jmap(drive,
                static_cast<SampleType>(0.0),
                static_cast<SampleType>(10.0),
                static_cast<SampleType>(1.0),
                static_cast<SampleType>(2.0));
            SampleType output = std::log(driveRemap * x + std::sqrt(driveRemap * x * driveRemap * x + static_cast<SampleType>(1.0)));

            output = (static_cast<SampleType>(1.0) - warp) * output + warp * x;

            return output;
        }

        static inline SampleType tape(SampleType x, SampleType drive, SampleType warp)
        {
            SampleType driveRemap = juce::jmap(drive,
                static_cast<SampleType>(0.0),
                static_cast<SampleType>(10.0),
                static_cast<SampleType>(1.0),
                static_cast<SampleType>(2.0));

            SampleType xDrive = x * driveRemap;
            SampleType a = static_cast<SampleType>(0.6);
            SampleType aCube = a * a * a;
            SampleType b = static_cast<SampleType>(std::sqrt(aCube/ 3.0));

            SampleType output = xDrive < static_cast<SampleType>(-b) ? xDrive + b - b*b*b / (aCube)
                : xDrive > static_cast<SampleType>(b) ? xDrive - b + b * b * b / (aCube)
                : xDrive * xDrive * xDrive / (aCube);

            output = (static_cast<SampleType>(1.0) - warp) * output + warp * x;

            return output;
        }

        static inline SampleType foldover(SampleType x, SampleType drive, SampleType warp)
        {
            SampleType driveRemap = juce::jmap(drive,
                static_cast<SampleType>(0.0),
                static_cast<SampleType>(10.0),
                static_cast<SampleType>(1.0),
                static_cast<SampleType>(2.0));

            SampleType output = std::sin(static_cast<SampleType>(2.0) * juce::MathConstants<SampleType>::pi * x * driveRemap / static_cast<SampleType>(2.5));

            output = (static_cast<SampleType>(1.0) - warp) * output + warp * x;

            return output;
        }
    };

    class Asymetric
    {
    public:
        static inline SampleType tubeSimulation(SampleType x, SampleType drive, SampleType warp)
        {
            SampleType driveRemap = juce::jmap(drive,
                static_cast<SampleType>(0.0),
                static_cast<SampleType>(10.0),
                static_cast<SampleType>(2.0),
                static_cast<SampleType>(40.0));

            SampleType q = static_cast<SampleType>(-0.2);
            SampleType xMinusQ = x - q;
            SampleType output = (xMinusQ) / (static_cast<SampleType>(1.0) - std::exp(-driveRemap * (xMinusQ))) + q / (static_cast<SampleType>(1.0) - std::exp(driveRemap * q));

            output = (static_cast<SampleType>(1.0) - warp) * output + warp * x;

            return output;
        }

        static inline SampleType distortionSimulation(SampleType x, SampleType drive, SampleType warp)
        {
            SampleType driveRemap = juce::jmap(drive,
                static_cast<SampleType>(0.0),
                static_cast<SampleType>(10.0),
                static_cast<SampleType>(1.0),
                static_cast<SampleType>(10.0));

            SampleType driveX = driveRemap * x;
            SampleType absX = std::abs(driveX);
            SampleType xOverAbsX = x == static_cast<SampleType>(0.0) ? static_cast<SampleType>(0.0) : driveX / absX;
            SampleType output = xOverAbsX * (static_cast<SampleType>(1.0) - std::exp(xOverAbsX*driveX));

            //SampleType q = x * driveRemap / std::abs(x);
            //SampleType signQ;
            //std::signbit(-q) ? signQ = static_cast<SampleType>(-1.0) : signQ = static_cast<SampleType>(-1.0);
            //SampleType z = signQ * (static_cast<SampleType>(1.0) - std::exp(signQ*q));

            //output = z * absX / std::abs(z);

            //output = output * absX / std::abs(output);

            output = (static_cast<SampleType>(1.0) - warp) * output + warp * x;

            return output;
        }

        static inline SampleType diode(SampleType x, SampleType drive, SampleType warp)
        {
            SampleType driveRemap = juce::jmap(drive,
                static_cast<SampleType>(0.0),
                static_cast<SampleType>(10.0),
                static_cast<SampleType>(0.25),
                static_cast<SampleType>(4.0));

            SampleType output = x > static_cast<SampleType>(0.0) ? static_cast<SampleType>(2.0) * driveRemap * x : x / (static_cast<SampleType>(2.0) * driveRemap);

            output = (static_cast<SampleType>(1.0) - warp) * output + warp * x;

            return output;
        }

        static inline SampleType diode2(SampleType x, SampleType drive, SampleType warp)
        {
            SampleType driveRemap = juce::jmap(drive,
                static_cast<SampleType>(0.0),
                static_cast<SampleType>(10.0),
                static_cast<SampleType>(0.25),
                static_cast<SampleType>(4.0));

            //SampleType alpha = static_cast<SampleType>(1.93);
            SampleType beta = static_cast<SampleType>(0.2);
            SampleType output = beta * (std::pow(juce::MathConstants<SampleType>::euler, driveRemap * x) - static_cast<SampleType>(1.0));

            output = (static_cast<SampleType>(1.0) - warp) * output + warp * x;

            return output;
        }
    };

    class Special
    {
    public:
        static inline SampleType halfrect(SampleType x, SampleType drive, SampleType warp)
        {
            SampleType driveRemap = juce::jmap(drive,
                static_cast<SampleType>(0.0),
                static_cast<SampleType>(10.0),
                static_cast<SampleType>(1.0),
                static_cast<SampleType>(4.0));

            SampleType output = x > static_cast<SampleType>(0.0) ? driveRemap * x : static_cast<SampleType>(0.0);

            output = (static_cast<SampleType>(1.0) - warp) * output + warp * x;

            return output;
        }

        static inline SampleType fullrect(SampleType x, SampleType drive, SampleType warp)
        {
            SampleType driveRemap = juce::jmap(drive,
                static_cast<SampleType>(0.0),
                static_cast<SampleType>(10.0),
                static_cast<SampleType>(1.0),
                static_cast<SampleType>(4.0));

            SampleType output = std::abs(driveRemap * x);

            output = (static_cast<SampleType>(1.0) - warp) * output + warp * x;

            return output;
        }
    };


};



