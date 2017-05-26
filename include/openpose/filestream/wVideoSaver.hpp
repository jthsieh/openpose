#ifndef OPENPOSE__FILESTREAM__W_VIDEO_SAVER_HPP
#define OPENPOSE__FILESTREAM__W_VIDEO_SAVER_HPP

#include <memory> // std::shared_ptr
#include <string>
#include <vector>
#include <opencv2/core/core.hpp>
#include <openpose/thread/workerConsumer.hpp>
#include "videoSaver.hpp"

namespace op
{
    template<typename TDatums>
    class WVideoSaver : public WorkerConsumer<TDatums>
    {
    public:
        explicit WVideoSaver(const std::shared_ptr<VideoSaver>& videoSaver);

        void initializationOnThread();

        void workConsumer(const TDatums& tDatums);

    private:
        std::shared_ptr<VideoSaver> spVideoSaver;

        DELETE_COPY(WVideoSaver);
    };
}





// Implementation
#include <openpose/utilities/errorAndLog.hpp>
#include <openpose/utilities/macros.hpp>
#include <openpose/utilities/pointerContainer.hpp>
#include <openpose/utilities/profiler.hpp>
namespace op
{
    template<typename TDatums>
    WVideoSaver<TDatums>::WVideoSaver(const std::shared_ptr<VideoSaver>& videoSaver) :
        spVideoSaver{videoSaver}
    {
    }

    template<typename TDatums>
    void WVideoSaver<TDatums>::initializationOnThread()
    {
    }

    template<typename TDatums>
    void WVideoSaver<TDatums>::workConsumer(const TDatums& tDatums)
    {
        try
        {
            if (checkNoNullNorEmpty(tDatums))
            {
                // Debugging log
                dLog("", Priority::Low, __LINE__, __FUNCTION__, __FILE__);
                // Profiling speed
                const auto profilerKey = Profiler::timerInit(__LINE__, __FUNCTION__, __FILE__);
                // T* to T
                auto& tDatumsNoPtr = *tDatums;
                // Record video(s)
                std::vector<cv::Mat> cvOutputDatas(tDatumsNoPtr.size());
                for (auto i = 0 ; i < cvOutputDatas.size() ; i++)
                    cvOutputDatas[i] = tDatumsNoPtr[i].cvOutputData;
                spVideoSaver->write(cvOutputDatas);
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
            error(e.what(), __LINE__, __FUNCTION__, __FILE__);
        }
    }

    COMPILE_TEMPLATE_DATUM(WVideoSaver);
}

#endif // OPENPOSE__FILESTREAM__W_VIDEO_SAVER_HPP
