#ifndef OPENPOSE__CORE__W_CV_MAT_TO_OP_OUTPUT_HPP
#define OPENPOSE__CORE__W_CV_MAT_TO_OP_OUTPUT_HPP

#include <memory> // std::shared_ptr
#include <opencv2/core/core.hpp>
#include <openpose/thread/worker.hpp>
#include "cvMatToOpOutput.hpp"

namespace op
{
    template<typename TDatums>
    class WCvMatToOpOutput : public Worker<TDatums>
    {
    public:
        explicit WCvMatToOpOutput(const std::shared_ptr<CvMatToOpOutput>& cvMatToOpOutput);

        void initializationOnThread();

        void work(TDatums& tDatums);

    private:
        const std::shared_ptr<CvMatToOpOutput> spCvMatToOpOutput;

        DELETE_COPY(WCvMatToOpOutput);
    };
}





// Implementation
#include <openpose/utilities/errorAndLog.hpp>
#include <openpose/utilities/macros.hpp>
#include <openpose/utilities/openCv.hpp>
#include <openpose/utilities/pointerContainer.hpp>
#include <openpose/utilities/profiler.hpp>
namespace op
{
    template<typename TDatums>
    WCvMatToOpOutput<TDatums>::WCvMatToOpOutput(const std::shared_ptr<CvMatToOpOutput>& cvMatToOpOutput) :
        spCvMatToOpOutput{cvMatToOpOutput}
    {
    }

    template<typename TDatums>
    void WCvMatToOpOutput<TDatums>::initializationOnThread()
    {
    }

    template<typename TDatums>
    void WCvMatToOpOutput<TDatums>::work(TDatums& tDatums)
    {
        try
        {
            if (checkNoNullNorEmpty(tDatums))
            {
                // Debugging log
                dLog("", Priority::Low, __LINE__, __FUNCTION__, __FILE__);
                // T* to T
                auto& tDatumsNoPtr = *tDatums;
                // Profiling speed
                const auto profilerKey = Profiler::timerInit(__LINE__, __FUNCTION__, __FILE__);
                // cv::Mat -> float*
                for (auto& tDatum : tDatumsNoPtr)
                    std::tie(tDatum.scaleInputToOutput, tDatum.outputData) = spCvMatToOpOutput->format(tDatum.cvInputData);
                // Profiling speed
                Profiler::timerEnd(profilerKey);
                Profiler::printAveragedTimeMsOnIterationX(profilerKey, __LINE__, __FUNCTION__, __FILE__, Profiler::DEFAULT_X);
                // Debugging log
                dLog("", Priority::Low, __LINE__, __FUNCTION__, __FILE__);
            }
        }
        catch (const std::exception& e)
        {
            this->stop();
            tDatums = nullptr;
            error(e.what(), __LINE__, __FUNCTION__, __FILE__);
        }
    }

    COMPILE_TEMPLATE_DATUM(WCvMatToOpOutput);
}

#endif // OPENPOSE__CORE__W_CV_MAT_TO_OP_OUTPUT_HPP
