#ifndef OPENPOSE__FILESTREAM__W_IMAGE_SAVER_HPP
#define OPENPOSE__FILESTREAM__W_IMAGE_SAVER_HPP

#include <memory> // std::shared_ptr
#include <string>
#include <openpose/thread/workerConsumer.hpp>
#include "imageSaver.hpp"

namespace op
{
    template<typename TDatums>
    class WImageSaver : public WorkerConsumer<TDatums>
    {
    public:
        explicit WImageSaver(const std::shared_ptr<ImageSaver>& imageSaver);

        void initializationOnThread();

        void workConsumer(const TDatums& tDatums);

    private:
        const std::shared_ptr<ImageSaver> spImageSaver;

        DELETE_COPY(WImageSaver);
    };
}





// Implementation
#include <vector>
#include <opencv2/core/core.hpp>
#include <openpose/utilities/errorAndLog.hpp>
#include <openpose/utilities/macros.hpp>
#include <openpose/utilities/pointerContainer.hpp>
#include <openpose/utilities/profiler.hpp>
namespace op
{
    template<typename TDatums>
    WImageSaver<TDatums>::WImageSaver(const std::shared_ptr<ImageSaver>& imageSaver) :
        spImageSaver{imageSaver}
    {
    }

    template<typename TDatums>
    void WImageSaver<TDatums>::initializationOnThread()
    {
    }

    template<typename TDatums>
    void WImageSaver<TDatums>::workConsumer(const TDatums& tDatums)
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
                // Record image(s) on disk
                std::vector<cv::Mat> cvOutputDatas(tDatumsNoPtr.size());
                for (auto i = 0; i < tDatumsNoPtr.size(); i++)
                    cvOutputDatas[i] = tDatumsNoPtr[i].cvOutputData;
                const auto fileName = (!tDatumsNoPtr[0].name.empty() ? tDatumsNoPtr[0].name : std::to_string(tDatumsNoPtr[0].id));
                spImageSaver->saveImages(cvOutputDatas, fileName);
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

    COMPILE_TEMPLATE_DATUM(WImageSaver);
}

#endif // OPENPOSE__FILESTREAM__W_IMAGE_SAVER_HPP
