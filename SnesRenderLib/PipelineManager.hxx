#pragma once

#include <cstddef>
#include <unordered_map>
#include <utility>

#include "Pipeline.hxx"
#include "PixelFormat.hxx"

namespace maseya::vksnes {
class PipelineManager {
public:
    PipelineManager(std::nullptr_t) noexcept;

    PipelineManager(VkDevice device, VkRenderPass flat_render_pass);

    PipelineManager(const PipelineManager&) = delete;
    PipelineManager(PipelineManager&&) noexcept = default;

    PipelineManager& operator=(const PipelineManager&) = delete;
    PipelineManager& operator=(PipelineManager&&) noexcept = default;

    const Pipeline& overlay_pipeline() const noexcept { return overlay_pipeline_; }
    const Pipeline& line_pipeline() const noexcept { return line_pipeline_; }
    const Pipeline& checker_pipeline() const noexcept { return checker_pipeline_; }
    const Pipeline& gradient_pipeline() const noexcept { return gradient_pipeline_; }
    const Pipeline& palette_pipeline() const noexcept { return palette_pipeline_; }
    const Pipeline& gfx_pipeline(PixelFormat format) const {
        return gfx_pipelines_.at(format);
    }
    const Pipeline& obj_pipeline(bool map16, PixelFormat format) const {
        return obj_pipelines_.at(map16).at(format);
    }
    const Pipeline& map_pipeline(bool map16, bool word_index,
                                 PixelFormat format) const {
        return map_pipelines_.at(map16).at(word_index).at(format);
    }
    const Pipeline& chr_pipeline(PixelFormat format) const {
        return chr_pipelines_.at(format);
    }

    operator bool() const noexcept { return checker_pipeline_; }

private:
    Pipeline overlay_pipeline_;
    Pipeline checker_pipeline_;
    Pipeline gradient_pipeline_;
    Pipeline palette_pipeline_;
    std::unordered_map<PixelFormat, Pipeline> gfx_pipelines_;
    std::unordered_map<bool, std::unordered_map<PixelFormat, Pipeline>> obj_pipelines_;
    std::unordered_map<
            bool, std::unordered_map<bool, std::unordered_map<PixelFormat, Pipeline>>>
            map_pipelines_;
    std::unordered_map<PixelFormat, Pipeline> chr_pipelines_;

    Pipeline line_pipeline_;
};
}  // namespace maseya::vksnes
