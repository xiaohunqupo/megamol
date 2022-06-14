#ifndef SRC_IMAGESERIES_FILTER_CHORDFILTER_HPP_
#define SRC_IMAGESERIES_FILTER_CHORDFILTER_HPP_

#include "imageseries/AsyncImageData2D.h"

#include <memory>

namespace megamol::ImageSeries::filter {

class ChordFilter {
public:
    using AsyncImagePtr = std::shared_ptr<const AsyncImageData2D>;
    using ImagePtr = std::shared_ptr<const AsyncImageData2D::BitmapImage>;

    struct Input {
        AsyncImagePtr image;
        double threshold = 0.5;
        bool clearEdges = true;
    };

    ChordFilter(Input input);
    ChordFilter(AsyncImagePtr image, double threshold = 0.5, bool clearEdges = true);
    ImagePtr operator()();

    ImageMetadata getMetadata() const;

private:
    Input input;
};

} // namespace megamol::ImageSeries::filter


#endif
